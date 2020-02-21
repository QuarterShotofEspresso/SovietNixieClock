/*
 *
 * Code Sourced from:
 *     https://embedds.com/programming-avr-i2c-interface/
 *
 */

#include "i2c.h"

/* Set the rate of SCL and initiate I2C */
void TWIInit ( void )
{
    //set SCL to 400kHz
    TWSR = 0x00;
    TWBR = 0x0C;
    //enable TWI
    TWCR = (1<<TWEN);
}

/* Send the START bit */
void TWIStart ( void )
{
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
}

/* Send the STOP bit */
void TWIStop ( void )
{
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

/* write 8-bit data to the SDA wire */
void TWIWrite ( unsigned char u8data )
{
    TWDR = u8data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
}

/* Read and return an ACKnowledge */
unsigned char TWIReadACK ( void )
{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;
}

/* read byte with NACK */
unsigned char TWIReadNACK ( void )
{
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;
}

/* See if the message transfer was succesful */
unsigned char TWIGetStatus ( void )
{
    uint8_t status;
    //mask status
    status = TWSR & 0xF8;
    return status;
}

