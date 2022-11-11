/***************************************************************************//**
  @file		LEDMuxTest.c
  @brieef	Testbench para las funciones del manejo de leds de la placa
  @author	Grupo 5
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "LEDMux.h"
#include "encoder_hal.h"

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void nextMode(ENC_STATE state);


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	LEDMuxInit();
	encoderInit(nextMode);
	LEDMuxOff();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{



}

void nextMode(ENC_STATE state) {

	static uint8_t mode = 0;

	if (state == ENC_RIGHT || state == ENC_CLICK) {
		mode++;
	}
	else if (state == ENC_LEFT) {
		mode--;
	}

	if (state == ENC_DOUBLE) {
		LEDMuxOff();
	}
	else if (state == ENC_LONG) {
		LEDMuxSetForTime(mode, 5000);
	}
	else {
		LEDMuxSet(mode);
	}
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



 
