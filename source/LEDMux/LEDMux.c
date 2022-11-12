/***************************************************************************//**
  @file		LEDMux.c
  @brief    Funciones para manejo de los LEDs mediante MUX
  @author	Grupo 5
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "LEDMux.h"
#include "MCAL/gpio.h"
//#include "timer/timer.h"

#include <os.h>
#include "os_cfg_app.h"
#include <stddef.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define LED_SEL_PIN_COUNT	2

#define SEL1_PIN	PORTNUM2PIN(PA, 2)
#define SEL2_PIN	PORTNUM2PIN(PB, 23)

#define S_2_TICKS(x) ((x)*OS_CFG_TMR_TASK_RATE_HZ)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum LED_SEL_STATES	{LED_OFF, LED1_ON, LED2_ON, LED3_ON};


/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

//static tim_id_t timerID;
static OS_TMR timerOS;

static OS_ERR os_err;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void muxWrite(uint8_t num);

static void  TMRCallBack (OS_TMR *p_tmr, void *p_arg);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const pin_t selPins[] = {SEL1_PIN, SEL2_PIN};

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief Init Hw for LEDs
 * @return init succes
*/
int LEDMuxInit() {
	gpioMode(SEL1_PIN, OUTPUT);
	gpioMode(SEL2_PIN, OUTPUT);

	LEDMuxOff();

//	timerID = timerGetId();		// ID Request for timed functions

	return true;
}


/**
 * @brief Turn on one of the LEDs
 * @param ledN: The number of LED to turn on [1 - N]
*/
void LEDMuxSet(uint8_t ledN) {
	muxWrite(ledN);
//	timerStop(timerID);		// Stop timer if was active
	OSTmrStop(&timerOS, OS_OPT_TMR_NONE, NULL, &os_err);
}


/**
 * @brief Turn on one of the LEDs for the time specified
 * @param ledN: The number of LED to turn on [1 - N]
 * @param time: time to show in ms
*/
void LEDMuxSetForTime(uint8_t ledN, uint32_t time) {
	LEDMuxSet(ledN);
//	timerStart(timerID, TIMER_MS2TICKS(time), TIM_MODE_SINGLESHOT, LEDMuxOff);
	OSTmrCreate(	(OS_TMR *)&timerOS,
						(CPU_CHAR *)"LEDMux Timer",
						(OS_TICK )S_2_TICKS(time/1000.0),
						(OS_TICK )0,
						(OS_OPT )OS_OPT_TMR_ONE_SHOT,
						(OS_TMR_CALLBACK_PTR)TMRCallBack,
						(void *)0,
						(OS_ERR *)&os_err);
	OSTmrStart(&timerOS, &os_err);
}


/**
 * @brief Turn off the LEDs
*/
void LEDMuxOff() {
	muxWrite(LED_OFF);
//	timerStop(timerID);		// Stop timer if was active
	OSTmrStop(&timerOS, OS_OPT_TMR_NONE, NULL, &os_err);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void muxWrite(uint8_t num) {
	for (int i = 0; i < LED_SEL_PIN_COUNT; i++) {
		gpioWrite(selPins[i], num & 0x1);		// Write last bit
		num >>= 1;								// Next bit
	}
}


static void  TMRCallBack (OS_TMR *p_tmr, void *p_arg) {
	LEDMuxOff();
}
