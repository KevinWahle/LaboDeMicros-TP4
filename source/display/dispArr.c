/***************************************************************************//**
  @file     dispArr.c
  @brief    Functions for 7 segment display array
  @author   Grupo 5
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "dispArr.h"
#include "dispDigits.h"
// #include "timer/timer.h"
#include "MCAL/gpio.h"
#include <stdlib.h>
#include <string.h>

#include <os.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


////// Pin Assignment

	// Segments

#define A_SEGMENT	PORTNUM2PIN(PC, 5)
#define B_SEGMENT	PORTNUM2PIN(PC, 7)
#define C_SEGMENT	PORTNUM2PIN(PC, 0)
#define D_SEGMENT	PORTNUM2PIN(PC, 9)
#define E_SEGMENT	PORTNUM2PIN(PC, 8)
#define F_SEGMENT	PORTNUM2PIN(PC, 1)
#define G_SEGMENT	PORTNUM2PIN(PB, 19)
#define DP_SEGMENT	PORTNUM2PIN(PB, 18)

#define SEGMENT_ARR	{A_SEGMENT, B_SEGMENT, C_SEGMENT, D_SEGMENT, E_SEGMENT, F_SEGMENT, G_SEGMENT, DP_SEGMENT}


	// Selection (MUX) pins

#define SEL_0			PORTNUM2PIN(PC, 3)		// LSB
#define SEL_1			PORTNUM2PIN(PC, 2)		// MSB

#define SEL_ARR			{SEL_0, SEL_1}

#define SEL_PIN_COUNT	2



////// Timers

#define DISP_TIME	3	// Time of each display (milliseconds)

#define TRANS_TIME	250	// Time of digit transition (milliseconds)

#define BLINK_TIME	500	// 1/2 period of blink (milliseconds)


#define PISR_TIME	1	// Time of the PISR (milliseconds)


#define DISP_CYC	(DISP_TIME/PISR_TIME)

#define TRANS_CYC	(TRANS_TIME/PISR_TIME)

#define BLINK_CYC	(BLINK_TIME/PISR_TIME)


////// Brightness

#define MAX_BRIGHT	100


////// Useful macros

#define LIMIT(val, min, max)	( ( (val) >= (min) ) && ( (val) <= (max) ) ? (val) : ( ( (val) < (min) ) ? (min) : (max) ) )

#define MS2CYC(ms)	((ms)/PISR_TIME)	// Converts milliseconds to PISR cycles

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef pin_t dispSegmentsPins_t[SEGMENTS_COUNT];

typedef enum {SHOW, TIMED, LOOP, ONCE, SELECT, BLINK} DISPLAY_MODES;

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

// Pin assignment
static dispSegmentsPins_t segments = SEGMENT_ARR;

static pin_t control[SEL_PIN_COUNT] = SEL_ARR;


// Display control

static dispDigit_t actualDigits[DISP_COUNT];	// Array to show on displays
static uint8_t actualDisp = 0;		// Index of array to update

static dispDigit_t selectedChar;		// Save char to blink
static uint8_t selectedIndex;

static dispDigit_t slideDigits[MAX_SLIDE_LEN];	// Array to copy digits to slide
static uint8_t slideLen;
static uint8_t slideIndex;


static DISPLAY_MODES actualMode = SHOW;


static uint8_t actualBright = MAX_BRIGHT;


// Timers

static uint32_t dispCont = DISP_CYC-1;		// Counter for display update
static uint32_t transCont = TRANS_CYC-1;	// Counter for slide
static uint32_t timedShowCont;				// Counter for TIMED mode
static uint32_t blinkCont = BLINK_CYC-1;	// Timer for BLINK mode
static uint32_t dutyCont = DISP_CYC-1;		// Counter of display show time for brightness control


// Flags

static bool blinkOff = false;
static bool dutyOff = false;


/* Display Task */
#define TASK_STK_SIZE			256u
#define TASK_STK_SIZE_LIMIT	(TASK_STK_SIZE / 10u)
#define TASK_PRIO              3u
static OS_TCB displayTask;
static CPU_STK TaskStk[TASK_STK_SIZE];

static OS_ERR os_err;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void dispPISR(void *p_arg);

/**
 * @brief Set the control output to select the desired display. Control is assumed to be a MUX.
 * @param *control: pin array with the pins used for control (according PORTNUM2PIN and ordered from LSB to MSB)
 * @param nctrl: number of pins dedicated to control
 * @param sel: display to be selected
 */
static void selectDisp(uint8_t sel);

/**
 * @brief Display the desired value in the display.
 * @param segments[8]: pin array with the pins of the segments (according PORTNUM2PIN).
 * @param digit: states of the pins of each segment in the display (according DispDigits)
 */
static void dispShow(const dispSegmentsPins_t segments, dispDigit_t digit);

static void rollDisplay();

static void loopDigits();

static void string2DispArr(const char* str);

// Copy the string to slideDigits after allocation
static void retrieveSlideString(const char* str);


static void setMode(DISPLAY_MODES mode);

static void clearDispArr();

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief Configures the 7 Segment Display Array Driver
 * @return Initialization succeed
 */
bool dispArrInit() {

	// Set segments as outputs
	for (int i = 0; i < SEGMENTS_COUNT; i++) {
		gpioMode(segments[i], OUTPUT);
	}

	// Set control pins as outputs
	for (int i = 0; i < SEL_PIN_COUNT; i++) {
		gpioMode(control[i], OUTPUT);
	}

	dispArrClear();		// Set to clear mode

	// Init timer for PISR
	// timerInit();
	// timerStart(timerGetId(), TIMER_MS2TICKS(PISR_TIME), TIM_MODE_PERIODIC, dispPISR);
    /* Create Task */

    OSTaskCreate(&displayTask, 			//tcb
                 "Encoder Task",		//name
                  dispPISR,			//func
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

/**
 * @brief Get the actual brightness of the display
 * @return bright: The level of brightness [0-100]
 * @return bright: The level of brightness [0-100]
 */
uint8_t dispArrGetBright() {
	return actualBright;
}


/**
 * @brief Set the brightness of the display array
 * @param bright: Level of brightness [0-100]
 */
void dispArrSetBright(int8_t bright) {
	actualBright = LIMIT(bright, 0, MAX_BRIGHT);

	if (actualBright == 0) {
		for (int i = 0 ; i < DISP_COUNT; i++) {
			dispShow(segments, DISP_OFF);
		}
	}

}


/**
 * @brief Show the first DISP_COUNT characters of the string given in the display
 * @param str: string to show
 */
void dispArrShow(const char* str) {

	setMode(SHOW);

	string2DispArr(str);

}


/**
 * @brief Show the first DISP_COUNT characters of the string given in the display for the time given
 * @param str: string to show
 * @param time: the time in milliseconds
 */
void dispArrShowForTime(const char* str, uint32_t time) {

	setMode(TIMED);

	string2DispArr(str);

	timedShowCont = MS2CYC(time)-1;

}


/**
 * @brief Show the first DISP_COUNT digits of the number given in the display
 * @param num: the num to show
 */
void dispArrShowNum(uint32_t num) {

	setMode(SHOW);

	uint32_t div = 10;
	uint8_t len = 1;

	while(num/div) {
		len++;
		div *= 10;
	}

	if (len > DISP_COUNT) {
		for (int i=0; i < DISP_COUNT; i++) {
			div /= 10;
		}
		num2Digit(num/div, actualDigits);		// Keep first 4 digits, discard the rest
	}
	else {
		clearDispArr();									// Clear Array
		num2Digit(num, actualDigits+DISP_COUNT-len);	// and write to end of array
	}
}


/**
 * @brief Show the first DISP_COUNT digits of the number array given in the display
 * @param numArr: the number array to show (numbers from 0 to 9)
 */
void dispArrShowNumArr(const uint8_t numArr[DISP_COUNT]) {

	setMode(SHOW);

	const dispDigit_t digitsNum[] = DISP_DIGITS_NUM;

	for (int i = 0; i < DISP_COUNT; i++) {
		uint8_t num = numArr[i];
		if (num <= 9) {
			actualDigits[i] = digitsNum[num];
		}
		else {
			actualDigits[i] = DISP_OFF;
		}
	}

}


/**
 * @brief Show the string given in the display sliding once
 * @param str: string to show
 */
void dispArrSlideOnce(const char* str) {

	setMode(ONCE);

	retrieveSlideString(str);

	clearDispArr();

	transCont = TRANS_CYC-1;	// Timer reset

	slideIndex = 0;		// Index reset

}


/**
 * @brief Show the string given in the display sliding in a loop
 * @param str: string to show
 */
void dispArrSlideLoop(const char* str) {

	setMode(LOOP);

	retrieveSlideString(str);

	clearDispArr();

	transCont = TRANS_CYC-1;	// Timer reset

	slideIndex = 0;		// Index reset
}


/**
 * @brief Choose one digit to be selected by adding DP
 * @param sel: index of the digit to select [0 - (DISP_COUNT-1)]
 */
void dispArrSelect(uint8_t sel) {

	setMode(SELECT);

	actualDigits[LIMIT(sel, 0, DISP_COUNT-1)] |= DISP_DP;		// Turn on DP on selected digit

}



/**
 * @brief Select one digit to blink periodically
 * @param sel: index of the digit to blink [0 - (DISP_COUNT-1)]
 */
void dispArrBlinkSel(uint8_t sel) {

	setMode(BLINK);

	selectedIndex = LIMIT(sel, 0, DISP_COUNT-1);

	selectedChar = actualDigits[selectedIndex];

	// Reset of timers and flags
	blinkCont = BLINK_CYC-1;
	blinkOff = true;

}


void dispArrClear() {

	setMode(SHOW);

	clearDispArr();
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void dispPISR(void *p_arg) {

	while (1) {

		switch (actualMode) {
			case SHOW:
				// Do nothing
				break;

			case TIMED:

				if (!timedShowCont) {	// Time elapsed
					dispArrClear();			// set mode to clear
				}
				else {
					timedShowCont--;
				}

				break;

			case LOOP:
			case ONCE:

				if (!transCont) {
					transCont = TRANS_CYC-1;
					loopDigits();
				}
				else {
					transCont--;
				}

				break;

			case SELECT:
				// Do nothing
				break;

			case BLINK:

				if (!blinkCont) {

					actualDigits[selectedIndex] = blinkOff ? DISP_OFF : selectedChar;	// Change selected digit

					blinkOff = !blinkOff;

					blinkCont = BLINK_CYC-1;
				}
				else {
					blinkCont--;
				}
				break;

			default:
				break;
		}

		// Display rolling and brightness control.
		if (actualBright) {
			if (!dispCont) {
				rollDisplay();	// actualDisp increment
				dispCont = DISP_CYC-1;

				dutyCont = ((double)actualBright/MAX_BRIGHT)*DISP_CYC;	// Duty reset
				dutyOff = false;
			}
			else {
				dispCont--;
			}

			if (!dutyCont) {
				if (!dutyOff) {
					dispShow(segments, DISP_OFF);		// Turn off actual display
					dutyOff = true;		// Dont do it again until next display
				}
			}
			else {
				dutyCont--;
			}
		}
		
	    OSTimeDlyHMSM(0u, 0u, 0u, PISR_TIME, OS_OPT_TIME_DLY, &os_err);
	}
}

/**
 * @brief Set the control output to select the desired display. Control is assumed to be a MUX.
 * @param sel: display to be selected
 */
static void selectDisp(uint8_t sel) {

	for (int i = 0; i < SEL_PIN_COUNT; i++) {
		gpioWrite(control[i], sel & 0x1);		// Write last bit
		sel >>= 1;								// Next bit
	}

}


/**
 * @brief Display the desired value in the display.
 * @param segments[8]: pin array with the pins of the segments (according PORTNUM2PIN).
 * @param digit: states of the pins of each segment in the display (according DispDigits)
 */
static void dispShow(const dispSegmentsPins_t segments, dispDigit_t digit) {

	for (int i = (SEGMENTS_COUNT-1); i >= 0; i--) {		// Reverse order (MSB is a)
		gpioWrite(segments[i], digit & 0x1);	// Write bit
		digit >>= 1;							// Next bit
	}

}


static void rollDisplay() {
	selectDisp(actualDisp);							// Select next display
	dispShow(segments, actualDigits[actualDisp++]);		// and show next char
	actualDisp %= DISP_COUNT;							// increment counter and reset
}


static void loopDigits() {

	for (int i = 0; i < DISP_COUNT-1; i++) {
		actualDigits[i] = actualDigits[i+1];	// Roll
	}

	if (slideIndex < slideLen) actualDigits[DISP_COUNT-1] = slideDigits[slideIndex];	// Enter last digit and increment
	else actualDigits[DISP_COUNT-1] = DISP_OFF;

	slideIndex++;

	if (slideIndex >= slideLen+DISP_COUNT) {
		if (actualMode == ONCE) {
			actualMode = SHOW;
		}
		else if (actualMode == LOOP) {
			slideIndex = 0;
		}
		// TODO: else: throw error
	}

}

static void setMode(DISPLAY_MODES mode) {

	actualMode = mode;

}

// From string to actualDigits (clamper)
static void string2DispArr(const char* str) {

	for(uint8_t i = 0; i < DISP_COUNT; i++) {

		if (*str) {
			actualDigits[i] = char2Digit(*(str++));
		}
		else {		// str length < DISP_COUNT
			actualDigits[i] = DISP_OFF;
		}

	}

}

// Copy the string to slideDigits after allocation
static void retrieveSlideString(const char* str) {

	slideLen = strlen(str);

	if (slideLen > MAX_SLIDE_LEN) {
		slideLen = MAX_SLIDE_LEN;
		char temp[MAX_SLIDE_LEN];
		memcpy(temp, str, slideLen*sizeof(char*));
		string2Digit(temp, slideDigits);
	}
	else {
		string2Digit(str, slideDigits);
	}

}

static void clearDispArr() {
	for (int i = 0; i < DISP_COUNT; i++) {
		actualDigits[i] = DISP_OFF;
	}
}

/*******************************************************************************
 ******************************************************************************/
