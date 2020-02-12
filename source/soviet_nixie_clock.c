// includes
#include "soviet_nixie_clock"


// functions definitions
void samplePins ( void ) {
    
    A0 = ( ~PINC & 0x04 ) >> 2;
    A1 = ( ~PINC & 0x08 ) >> 3;

    return;
}


/////// function definitions for sncData
void shiftUnitaryIndex ( struct sncData *snc, unsigned char select_hour ) {

    if ( select_hour ) { // if an decrement is requested, the index is shifted left
        ( *snc ).timeUnitIndex = 0;
    } else {
	( *snc ).timeUnitIndex = 1;
    }

    return;
}


void setTime ( struct sncData *snc, unsigned char value ) {

    ( *snc ).timeData[( *snc ).unitaryIndex] = value;

    return;
}


void startDisplay ( struct sncData *snc ) {

    // each port is responsible for one unit of time. eg. PORTA: Hours, PORTB: Minutes, PORTD: Seconds
    unsigned char displayOnA, displayOnB, displayOnD;

    // to calculate the 10s place value.
    displayOnA  = ((( *snc ).timeData[0] / 10 ) << 4 ); // shift the value to the upper nibble of the output PORT
    displayOnA |= ((( *snc ).timeData[0] % 10 ) << 0 ); // we can OR in the 1s place as the lower nibble

    displayOnB  = ((( *snc ).timeData[1] / 10 ) << 4 ); // 10s place
    displayOnB |= ((( *snc ).timeData[1] % 10 ) << 0 ); // 1s place

    displayOnD  = ((( *snc ).timeData[2] / 10 ) << 4 ); // 10s place
    displayOnD |= ((( *snc ).timeData[2] % 10 ) << 0 ); // 1s place


    return;
}


void stopDisplaySegment ( struct sncData *snc ) {

    if (( *snc ).timeData[0] =  )











    return;

}


/********************************* TICK FUNCTION DEF ***********************************/

void tick_timer ( unsigned char task_state ) {
	
	





