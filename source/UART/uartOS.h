/***************************************************************************//**
  @file     uartOS.c
  @brief	Funciones para comunicación por UART en RTOS
  @author	Grupo 5
  @date		12 nov. 2022
 ******************************************************************************/

#ifndef _UARTOS_H_
#define _UARTOS_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_CANT_IDS   5


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {NO_PARITY, EVEN_PARITY, ODD_PARITY} PARITY_TYPE;

typedef struct {
    uint32_t baudrate;
    PARITY_TYPE parity;
    bool MSBF;    // MSB First
} uart_cfg_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize UART driver
 * @param id UART's number
 * @param config UART's configuration (baudrate, parity, etc.)
*/
void uartOSInit (uint8_t id, uart_cfg_t config);

/**
 * @brief Waits for a byte to be received
 * @param id UART's number
 * @param timeout in ms. 0 to disable
 * @return true if message, false if error (timeout)
*/
bool uartOSWaitMsg(uint8_t id, uint32_t timeout);

/**
 * @brief Check how many bytes were received
 * @param id UART's number
 * @return Quantity of received bytes
*/
uint8_t uartGetRxMsgLength(uint8_t id);

/**
 * @brief Read a received message. Non-Blocking
 * @param id UART's number
 * @param msg Buffer to paste the received bytes
 * @param cant Desired quantity of bytes to be pasted
 * @return Real quantity of pasted bytes
*/
uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant);

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param id UART's number
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
 * @return Real quantity of bytes to be transfered
*/
uint8_t uartOSWriteMsg(uint8_t id, const char* msg, uint8_t cant);

/**
 * @brief Check if all bytes were transfered
 * @param id UART's number
 * @return All bytes were transfered
*/
bool uartIsTxMsgComplete(uint8_t id);


/*******************************************************************************
 ******************************************************************************/

#endif // _UARTOS_H_
