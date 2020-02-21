#ifndef _I2C_H_
#define _I2C_H_

/*
 *
 * Code Sourced from:
 *     https://embedds.com/programming-avr-i2c-interface/
 *
 */

#include <util/twi.h>

void TWIInit  ( void ); 
void TWIStart ( void );
void TWIStop  ( void );
void TWIWrite ( unsigned char u8data );
unsigned char TWIReadACK   ( void );
unsigned char TWIReadNACK  ( void );
unsigned char TWIGetStatus ( void );
void TWIInit ( void );

#endif
