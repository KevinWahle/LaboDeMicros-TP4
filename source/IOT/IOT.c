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

#include <stdio.h>
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_ID			0
#define UART_BAUDRATE	1200

#define HEADER_STR	{0xAA, 0x55, 0xC3, 0x3C}

#define SENDDATA_CODE	0x01
#define DATAOK_CODE		0x81
#define DATAFAIL_CODE	0xC1
#define KEEPALIVE_CODE	0x02
#define KEEPALOK_CODE	0x82

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void IOTThread();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const char headerStr[] = HEADER_STR;

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

	char msg[] = {0xAA, 0x55, 0xC3, 0x3C, KEEPALIVE_CODE};

	while(1) {

		uartOSWriteMsg(UART_ID, msg, sizeof(msg));

		do {
			uartOSWaitMsg(UART_ID);
		}
		while (uartGetRxMsgLength(UART_ID) < 6);

		char rx[10];
		uartReadMsg(UART_ID, rx, 6);
		printf("%X %X %X %X %X %X\n", rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]);

		OSTimeDlyHMSM(0, 0, 5, 0, OS_OPT_TIME_DLY, &os_err);

	}


}
