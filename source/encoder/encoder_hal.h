/***************************************************************************//**
  @file     encoder_hal.h
  @brief    Funciones para manejo del encoder
  @author   Grupo 5
 ******************************************************************************/

#ifndef _ENCODER_HAL_H_
#define _ENCODER_HAL_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <os.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Different events supported by the encoder module
typedef enum {ENC_LEFT, ENC_RIGHT, ENC_CLICK, ENC_LONG, ENC_DOUBLE} ENC_STATE;

// typedef void (*encoderCb) (ENC_STATE state);

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

extern OS_Q encoderQ;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes encoder module
 * @param callback for new event notification
 */
bool encoderInit();


/*******************************************************************************
 ******************************************************************************/

#endif // _ENCODER_HAL_H_
