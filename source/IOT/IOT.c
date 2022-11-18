/***************************************************************************//**
  @file     IOT.c
  @brief	Funciones para comunicación con la nube
  @author	Grupo 5
  @date		12 nov. 2022
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "IOT.h"
#include "../UART/uartOS.h"
// #include "../LEDMux/LEDMux.h"
#include "MCAL/gpio.h"

#include <os.h>
#include <stddef.h>
//#include <stdio.h>
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_ID			0
#define UART_BAUDRATE	1200
#define UART_TIMEOUT	1000	//ms

#define HEADER_STR	{0xAA, 0x55, 0xC3, 0x3C}

#define SENDDATA_CODE	0x01
#define DATAOK_CODE		0x81
#define DATAFAIL_CODE	0xC1
#define KEEPALIVE_CODE	0x02
#define KEEPALOK_CODE	0x82

#define ANSWER_LENGTH	6

#define KEEPALIVE_TIME	3	// seconds
#define IOTINTERVAL_TIME	20	// seconds

#define KEEPALIVE_LED	PORTNUM2PIN(PE, 26)

#define FLOOR_COUNT		3

#define S_2_TICKS(x) ((x)*OS_CFG_TMR_TASK_RATE_HZ)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {ERROR_MSG, DATAOK_MSG, DATAFAIL_MSG, KEEPALIVEOK_MSG} MSG_TYPE;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void IOTThread();
static void keepAliveTask();

static MSG_TYPE readMsg();

static void toLittleEndian(uint16_t num, uint8_t little[2]);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const char headerStr[] = HEADER_STR;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/* IOT Task */
#define TASK_STK_SIZE			512U
#define TASK_STK_SIZE_LIMIT	(TASK_STK_SIZE / 10u)
#define TASK_PRIO				15u
static OS_TCB IOTTask;
static CPU_STK TaskStk[TASK_STK_SIZE];

/* keep alive Task */
#define TASKKA_STK_SIZE			100U
#define TASKKA_STK_SIZE_LIMIT	(TASK_STK_SIZE / 10u)
#define TASKKA_PRIO				25u

static OS_TCB keepAliveT;
static CPU_STK kATaskStk[TASKKA_STK_SIZE];

static OS_SEM floorSem;
static OS_MUTEX uartMutex;

static OS_ERR	os_err;


static uint16_t floors[FLOOR_COUNT];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief Initialize IOT functionality
*/
void IOTInit () {

	uart_cfg_t uartConfig = {.MSBF = false, .baudrate = UART_BAUDRATE, .parity = NO_PARITY};

	gpioMode(KEEPALIVE_LED, OUTPUT);
	gpioWrite(KEEPALIVE_LED, HIGH);

	OSSemCreate(&floorSem, "Floor Sem", 0U, &os_err);
	OSMutexCreate(&uartMutex, "UART Mutex", &os_err);

	uartOSInit(UART_ID, uartConfig);

	// keep Alive task
    OSTaskCreate(&keepAliveT, 		//tcb
                 "keepAlive Task",		//name
				 keepAliveTask,			    //func
                  0u,					//arg
                  TASKKA_PRIO,			//prio
                 &kATaskStk[0u],			//stack
                  TASKKA_STK_SIZE_LIMIT,	//stack limit
                  TASKKA_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    // IOT Task
    OSTaskCreate(&IOTTask, 		//tcb
                 "IOT Task",		//name
				 IOTThread,			    //func
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

}

/**
 * @brief Update floor state
 * @param f_i: Number of people in the i-th floor
*/
void IOTUpdate(uint16_t f1, uint16_t f2, uint16_t f3) {
	floors[0] = f1;
	floors[1] = f2;
	floors[2] = f3;
	OSSemPost(&floorSem, OS_OPT_POST_1, &os_err);	// Send signal
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void IOTThread() {

	char msg[2 + 2*FLOOR_COUNT] = {0x07, 0x01};

	while(1) {

		// Wait signal
		OSSemPend(&floorSem, 0U, OS_OPT_PEND_BLOCKING, NULL, &os_err);

		// LOCK UART
		OSMutexPend(&uartMutex, 0U, OS_OPT_PEND_BLOCKING, NULL, &os_err);

		char *data = msg+2;
		do {
			// Send data msg
			uartOSWriteMsg(UART_ID, headerStr, sizeof(headerStr));	// Send header

			for (int i = 0; i < FLOOR_COUNT; i++) {
				toLittleEndian(floors[i], (uint8_t*)data);
				data += 2;
			}
			uartOSWriteMsg(UART_ID, msg, sizeof(msg));	// send data

		} while (readMsg() != DATAOK_MSG && false);		// Repeat until OK

		// Free UART
		OSMutexPost(&uartMutex, OS_OPT_POST_NONE, &os_err);

		// Space between requests
		OSTimeDlyHMSM(0, 0, IOTINTERVAL_TIME, 0, OS_OPT_TIME_DLY, &os_err);
	}
}



static void keepAliveTask() {

	while (1) {

		// LOCK UART
		OSMutexPend(&uartMutex, 0U, OS_OPT_PEND_BLOCKING, NULL, &os_err);

		// Send keep alive msg
		uartOSWriteMsg(UART_ID, headerStr, sizeof(headerStr));
		const char msg[] = {0x01, KEEPALIVE_CODE};
		uartOSWriteMsg(UART_ID, msg, sizeof(msg));

		// Wait response
		MSG_TYPE res = readMsg();

		// Free UART
		OSMutexPost(&uartMutex, OS_OPT_POST_NONE, &os_err);

		// Check response
		if (res == KEEPALIVEOK_MSG) {
			// LEDMuxSet(KEEPALIVE_LED);
			gpioWrite(KEEPALIVE_LED, LOW);
		}
		else {
			// LEDMuxOff();
			gpioWrite(KEEPALIVE_LED, HIGH);
		}

		// Wait for next keepalive
		OSTimeDlyHMSM(0, 0, KEEPALIVE_TIME, 0, OS_OPT_TIME_DLY, &os_err);
	}
}


static MSG_TYPE readMsg() {

	// Wait response
	do {
		if (!uartOSWaitMsg(UART_ID, UART_TIMEOUT)) {		// Wait and check error
		 	return ERROR_MSG;
		}
	}
	while (uartGetRxMsgLength(UART_ID) < ANSWER_LENGTH);

	// Check message
	char msg[ANSWER_LENGTH];
	if (uartReadMsg(UART_ID, msg, ANSWER_LENGTH) == ANSWER_LENGTH) {		// Read bytes

		// Check header
		for (uint8_t i = 0; i < sizeof(headerStr); i++) {
			if (msg[i] != headerStr[i]) {
					return ERROR_MSG;		// Discard message
			}
		}

		// Check length
		if (msg[sizeof(headerStr)] != 0x01) {
			return ERROR_MSG;		// Discard message
		}

		switch(msg[sizeof(headerStr)+1]) {
			case DATAOK_CODE:
				return DATAOK_MSG;
			case DATAFAIL_CODE:
				return DATAFAIL_MSG;
			case KEEPALOK_CODE:
				return KEEPALIVEOK_MSG;
			default:
				return ERROR_MSG;
		}

	}

	return ERROR_MSG;

}


static void toLittleEndian(uint16_t num, uint8_t little[2]) {
	little[0] = (uint8_t)num;
	little[1] = (uint8_t)(num >> 8);
}
