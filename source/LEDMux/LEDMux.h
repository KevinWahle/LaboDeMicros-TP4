/***************************************************************************//**
  @file     LEDMux.h
  @brief    Funciones para manejo de los LEDs mediante MUX
  @author   Grupo 5
 ******************************************************************************/

#ifndef _LEDMUX_LEDMUX_H_
#define _LEDMUX_LEDMUX_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Init Hw for LEDs
 * @return init succes
*/
int LEDMuxInit();

/**
 * @brief Turn on one of the LEDs
 * @param ledN: The number of LED to turn on [1 - N]
*/
void LEDMuxSet(uint8_t ledN);

/**
 * @brief Turn on one of the LEDs for the time specified
 * @param ledN: The number of LED to turn on [1 - N]
 * @param time: time to show in ms
*/
void LEDMuxSetForTime(uint8_t ledN, uint32_t time);

/**
 * @brief Turn off the LEDs
*/
void LEDMuxOff();

/*******************************************************************************
 ******************************************************************************/

#endif // _LEDMUX_LEDMUX_H_
