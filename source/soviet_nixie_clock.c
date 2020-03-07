// includes
#include "soviet_nixie_clock.h"
#include <util/twi.h>
#include "i2c.h"

// custom defines
#define C_Ds1307ReadMode_U8   0xD1u // DS1307 ID
#define C_Ds1307WriteMode_U8  0xD0u // DS1307 ID
#define ControlRegAddress     0x07u // control register address
#define SecondRegAddress_U8   0x00u // first register address
#define HourRegAddress_U8     0x02u // hour register address

// additional custom defines
#define SECONDS_IN_A_HALF_DAY 43200
#define TIME_UPPER_BOUND SECONDS_IN_A_HALF_DAY
#define TIME_LOWER_BOUND 0

// functions definitions
void setTime ( struct sncData *snc, short adjustment ) {

    // circulate timeData between two numerical bound TIME_UPPER_BOUND, and TIME_LOWER_BOUND based on input adjustment

	short adjustment_abs;
	unsigned short modification;

	adjustment_abs = ( adjustment < 0 ) ? -adjustment : adjustment;

	( *snc ).timeData = (( *snc ).timeData / adjustment_abs ) * adjustment_abs;
	modification = ( *snc ).timeData + adjustment;
   
	// reset all values below the digits being adjusted
	if ( adjustment < 0 ) {
		adjustment *= -1;
	}

	if (( modification >= TIME_LOWER_BOUND ) && ( modification < TIME_UPPER_BOUND )) {
		( *snc ).timeData = modification;
	}

	return;
}

// increments time by one second
void autoIncrement ( struct sncData *snc ) {
	( *snc ).timeData = ++( *snc ).timeData % SECONDS_IN_A_HALF_DAY;

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

    // connections between the tubes of the second's 10s places and 1s place have been switched
    ( *snc ).secondDisplay  = (( seconds / 10 ) << 0 ); // 1s place
    ( *snc ).secondDisplay |= (( seconds % 10 ) << 4 ); // 10s place

    return;
}

/* Selecting the units HOUR, MINUTE, SECOND, or ALL will switch off that segment of the display */
void clearDisplaySegments ( struct sncData *snc ) {
	
	( *snc ).hourDisplay = 0xAA; // 0 to 9 correspond to a number. #10 to 0xFF will not light anything
	( *snc ).minuteDisplay = 0xAA;
	( *snc ).secondDisplay = 0xAA;
	
	return;
}

/* initialize the RTC communication channel using TWI.h */
void initialize__RTC ( void ) {

    unsigned char hourRegisterData;

    TWIStart(); // send start bit
    TWIWrite( C_Ds1307WriteMode_U8 ); // send the address of the RTC module and set it in write mode
    TWIWrite( ControlRegAddress ); // tell the rtc module the register to write data to
    TWIWrite( 0x00 ); // send the data
    TWIStop(); // send stop bit

    /*
    ////////// Setting the RTC module in 12 - hour mode
    //
    // select the address to first read from
    TWIStart();
    TWIWrite( C_Ds1307WriteMode_U8 );
    TWIWrite( HourRegAddress_U8 );
    TWIStop();

    // collect the data stored in the address
    TWIStart();
    TWIWrite( C_Ds1307ReadMode_U8 );
    hourRegisterData = TWIReadNACK();
    TWIStop();

    // modify the data
    hourRegisterData |= 0x40; // set the bit in hourRegsiterData to 12-hour mode

    // send the modified data back
    TWIStart(); // send start bit
    TWIWrite( C_Ds1307WriteMode_U8 ); // send the address of the RTC module and set in write mode
    TWIWrite( HourRegAddress_U8 ); // submit the address of the RTC to write to. In this case the hour register
    TWIWrite( hourRegisterData ); // set the data of the hour address to register 12-hour mode
    TWIStop(); // send the stop bit

    */
    return;
}

/* save the time of snc to the RTC module */
// https://embedds.com/programming-avr-i2c-interface/
void saveTimeToRTC ( struct sncData *snc ) {

    unsigned char secondsInHex, minutesInHex, hoursInHex;

    secondsInHex = ( *snc ).secondDisplay;
    minutesInHex = ( *snc ).minuteDisplay;
    hoursInHex   = ( *snc ).hourDisplay;

    // begin i2C communication and store variables in BCD hex fomat
    TWIStart(); // send the start bit
    TWIWrite( C_Ds1307WriteMode_U8 ); // send the mode
    TWIWrite( SecondRegAddress_U8 ); // send the address to write to

    TWIWrite( secondsInHex ); // send the seconds data
    TWIWrite( minutesInHex ); // send the minutes data
    TWIWrite( hoursInHex ); // end the hour data
    TWIStop(); // send the stop bit
    
    return;
}


/* retrieve the time from the RTC module */
void recallTimeFromRTC ( struct sncData *snc ) {

    unsigned char secondsInHex, minutesInHex, hoursInHex;
    unsigned char secondsInDec, minutesInDec, hoursInDec;
    unsigned short totalTimeInSeconds;

    // begin communication
    TWIStart(); // send start bit
    // set the module in read mode
    TWIWrite( C_Ds1307WriteMode_U8 ); // set the module in write mode
    TWIWrite( SecondRegAddress_U8 ); // send the addres to read from
    TWIStop(); // send stop bit


    TWIStart(); // send start bit
    TWIWrite( C_Ds1307ReadMode_U8 ); // connect to DS1307(Read mode) by sending its ID
    secondsInHex = TWIReadACK(); // read second and return Positive ACK
    minutesInHex = TWIReadACK(); // read minute and return Positive ACK
    hoursInHex = TWIReadNACK(); // read hour and return Not ACK
    TWIStop(); // send stop bit
    /* DISTINCTION BETWEEN NOT ACKNOWLEDGE ( MASTER READS FROM SLAVE ) AND NO ACKNOWLEDGE ( MASTER WRITES TO SLAVE ) http://www.esacademy.com/en/library/technical-articles-and-documents/miscellaneous/i2c-bus/i2c-bus-events/giving-acknowledge-to-a-slave-device.html*/

    //++hoursInHex; // set the value back to 12-hour bracket

    hoursInDec   = (( hoursInHex   / 16 ) * 10 ) + ( hoursInHex   % 16 );
    minutesInDec = (( minutesInHex / 16 ) * 10 ) + ( minutesInHex % 16 );
    secondsInDec = (( secondsInHex / 16 ) * 10 ) + ( secondsInHex % 16 );
    
    --hoursInDec;

    totalTimeInSeconds = ( 3600 * hoursInDec ) + ( 60 * minutesInDec ) + ( secondsInDec );
    ( *snc ).timeData = totalTimeInSeconds;

    return;
}
