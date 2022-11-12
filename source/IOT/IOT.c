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

#include <os.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_ID			0
#define UART_BAUDRATE	1200

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void IOTThread();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/* IOT Task */
#define TASK_STK_SIZE			256u
#define TASK_STK_SIZE_LIMIT	(TASK_STK_SIZE / 10u)
#define TASK_PRIO				3u
static OS_TCB IOTTask;
static CPU_STK TaskStk[TASK_STK_SIZE];

static OS_ERR	os_err;

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

	uartOSInit(UART_ID, uartConfig);

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

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void IOTThread() {

	char c;

	while(1) {

		uartOSWaitMsg(UART_ID);
		uartReadMsg(UART_ID, &c, 1);
		uartOSWriteMsg(UART_ID, &c, 1);

	}


}
