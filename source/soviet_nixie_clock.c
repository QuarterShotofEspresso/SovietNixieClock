// includes
#include "soviet_nixie_clock.h"
#include <util/twi.h>
#include "i2c.h"

// custom defines
#define C_Ds1307ReadMode_U8   0xD1u // DS1307 ID
#define C_Ds1307WriteMode_U8  0xD0u // DS1307 ID
#define SecondRegAddress_U8   0x00u // first register address

// functions definitions
void setTime ( struct sncData *snc, enum TimeAdjustments adjustment ) {

    if ( adjustment == INCREMENT ) {
        timeData = ( ++timeData < TIME_UPPER_BOUND ) ? timeData : TIME_LOWER_BOUND; // standard cyclic incrementer
    }
    else if ( adjustment == DECREMENT ) {
        timeData = ( --timeData > TIME_LOWER_BOUND ) ? timeData : TIME_UPPER_BOUND; // standard cyclic decrementer
    }
    
    // clear the seconds
    timeData = ( timeData / 60 ) * 60;

    return;
}

/* update the display segments on the snc */
void updateDisplaySegments ( struct sncData *snc ) {

    unsigned char hours, minutes, seconds;

    // convert the total seconds in timeData into hours, minutes, and seconds for the display
    hours   = (( *snc ).timeData / 3600 ) + 1;   // incrementing by 1 shifts the hour range from 0-11 to 1-12
    minutes = (( *snc ).timeData % 3600 ) / 60;
    seconds = (( *snc ).timeData % 3600 ) % 60;

    // to calculate the 10s place value.
    ( *snc ).hourDisplay    = (( hours   / 10 ) << 4 ); // shift the value to the upper nibble of the output PORT
    // to calcualte the 1s place value
    ( *snc ).hourDisplay   |= (( hours   % 10 ) << 0 ); // we can OR in the 1s place as the lower nibble

    ( *snc ).minuteDisplay  = (( minutes / 10 ) << 4 ); // 10s place
    ( *snc ).minuteDisplay |= (( minutes % 10 ) << 0 ); // 1s place

    ( *snc ).secondDisplay  = (( seconds / 10 ) << 4 ); // 10s place
    ( *snc ).secondDisplay |= (( seconds % 10 ) << 0 ); // 1s place

    return;
}

/* Selecting the units HOUR, MINUTE, SECOND, or ALL will switch off that segment of the display */
void clearDisplaySegment ( struct sncData *snc, enum UnitsOfTime segment ) {

    if (( segment == HOUR ) || ( segment == ALL )) {
        ( *snc ).hourDisplay = 0x0A; // 0 to 9 correspond to a number. #10 to 0xFF will not light anything
    }
    else if (( segment == MINUTE ) || ( segment == ALL )) {
	( *snc ).minuteDisplay = 0x0A;
    }
    else if (( segment == SECOND ) || ( segment == ALL )) {
	( *snc ).secondDisplay = 0x0A;
    }

    return;
}

/* initialize the RTC communication channel using TWI.h */
void initRTC ( void ) {

    TWIStart(); // send start bit
    TWIWrite ( C_Ds1307WriteMode_U ); // send the address of the RTC module and set it in write mode
    TWIWrite ( ControlRegAddress ); // tell the rtc module the register to write data to
    TWIWrite ( 0x00 ); // send the data
    TWIStop(); // send stop bit

    return;
}

/* save the time of snc to the RTC module */
// https://embedds.com/programming-avr-i2c-interface/
void saveTime ( struct sncData *snc ) {

    // begin i2C communication and store variables in BCD hex fomat
    TWIStart (); // send the start bit
    TWIWrite ( C_Ds1307WriteMode_U ); // send the mode
    TWIWrite ( address ); // send the address to write to

    TWIWrite ( secondDisplay ); // send the seconds data
    TWIWrite ( minuteDisplay ); // send the minutes data
    TWIWrite ( hourDisplay ); // end the hour data
    TWIStop (); // send the stop bit
    
    return;
}


/* retrieve the time from the RTC module */
void recallTime ( struct sncData *snc ) {

    unsigned char seconds, minutes, hours;

    // begin communication
    TWIStart (); // send start bit
    // set the module in read mode
    TWIWrite ( C_Ds1307WriteMode_U ); // set the module in write mode
    TWIWrite ( SecondRegAddress_U8 ); // send the addres to read from
    TWIStop  (); // send stop bit


    TWIStart (); // send start bit
    TWIWrite ( C_Ds1307ReadMode_U8 ); // connect to DS1307(Read mode) by sending its ID
    seconds = TWIReadACK (); // read second and return Positive ACK
    minutes = TWIReadACK (); // read minute and return Positive ACK
    hours   = TWIReadNACK (); // read hour and return Not ACK
    TWIStop  (); // send stop bit
    /* DISTINCTION BETWEEN NOT ACKNOWLEDGE ( MASTER READS FROM SLAVE ) AND NO ACKNOWLEDGE ( MASTER WRITES TO SLAVE ) http://www.esacademy.com/en/library/technical-articles-and-documents/miscellaneous/i2c-bus/i2c-bus-events/giving-acknowledge-to-a-slave-device.html*/

    // (( time >> 4 ) * 10 ) + ( time & 0x0F ))
    ( *snc ).timeData = ( 3600 * hours ) + ( 60 * minutes ) + seconds;

    return;
}
