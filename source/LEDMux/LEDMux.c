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
#include "timer/timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define LED_SEL_PIN_COUNT	2

#define SEL1_PIN	PORTNUM2PIN(PA, 2)
#define SEL2_PIN	PORTNUM2PIN(PB, 23)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum LED_SEL_STATES	{LED_OFF, LED1_ON, LED2_ON, LED3_ON};


/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

tim_id_t timerID;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void muxWrite(uint8_t num);

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

	timerID = timerGetId();		// ID Request for timed functions

	return true;
}


/**
 * @brief Turn on one of the LEDs
 * @param ledN: The number of LED to turn on [1 - N]
*/
void LEDMuxSet(uint8_t ledN) {
	muxWrite(ledN);
	timerStop(timerID);		// Stop timer if was active
}


/**
 * @brief Turn on one of the LEDs for the time specified
 * @param ledN: The number of LED to turn on [1 - N]
 * @param time: time to show in ms
*/
void LEDMuxSetForTime(uint8_t ledN, uint32_t time) {
	LEDMuxSet(ledN);
	timerStart(timerID, TIMER_MS2TICKS(time), TIM_MODE_SINGLESHOT, LEDMuxOff);
}


/**
 * @brief Turn off the LEDs
*/
void LEDMuxOff() {
	muxWrite(LED_OFF);
	timerStop(timerID);		// Stop timer if was active
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void muxWrite(uint8_t num) {
	for (int i = 0; i < LED_SEL_PIN_COUNT; i++) {
		gpioWrite(selPins[i], num & 0x1);		// Write last bit
		num >>= 1;								// Next bit
	}
}
 
