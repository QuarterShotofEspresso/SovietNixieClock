//#include "simAVRHeader.h" // not including simulator
#include "soviet_nixie_clock.h"

// global variables
enum state_TimeIncrementer {START_timer, COUNT, TIME_INC} state_TIMER;
enum state_buttonSignals {START_button, INIT, IDLE, INC_WAIT, MAN_INC, SHIFT_LEFT, RESET_WAIT, SAVE_TIME, RESET, DECREMENT_WAIT, SHIFT_RIGHT, MAN_INC, MAST_WAIT} state_BUTTON;

unsigned char heldFor = 0, timerCount = 0;
unsigned char A0 = 0, A1 = 0;

// defines
#define HELD_FOR_THRESHOLD 10

// additional macros
#define unt ( !A0 && !A1 )
#define bot ( A0 && A1 )
#define inc ( A0 && !A1 )
#define dec ( !A0 && A1 )
#define HREQ ( heldFor >= HELD_FOR_THRESHOLD )


// additional functions
void samplePins ( void );

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
