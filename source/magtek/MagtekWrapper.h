/***************************************************************************//**
  @file     MagtekWrapper.h
  @brief    Mangetic stripe wrapper for 8 digit Id
  @author   Group 5
 ******************************************************************************/

#ifndef MAGTEKWRAPPER_H_
#define MAGTEKWRAPPER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <os.h>
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

//typedef void (*WrapperCardCb) (bool state, const char* mydata);

extern OS_Q magtekQ;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief: Initialize pins and set intrruption for Magtek
 * @return initialization succeed
*/
bool Card2Init();

/*******************************************************************************
 ******************************************************************************/

#endif // _MAGTEKWRAPPER_H_
