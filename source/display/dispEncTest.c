/***************************************************************************//**
  @file		dispEncTest.c
  @brieef	Testbench para las funciones de dispArr con Encoder
  @author	Grupo 5
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "dispArr.h"
#include "encoder/encoder_hal.h"

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
	dispArrInit();
	encoderInit(nextMode);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{



}

void nextMode(ENC_STATE state) {

	static uint8_t mode = 0;
	static cosnt uint8_t numArr[] = {1, 10, 8, 9};

	if (state == ENC_RIGHT || state == ENC_CLICK) {
		mode++;
	}
	else if (state == ENC_LEFT || state == ENC_LONG) {
		mode--;
	}
	else if (state == ENC_DOUBLE) {
		mode = 0;
	}

	switch (mode) {

	case 0:
		dispArrShow("Hola");
		break;

	case 1:
		dispArrShow("Mensaje largo");
		break;

	case 2:
		dispArrShowForTime("Hola", 5000);
		break;

	case 3:
		dispArrShowForTime("Mensaje largo", 3500);
		break;

	case 4:
		dispArrShowNum(12);
		break;

	case 5:
		dispArrShowNum(1234);
		break;

	case 6:
		dispArrShowNum(56789012);
		break;

	case 7:
		dispArrShowNum(0);
		break;

	case 8:
		dispArrSlideOnce("Un texto bastante largo");
		break;

	case 9:
		dispArrSlideOnce("Hola");
		break;

	case 10:
		dispArrSlideLoop("Un texto bastante largo");
		break;

	case 11:
		dispArrSlideLoop("Hola");
		break;

	case 12:
	case 13:
	case 14:
	case 15:
		dispArrShow("Hola");
		dispArrSelect(mode-12);
		break;

	case 16:
		dispArrShow("Hola");
		dispArrSelect(4);
		break;

	case 17:
	case 18:
	case 19:
	case 20:
		dispArrShow("Mensaje largo");
		dispArrSelect(mode-17);
		break;

	case 21:
		dispArrShow("Mensaje largo");
		dispArrSelect(5);
		break;

	case 22:
	case 23:
	case 24:
	case 25:
		dispArrShow("Hola");
		dispArrBlinkSel(mode-22);
		break;

	case 26:
		dispArrShow("Hola");
		dispArrBlinkSel(4);
		break;

	case 27:
	case 28:
	case 29:
	case 30:
		dispArrShow("Mensaje largo");
		dispArrBlinkSel(mode-27);
		break;

	case 31:
		dispArrShow("Mensaje largo");
		dispArrBlinkSel(10);
		break;

	case 32:
		dispArrShowNumArr(numArr);
		break;

	case 33:
		dispArrClear();
		break;

	}

	if (mode > 33) {
		dispArrSetBright(mode-34);
		dispArrShowNum(dispArrGetBright());
	}

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



 
