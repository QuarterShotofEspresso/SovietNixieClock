//#include "simAVRHeader.h" // not including simulator
#include "soviet_nixie_clock.h"

/*************  MAIN FUNCTION  **************/
int main ( void ) {
    // PORT initializations
    DDRA = 0xFF; PORTA = 0x00;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0x00; PORTC = 0xFF; // other ports are used to display time. Becuase PORTC uses I2C, button inputs are mapped into this port aswell
    DDRD = 0xFF; PORTD = 0x00;

    // initialize Timer Interrupt


    // forever
    while ( true ) {
	samplePins();
	tick_timer();
	tick_buttons();
	updateSNC();
    }

    return 1;
}
/////////////////////////////////////////////

void samplePins ( void ) {
    
    A0 = ( ~PINC & 0x04 ) >> 2;
    A1 = ( ~PINC & 0x08 ) >> 3;

    return;
}
