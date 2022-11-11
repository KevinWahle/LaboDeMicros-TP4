/***************************************************************************//**
  @file     Magtek.h
  @brief    Mangetic stripe reader
  @author   Group 5
 ******************************************************************************/


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Magtek.h"
#include <stdlib.h>
#include <stdint.h>
#include "MCAL/gpio.h"
// #include "timer/timer.h"

#include <os.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// #define ENABLE_TP

#define DATA	PORTNUM2PIN(PA,1)
#define CLK		PORTNUM2PIN(PB,9)
#define ENABLE	PORTNUM2PIN(PC,17)

#ifdef	ENABLE_TP
#define TESTPIN PORTNUM2PIN(PB,2)
#endif

#define CHAR_0 		0b00001
#define CHAR_1 		0b10000
#define CHAR_2 		0b01000
#define CHAR_3		0b11001
#define CHAR_4 		0b00100
#define CHAR_5 		0b10101
#define CHAR_6 		0b01101
#define CHAR_7 		0b11100
#define CHAR_8 		0b00010
#define CHAR_9 		0b10011
#define COLON 		0b01011
#define SEMICOLON 	0b11010
#define LESS 		0b00111
#define EQUAL		0b10110
#define GREATER 	0b01110
#define QUESTION 	0b11111

#define RESET	0



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	uint8_t buff :5;
	uint8_t  nth :3;
}buffer;


enum {SS, PAN, FS, ADDIT, DISCR, ES, ERROR, NOTDATA};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// Callback called when clock interrupts. Reads data
static void clkCb ();

// Callback called when enable interrupts. Cancells reading
static void enCb (void *p_arg);

// Process the data stored in the buffer
static void readbuff();

// Save character on the corresponding string
static void write(uint32_t position, uint8_t status, char character);

// Clear all strings
static void clear();

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static buffer mybuffer;
static uint32_t buff_count;
static data mydata;
static cardCb mainCb;
// static tim_id_t timer_id;
static uint8_t status;
static bool read;


/* Display Task */
#define TASK_STK_SIZE			256u
#define TASK_STK_SIZE_LIMIT	(TASK_STK_SIZE / 10u)
#define TASK_PRIO              3u
static OS_TCB magtekTask;
static CPU_STK TaskStk[TASK_STK_SIZE];

static OS_SEM magtekSem;

static OS_ERR os_err;




/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool CardInit(cardCb funCb){
	if (funCb==NULL)
		return false;


	gpioMode (CLK,INPUT);		// Set GPIO
	gpioMode (DATA,INPUT);
	gpioMode (ENABLE,INPUT);

#ifdef ENABLE_TP
	gpioMode(TESTPIN, OUTPUT);
#endif

	// timerInit();

	gpioIRQ(CLK, GPIO_IRQ_MODE_FALLING_EDGE, clkCb);	//Set clock falling edge interruption

	// timer_id = timerGetId();

	mainCb=funCb;

	mybuffer.buff=0b11111;		// 5 digit buffer
	mybuffer.nth=0b000;

	read=false;					// Reset variables
	buff_count=RESET;
	status = SS;

    // Semaforo para habilitar chequeo periodico
    OSSemCreate(&magtekSem, "Magtek Sem", 0u, &os_err);

    /* Create Task */
    OSTaskCreate(&magtekTask, 			//tcb
                 "Magtek Task",			//name
                  enCb,					//func
                  0u,					//arg
                  TASK_PRIO,			//prio
                 &TaskStk[0u],			//stack
                  TASK_STK_SIZE_LIMIT,	//stack limit
                  TASK_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

	return true;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void clkCb (){

#ifdef ENABLE_TP
	gpioWrite(TESTPIN, HIGH);
#endif

	buffer tempbuf = {.buff=(mybuffer.buff<<1), .nth=0b000};
	mybuffer.buff = tempbuf.buff | !(gpioRead(DATA));			// Add new data to buffer
	buff_count++;
	if (buff_count==5){
		read=true;												// Read every 5 bits (1 char)
		buff_count=RESET;
	}
	readbuff();

#ifdef ENABLE_TP
	gpioWrite(TESTPIN, LOW);
#endif

}

void enCb (void *p_arg){

	while (1) {

		if (gpioRead(ENABLE)){
			status=SS;
			OSSemPend(&magtekSem, 0U, OS_OPT_PEND_BLOCKING, NULL, &os_err);		// Espero reactivacion
		}
		else {
			OSTimeDlyHMSM(0U, 0U, 0U, 100U, OS_OPT_TIME_DLY, &os_err);	// PISR time
		}

	}
}

void readbuff(){
	static uint32_t i=RESET;

	if (mybuffer.buff==SEMICOLON && status==SS)			// ; starts the reading
	{
		// timerStart(timer_id, TIMER_MS2TICKS(100), TIM_MODE_PERIODIC, enCb);
		OSSemPost(&magtekSem, OS_OPT_POST_ALL, &os_err);	// Enable periodic check
		i=RESET;
		buff_count=RESET;
		read=false;
		status=PAN;
	}

	if (status==PAN || status==ADDIT || status==DISCR){
		if (read){									// Read every 5 bits (1 char)
				read=false;
				switch (mybuffer.buff) {
					case CHAR_0:
						write(i, status, '0');
						break;
					case CHAR_1:
						write(i, status, '1');
						break;
					case CHAR_2:
						write(i, status, '2');
						break;
					case CHAR_3:
						write(i, status, '3');
						break;
					case CHAR_4:
						write(i, status, '4');
						break;
					case CHAR_5:
						write(i, status, '5');
						break;
					case CHAR_6:
						write(i, status, '6');
						break;
					case CHAR_7:
						write(i, status, '7');
						break;
					case CHAR_8:
						write(i, status, '8');
						break;
					case CHAR_9:
						write(i, status, '9');
						break;
					case COLON:
						status=ERROR;		//There should not be colon
						break;
					case SEMICOLON:
						status=ERROR;		//There should not be semicolon
						break;
					case LESS:
						status=ERROR;		//There should not be less
						break;
					case EQUAL:
						status=ADDIT;		// = is the Additional separator
						buff_count=RESET;
						i=RESET-1;
						break;
					case GREATER:
						status=ERROR;		//There should not be colon
						break;
					case QUESTION:
						status=ES;			// ? is the end separator
						i=RESET-1;
						break;
					default:
						status=ERROR;
						break;
				}

			if (i>=PAN_MAX_L && status==PAN)	// If maximum Pan length is exceeded
				status=ERROR;

			if (i>=DISCR_L && status==DISCR){	// If maximum Discretionary length is exceeded
				status=ES;
			}

			if (i>=ADDIT_L-1 && status==ADDIT){	//Switch from Additional to Discretioary
				status=DISCR;
				i=RESET-1;
			}

			i++;

		}

		if (status==ES || status==ERROR){		// If end or Error return
			if (status==ES){
				mainCb(true, &mydata);
				//OSQPost(&magtekQ, mydata.id, PAN_MAX_L, OS_OPT_POST_ALL, &os_err);

			}
			if  (status==ERROR){
				mainCb(false, NULL);
				//OSQPost(&magtekQ, NULL, 0, OS_OPT_POST_ALL, &os_err);
			}
			clear();							//Clear matrix and reset variables
			status = NOTDATA;
			i=RESET;
		}

	}



}

void write(uint32_t position, uint8_t status, char character){
	if (status==PAN){
		if (position<PAN_MAX_L){
			mydata.id[position]=character;
		}
	}
	else if (status==ADDIT){
		if(position<ADDIT_L){
			mydata.additional[position]=character;
		}
	}
	else if (status==DISCR){
		if(position<DISCR_L){
			mydata.discretionary[position]=character;

		}
	}
}

void clear(){
	for (int i = 0; i < PAN_MAX_L; i++)
		mydata.id[i]=0;
	for (int i = 0; i < ADDIT_L; i++)
		mydata.additional[i]=0;
	for (int i = 0; i < DISCR_L; i++)
		mydata.discretionary[i]=0;
}
