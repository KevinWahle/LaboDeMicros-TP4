/***************************************************************************//**
  @file     encoder_hal.c
  @brief    Funciones para manejo del encoder
  @author   Grupo 5
 ******************************************************************************/

#include "encoder_hal.h"
#include <stdbool.h>
#include <stdint.h>
#include "MCAL/gpio.h"

#include <os.h>
#include "os_cfg_app.h"
// #include "timer/timer.h"

#define LONG_PRESS_TIME 700
#define DOUBLE_CLICK_TIME 400

typedef enum {IDLE_ENCODER, LEFT_TRANS, RIGHT_TRANS} SM_ENCODER;

typedef enum {IDLE_BUTTON ,C1, C2, C3} SM_BUTTON; //States of the machine

static pin_t rcha = PORTNUM2PIN(PD, 0);
static pin_t rchb = PORTNUM2PIN(PD, 2);
static pin_t button = PORTNUM2PIN(PD, 3);


// // Timers
// static tim_id_t longPressedTimerID;
// static tim_id_t doublePressedTimerID;



/* Encoder Task */
#define TASK_STK_SIZE			256u
#define TASK_STK_SIZE_LIMIT	(TASK_STK_SIZE / 10u)
#define TASK_PRIO              3u
static OS_TCB EncoderTask;
static CPU_STK TaskStk[TASK_STK_SIZE];

OS_Q encoderQ;
#define S_2_TICKS(x) ((x)*OS_CFG_TMR_TASK_RATE_HZ) 

static OS_ERR os_err;
static OS_TMR LongTimer;
static OS_TMR doubleTimer;

/**
 * @brief FSM implementation
 */
static void smEnconder(void *p_arg);

void  TMRCallBack (OS_TMR *p_tmr, void *p_arg){
	
}

bool encoderInit(){
	// timerInit();
	// longPressedTimerID = timerGetId();
	// doublePressedTimerID = timerGetId();
	gpioMode(rcha, INPUT);
	gpioMode(rchb, INPUT);
	gpioMode(button, INPUT);

	// funCb = funCbParam;

	

	// tim_id_t periodicTimerID = timerGetId();
	// timerStart(periodicTimerID, TIMER_MS2TICKS(0.5), TIM_MODE_PERIODIC, smEnconder);  // Periodic ISR 500us

	/* Fabrico los timers */
	OSTmrCreate(	(OS_TMR *)&LongTimer,
					(CPU_CHAR *)"Long",
					(OS_TICK )S_2_TICKS(LONG_PRESS_TIME/1000.0),
					(OS_TICK )0,
					(OS_OPT )OS_OPT_TMR_ONE_SHOT,
					(OS_TMR_CALLBACK_PTR)TMRCallBack,
					(void *)0,
					(OS_ERR *)&os_err);

	/* Fabrico los timers */
	OSTmrCreate(	(OS_TMR *)&doubleTimer,
					(CPU_CHAR *)"Double",
					(OS_TICK )S_2_TICKS(DOUBLE_CLICK_TIME/1000.0),
					(OS_TICK )0,
					(OS_OPT )OS_OPT_TMR_ONE_SHOT,
					(OS_TMR_CALLBACK_PTR)TMRCallBack,
					(void *)0,
					(OS_ERR *)&os_err);



	/* Check ’err” */
//	OSTmrStart ((OS_TMR *)&MyTmr1, (OS_ERR *)&os_err);


    /* Create message queue */
    OSQCreate(&encoderQ, "Encoder Q", OS_CFG_MSG_POOL_SIZE, &os_err);

    /* Create Task */
    OSTaskCreate(&EncoderTask, 			//tcb
                 "Encoder Task",		//name
                  smEnconder,			//func
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

static void smEnconder(void *p_arg){

	static SM_ENCODER state_rotate = IDLE_ENCODER;
	static SM_BUTTON state_button = IDLE_BUTTON;

	while (1) {

		switch(state_rotate){
		case IDLE_ENCODER:
			if( ( !gpioRead(rcha) && gpioRead(rchb) ) || ( gpioRead(rcha) && !gpioRead(rchb) )){
				if(!gpioRead(rcha))
					state_rotate = LEFT_TRANS;
				else
					state_rotate = RIGHT_TRANS;
			}
			break;
		case LEFT_TRANS:
			if(gpioRead(rcha) && gpioRead(rchb)){
				state_rotate = IDLE_ENCODER;
				// funCb(ENC_LEFT);
				OSQPost(&encoderQ, (void*)((uint32_t)ENC_LEFT), sizeof(ENC_LEFT), OS_OPT_POST_ALL, &os_err);
			}
			break;
		case RIGHT_TRANS:
			if(gpioRead(rcha) && gpioRead(rchb)){
				state_rotate = IDLE_ENCODER;
				//funCb(ENC_RIGHT);
				OSQPost(&encoderQ, (void*)((uint32_t)ENC_RIGHT), sizeof(ENC_RIGHT), OS_OPT_POST_ALL, &os_err);
			}
			break;
		}

		switch(state_button){
		case IDLE_BUTTON:
			if(!gpioRead(button)){
				state_button = C1;
				//timerStart(longPressedTimerID, TIMER_MS2TICKS(LONG_PRESS_TIME), TIM_MODE_SINGLESHOT, NULL);
				//timerStart(doublePressedTimerID, TIMER_MS2TICKS(DOUBLE_CLICK_TIME), TIM_MODE_SINGLESHOT, NULL);
				OSTmrStart ((OS_TMR *)&LongTimer, (OS_ERR *)&os_err);
				OSTmrStart ((OS_TMR *)&doubleTimer, (OS_ERR *)&os_err);
			}
			break;
		case C1:
			if(gpioRead(button)){
				if(OSTmrStateGet((OS_TMR *)&LongTimer, (OS_ERR *)&os_err) == OS_TMR_STATE_COMPLETED){
					state_button = IDLE_BUTTON;
					//funCb(ENC_LONG);
					OSQPost(&encoderQ, (void*)((uint32_t)ENC_LONG), 0, OS_OPT_POST_ALL, &os_err);

				}
				else if(OSTmrStateGet((OS_TMR *)&doubleTimer, (OS_ERR *)&os_err) == OS_TMR_STATE_COMPLETED){
					state_button = IDLE_BUTTON;
					//funCb(ENC_CLICK);
					OSQPost(&encoderQ, (void*)((uint32_t)ENC_CLICK), 0, OS_OPT_POST_ALL, &os_err);

				}
				else{
					state_button = C2;
					// funCb(ENC_CLICK);
					OSQPost(&encoderQ, (void*)((uint32_t)ENC_CLICK), 0, OS_OPT_POST_ALL, &os_err);
				}
			}
			break;
		case C2:
			if(OSTmrStateGet((OS_TMR *)&doubleTimer, (OS_ERR *)&os_err) == OS_TMR_STATE_COMPLETED)
				state_button = IDLE_BUTTON;
			if(!gpioRead(button)){
				state_button = C3;
				OSTmrStart ((OS_TMR *)&LongTimer, (OS_ERR *)&os_err);
			}
			break;
		case C3:
			if(gpioRead(button)){
				if(OSTmrStateGet((OS_TMR *)&LongTimer, (OS_ERR *)&os_err) == OS_TMR_STATE_COMPLETED){
					state_button = IDLE_BUTTON;
					// funCb(ENC_LONG);
					OSQPost(&encoderQ, (void*)((uint32_t)ENC_LONG), 0, OS_OPT_POST_ALL, &os_err);
				}
				else if(!(OSTmrStateGet((OS_TMR *)&doubleTimer, (OS_ERR *)&os_err) == OS_TMR_STATE_COMPLETED)){
					state_button = IDLE_BUTTON;
					// funCb(ENC_DOUBLE);
					OSQPost(&encoderQ, (void*)((uint32_t)ENC_DOUBLE), 0, OS_OPT_POST_ALL, &os_err);
				}
				else{
					state_button = IDLE_BUTTON;
					//funCb(ENC_CLICK);
					OSQPost(&encoderQ, (void*)((uint32_t)ENC_CLICK), 0, OS_OPT_POST_ALL, &os_err);

				}
			}
			break;
		}

		OSTimeDlyHMSM(0U, 0U, 0U, 1U, OS_OPT_TIME_DLY, &os_err);	// Wait

	}
}

