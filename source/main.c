/*
 *
 *	Author: Ratnodeep Bandyopadhyay
 *	Project: Nixie Clock
 *
 *
 *
 *
 *
 */

#ifdef _SIMULATE_
#include "simAVRHeader.h" // not including simulator
#endif

#include "timer_isr.h"
#include "soviet_nixie_clock.h"

// defines
#define true 1
#define TOTAL_TASKS 4
#define PERIOD_timer 1000
#define PERIOD_adjustData 1
#define PERIOD_display 1
#define PERIOD_sample 50
#define PERIOD_GCD 1

// additional macros
#define unt ( !I0 && !I1 )
#define bot ( I0 && I1 )
#define inc ( I0 && !I1 )
#define dec ( !I0 && I1 )

struct _task {

	unsigned long elapsedTime; // total time elapsed between each tick
	unsigned long period; // the total amount of time to have elapsed before executing the task

	void ( *tick ) ( void ); // the events of a task

};


// possible states for each function
enum _States_sample { START_sample, INIT, MAST_WAIT, IDLE, MINC_WAIT, MINC, MINC_RAPID, MDEC_WAIT, MDEC, MDEC_RAPID, SAVE_TIME_WAIT, SAVE_TIME, STOP_DISPLAY };


// global variables
static struct sncData gSnc;
static struct _task gTasks[ TOTAL_TASKS ];
static unsigned char gTimerFlag = 0;
static unsigned char gAutoIncrementFlag = 0;
static unsigned char gDecFlag = 0;
static unsigned char gIncFlag = 0;
static unsigned char gSaveFlag = 0;
static unsigned char gReadFlag = 1;
static unsigned char gDisplayFlag = 1;
static short gAdjustmentFactor = 1;

// additional functions
void tick_timer ( void );
void tick_adjustData ( void );
void tick_display ( void );
void tick_sample ( void );
void executeTasks ( void );
void initializeTasks ( void );
void initializePorts ( void );
void initializeTimer ( void );
void initialize_gSnc ( void );
// global test function
void testFunction ( unsigned char TestID );

/******************************* MAIN FUNCTION *****************************/

int main ( void ) {

	initializeTimer();
	initializePorts();
	initializeTasks();
	initialize_gSnc();
	initialize__RTC();

	while ( true ) {
	
		while ( !gTimerFlag ); // will execute only at the frequency of PERIOD_GCD
		gTimerFlag = 0;

		
		//testFunction( 3 );

		executeTasks();
	}

	return 1;

}

/////////////////////////////////////////////////////////////////////////////

void testFunction ( unsigned char TestID ) {

	switch ( TestID ) {
		case 1:
			PORTA = PORTB = PORTD = ( ~PINC );
			break;

		case 2:
			//gSnc.timeData = 39599; // max possible time is 11:59:59
			gSnc.timeData = 43199; // max possible time is 12:59:59
			setTime( &gSnc, -600 );
			tick_display();
			break;

		case 3:
			if ( PINC ) {
				gSnc.timeData = 39000;
				setTime( &gSnc, 600 );
				break;
			}
	}

	return;
}

void executeTasks ( void ) {

	for ( unsigned char i = 0; i < TOTAL_TASKS; ++i ) {

		gTasks[i].elapsedTime += PERIOD_GCD; // log the total time elpased into that task variable

		if ( gTasks[i].elapsedTime == gTasks[i].period ) {
			gTasks[i].elapsedTime = 0; // if the task period has complete, reset the total time elpased
			gTasks[i].tick(); // and execute the task's duties
		}

	}

	return;
}


void initialize_gSnc ( void ) {

	gSnc.timeData = 0;
	gSnc.hourDisplay = 0;
	gSnc.minuteDisplay = 0;
	gSnc.secondDisplay = 0;

	return;
}

void initializeTimer ( void ) {

	TimerSet( PERIOD_GCD );
	TimerOn();

	return;
}


void initializePorts ( void ) {

	// PORT initializations
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0x00; PORTC = 0xFF; // other ports are used to display time. Becuase PORTC uses I2C, button inputs are mapped into this port aswell
	DDRD = 0xFF; PORTD = 0x00;

	return;
}

void initializeTasks ( void ) {

	gTasks[0].elapsedTime = 0;
	gTasks[0].period = PERIOD_timer;
	gTasks[0].tick = tick_timer;

	gTasks[1].elapsedTime = 0;
	gTasks[1].period = PERIOD_adjustData;
	gTasks[1].tick = tick_adjustData;

	gTasks[2].elapsedTime = 0;
	gTasks[2].period = PERIOD_display;
	gTasks[2].tick = tick_display;

	gTasks[3].elapsedTime = 0;
	gTasks[3].period = PERIOD_sample;
	gTasks[3].tick = tick_sample;

	return;
}

void tick_sample ( void ) {
	
	static unsigned char timerCount = 0;
	static enum _States_sample state_sample = START_sample;

	// sampler characters
	unsigned char I0;
	unsigned char I1;

	I0 = ~PINC & 0x08;
	I1 = ~PINC & 0x04;

	// transitions
	switch ( state_sample ) {
		
		case START_sample:
			state_sample = INIT;
			break;

		case INIT:
			state_sample = MAST_WAIT;
			break;

		case MAST_WAIT:
			state_sample = ( unt ) ? IDLE : MAST_WAIT;
			break;

		case IDLE:
			if ( unt ) {
				state_sample = IDLE;
			}
			else if ( inc ) {
				state_sample = MINC_WAIT;
			}
			else if ( dec ) {
				state_sample = MDEC_WAIT;
			}
			else if ( bot ) {
				state_sample = SAVE_TIME_WAIT;
			}
			else {
				state_sample = IDLE;
			}

			break;

		case MINC_WAIT:
			if ( bot ) {
				state_sample = SAVE_TIME_WAIT;
			}
			else if (( timerCount < 10 ) && inc ) {
				state_sample = MINC_WAIT;
			}
			else if (( timerCount < 10 ) && !inc ) {
				state_sample = MINC;
			}
			else if (( timerCount >= 10 ) && inc ) {
				state_sample = MINC_RAPID;
			}
		        else if (( timerCount >= 10 ) && !inc ) {
				state_sample = MAST_WAIT;
			}
			else {
				state_sample = MINC_WAIT;
			}

			break;


		case MINC:
			state_sample = MAST_WAIT;
			break;


		case MINC_RAPID:
			if ( inc ) {
				state_sample = MINC_RAPID;
			}
			else {
				state_sample = MAST_WAIT;
			}

			break;


		case MDEC_WAIT:
			if ( bot ) {
				state_sample = SAVE_TIME_WAIT;
			}
			else if (( timerCount < 10 ) && dec ) {
				state_sample = MDEC_WAIT;
			}
			else if (( timerCount < 10 ) && !dec ) {
				state_sample = MDEC;
			}
			else if (( timerCount >= 10 ) && dec ) {
				state_sample = MDEC_RAPID;
			}
			else if (( timerCount >= 10 ) && !dec ) {
				state_sample = MAST_WAIT;
			}
			else {
				state_sample = MDEC_WAIT;
			}

			break;


		case MDEC:
			state_sample = MAST_WAIT;
			break;

		case MDEC_RAPID:
			if ( dec ) {
				state_sample = MDEC_RAPID;
			}
			else {
				state_sample = MAST_WAIT;
			}

			break;

		case SAVE_TIME_WAIT:
			if (( timerCount < 10 ) && bot ) {
				state_sample = SAVE_TIME_WAIT;
			}
			else if (( timerCount >= 10 ) && bot ) {
				state_sample = SAVE_TIME;
			}
			else if ( !bot ) {
				state_sample = MAST_WAIT;
			}
			else {
				state_sample = SAVE_TIME_WAIT;
			}

			break;

		case SAVE_TIME:
			state_sample = STOP_DISPLAY;
			break;

		case STOP_DISPLAY:
			if ( timerCount < 10 ) {
				state_sample = STOP_DISPLAY;
			}
			else {
				state_sample = MAST_WAIT;
			}

			break;
	}

	// actions
	switch ( state_sample ) {

		case START_sample: 
			break;
		
		case INIT:
			gReadFlag = 1;
			break;
		
		case MAST_WAIT:
			timerCount = 0;
			break;

		case IDLE :
			break;

		case MINC_WAIT:
			timerCount++;
			break;

		case MINC:
			gIncFlag = 1;
			gAdjustmentFactor = 60;
			break;

		case MINC_RAPID:
			gIncFlag = 1;
			gAdjustmentFactor = 600;
			break;

		case MDEC_WAIT:
			timerCount++;
			break;

		case MDEC:
			gDecFlag = 1;
			gAdjustmentFactor = -60;
			break;

		case MDEC_RAPID:
			gDecFlag = 1;
			gAdjustmentFactor = -600;
			break;

		case SAVE_TIME_WAIT:
			timerCount++;
			break;

		case SAVE_TIME:
			gSaveFlag = 1;
			timerCount = 0;
			break;

		case STOP_DISPLAY:
			timerCount++;
			break;
	}

	return;
}

void tick_timer ( void ) {
	gAutoIncrementFlag = 1;

	return;
}


void tick_adjustData ( void ) {

	static unsigned char autoIncrementDelay = 0;
	static const unsigned char maxAutoIncDelay = 1;

	if ( gIncFlag ) {
		setTime( &gSnc, gAdjustmentFactor );
		gIncFlag = 0;
		autoIncrementDelay = (( autoIncrementDelay + 1 ) > maxAutoIncDelay ) ? maxAutoIncDelay : autoIncrementDelay + 1;
		gDisplayFlag = 1;
	}
	else if ( gDecFlag ) {
		setTime( &gSnc, gAdjustmentFactor );
		gDecFlag = 0;
		autoIncrementDelay = (( autoIncrementDelay + 1 ) > maxAutoIncDelay ) ? maxAutoIncDelay : autoIncrementDelay + 1;
		gDisplayFlag = 1;
	}
	else if ( gSaveFlag ) {
		saveTimeToRTC( &gSnc ); // TODO:: Implement RTC save time functionality
		gSaveFlag = 0;
		gDisplayFlag = 0;
		autoIncrementDelay = (( autoIncrementDelay + 1 ) > maxAutoIncDelay ) ? maxAutoIncDelay : autoIncrementDelay + 1;
	}
	// ensure this case is the **FINAL** case
	else if ( gAutoIncrementFlag ) {
		if ( autoIncrementDelay == 0 ) {
			autoIncrement( &gSnc );
		}
		else {
			autoIncrementDelay--;
		}

		gAutoIncrementFlag = 0;
		gDisplayFlag = 1;
	}
	else if ( gReadFlag ) {
		recallTimeFromRTC( &gSnc ); // TODO:: insert function to read from RTC module
		gReadFlag = 0;
		gDisplayFlag = 1;
	}


	return;
}


void tick_display ( void ) {

	if ( gDisplayFlag ) {
		updateDisplaySegments( &gSnc );
	}
	else {
		clearDisplaySegments( &gSnc );
	}
	
	PORTA = gSnc.secondDisplay;
	PORTB = gSnc.minuteDisplay;
	PORTD = gSnc.hourDisplay;

	return;
}

void TimerISR ( void ) {
	gTimerFlag = 1;
}


ISR ( TIMER1_COMPA_vect ) {
	_avr_timer_cntcurr--;
	if ( _avr_timer_cntcurr == 0 ) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

