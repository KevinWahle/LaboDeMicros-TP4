
/***************************************************************************//**
  @file     dispArr.h
  @brief    Functions for 7 segment display array
  @author   Grupo 5
 ******************************************************************************/

#ifndef _DISPARR_H_
#define _DISPARR_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DISP_COUNT		4

#define MAX_SLIDE_LEN	32

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Configures the 7 Segment Display Array Driver
 * @return Initialization succeed
 */
bool dispArrInit();


/**
 * @brief Get the actual brightness of the display
 * @return bright: The level of brightness [0-100]
 */
uint8_t dispArrGetBright();


/**
 * @brief Set the brightness of the display array
 * @param bright: Level of brightness [0-100]
 */
void dispArrSetBright(int8_t bright);


/**
 * @brief Show the first DISP_COUNT characters of the string given in the display
 * @param str: string to show
 */
void dispArrShow(const char* str);


/**
 * @brief Show the first DISP_COUNT characters of the string given in the display for the time given, then clear the display
 * @param str: string to show
 * @param time: the time in milliseconds
 */
void dispArrShowForTime(const char* str, uint32_t time);


/**
 * @brief Show the first DISP_COUNT digits of the number given in the display
 * @param num: the num to show
 */
void dispArrShowNum(uint32_t num);

/**
 * @brief Show the first DISP_COUNT digits of the number array given in the display
 * @param numArr: the number array to show (numbers from 0 to 9)
 */
void dispArrShowNumArr(const uint8_t numArr[DISP_COUNT]);


/**
 * @brief Show the string given in the display sliding once
 * @param str: string to show. max MAX_SLIDE_LEN digits
 */
void dispArrSlideOnce(const char* str);


/**
 * @brief Show the string given in the display sliding in a loop
 * @param str: string to show. max MAX_SLIDE_LEN digits
 */
void dispArrSlideLoop(const char* str);


/**
 * @brief Choose one digit to be selected after a call to show by adding DP
 * @param sel: index of the digit to select [0 - (DISP_COUNT-1)]
 */
void dispArrSelect(uint8_t sel);


/**
 * @brief Select one digit to blink periodically after a call to show
 * @param sel: index of the digit to blink [0 - (DISP_COUNT-1)]
 */
void dispArrBlinkSel(uint8_t sel);


/**
 * @brief Clear the entire display
 */
void dispArrClear();

/*******************************************************************************
 ******************************************************************************/

#endif /* _DISPARR_H_ */
