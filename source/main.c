#include "simAVRHeader.h"
#include "soviet_nixie_clock.h"

// global variables
enum state_TimeIncrementer {START, INIT, COUNT, TIME_INC} state_TIMER;
enum state_buttonSignals {STATE, IDLE, INC_WAIT, MANUAL_INC, SHIFT_LEFT, SHIFT_LEFT_WAIT, RESET_WAIT, SAVE_TIME, RESET, DECREMENT_WAIT, SHIFT_RIGHT, MANUAL_INC} state_BUTTON;

unsigned char heldFor = 0, timerCount = 0;

// additional functions




int main ( void ) {
    


    while ( true ) {
	}
