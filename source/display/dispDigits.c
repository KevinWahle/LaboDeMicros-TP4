/***************************************************************************//**
  @file		dispDigits.c
  @brief    7 Segment Display Digits definitions and functions
  @author   Grupo 5
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include  "dispDigits.h"

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const dispDigit_t digitsNum[] = DISP_DIGITS_NUM;
static const dispDigit_t digitsAlph[] = DISP_DIGITS_ALPH;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief Get the digit that corresponds to the character given.
 * @param c: the character.
 * @return the digit that represents c in the display (according DispDigits, DISP_OFF if unavailable)
 */
dispDigit_t char2Digit(char c) {
	if (c >= '0' && c <= '9') {
		return digitsNum[c-'0'];
	}
	else if (c>= 'a' && c <= 'z') {
		return digitsAlph[c-'a'];
	}
	else if (c >= 'A' && c <= 'Z') {
		return digitsAlph[c-'A'];
	}
	else if (c == '-') {
		return DISP_HYP;
	}
	else if (c == '_') {
		return DISP_UND;
	}

	return DISP_OFF;
}

/**
 * @brief Get an array of digits that generate the string provided.
 * @param *s: the string.
 * @param digits: array of digits that generates s in the display (according DispDigits)
 */
void string2Digit(const char* s, dispDigit_t* digits) {
	while (*s) {	// Continue until terminator

		*digits = char2Digit(*s);

		s++;			// Next char
		digits++;		// and next digit
	}
}


/**
 * @brief Get an array of digits that generate the number provided.
 * @param num: the number.
 * @param digits: array of digits that generates num in the display (according DispDigits)
 */
void num2Digit(uint32_t num, dispDigit_t* digits) {

	dispDigit_t* pdig = digits;

	if (num) {
		// num != 0
		while (num) {
			*pdig = digitsNum[num%10];	// Get the digits in reverse order

			num /= 10;
			pdig++;
		}

		while (--pdig > digits) {		// Reverse the order

			dispDigit_t temp = *pdig;

			*pdig = *digits;

			*digits = temp;

			digits++;

		}
	}
	else {
		// num == 0
		*digits = digitsNum[0];
	}


}
