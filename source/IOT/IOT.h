/***************************************************************************//**
  @file     IOT.h
  @brief	Funciones para comunicación con la nube
  @author	Grupo 5
  @date		12 nov. 2022
 ******************************************************************************/

#ifndef _IOT_H_
#define _IOT_H_

#include <stdint.h>

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize IOT functionality
*/
void IOTInit();

/**
 * @brief Update floor state
 * @param f_i: Number of people in the i-th floor
*/
void IOTUpdate(uint16_t f1, uint16_t f2, uint16_t f3);


/*******************************************************************************
 ******************************************************************************/

#endif // _IOT_H_
