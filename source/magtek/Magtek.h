/***************************************************************************//**
  @file     Magtek.h
  @brief    Mangetic stripe reader
  @author   Group 5
 ******************************************************************************/

#ifndef _MAGTEK_H_
#define _MAGTEK_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PAN_MAX_L	19
#define ADDIT_L		7
#define DISCR_L		8

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	char id[PAN_MAX_L];
	char additional[ADDIT_L];
	char discretionary[DISCR_L];
}data;

typedef void (*cardCb) (bool state, const data* mydata);

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief: Initialize pins and set intrruption for Magtek
 * @param fubCb Callback to return the data
 * @return initialization succeed
*/
bool CardInit(cardCb funCb);

/*******************************************************************************
 ******************************************************************************/

#endif // _MAGTEK_H_
