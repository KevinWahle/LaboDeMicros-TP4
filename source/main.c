#include "hardware.h"
#include "MCAL/board.h"
#include  <os.h>

#include "FSM_routines.h"
#include "event_queue/event_queue.h"
#include "magtek/MagtekWrapper.h"
#include "display/dispArr.h"
#include "encoder/encoder_hal.h"
#include <stddef.h>
#include <stdio.h>

/* Task Start */
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			2u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

/* Encoder Task */
#define TASK2_STK_SIZE			100U
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              3u
static OS_TCB encoder2EventTask;
static CPU_STK Task2Stk[TASK2_STK_SIZE];

/* Card Task */
#define TASK3_STK_SIZE			100u
#define TASK3_STK_SIZE_LIMIT	(TASK3_STK_SIZE / 10u)
#define TASK3_PRIO              3u
static OS_TCB card2EventTask;
static CPU_STK Task3Stk[TASK3_STK_SIZE];


void App_Init (void);
void App_Run (void);


static void encoderTask(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    OS_MSG_SIZE size;

    while (1) {
        ENC_STATE state = (ENC_STATE)OSQPend(&encoderQ, 0U, OS_OPT_PEND_BLOCKING, &size, NULL, &os_err);
        encoderCallback(state);
    }
}

static void CardTask(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    OS_MSG_SIZE size;

    while (1) {
        char* msg = OSQPend(&magtekQ, 0U, OS_OPT_PEND_BLOCKING, &size, NULL, &os_err);
        IDcardCb(msg!=NULL, msg);
    }
}


static void TaskStart(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    /* Initialize the uC/CPU Services. */
    CPU_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    /* (optional) Compute CPU capacity with no task running */
    OSStatTaskCPUUsageInit(&os_err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    App_Init();

    /* Create Tasks */
    
    OSTaskCreate(&encoder2EventTask, 			//tcb
                 "Encoder2EventTask",				//name
                  encoderTask,			//func
                  0u,					//arg
                  TASK2_PRIO,			//prio
                 &Task2Stk[0u],			//stack
                  TASK2_STK_SIZE_LIMIT,	//stack limit
                  TASK2_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    OSTaskCreate(&card2EventTask, 		//tcb
                 "Card2EventTask",		//name
                  CardTask,			    //func
                  0u,					//arg
                  TASK3_PRIO,			//prio
                 &Task3Stk[0u],			//stack
                  TASK3_STK_SIZE_LIMIT,	//stack limit
                  TASK3_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    while (1) {
        App_Run();
    }
}

int main(void) {
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

    hw_Init();

    OSInit(&err);
 #if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	 /* Enable task round robin. */
	 OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err);
 #endif
    OS_CPU_SysTickInit(SystemCoreClock / (uint32_t)OSCfg_TickRate_Hz);

   OSTaskCreate(&TaskStartTCB,
                "App Task Start",
                 TaskStart,
                 0u,
                TASKSTART_PRIO,
                &TaskStartStk[0u],
                (TASKSTART_STK_SIZE / 10u),
                 TASKSTART_STK_SIZE,
                 0u,
                 0u,
                 0u,
                (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                &err);

    OSStart(&err);

	/* Should Never Get Here */
    while (1) {

    }
}
