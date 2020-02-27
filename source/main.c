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
#define TOTAL_TASKS 3
#define PERIOD_timer 1000
#define PERIOD_adjustData 16
#define PERIOD_display 16
#define PERIOD_GCD 8

// additional macros
#define unt ( !A0 && !A1 )
#define bot ( A0 && A1 )
#define inc ( A0 && !A1 )
#define dec ( !A0 && A1 )
#define HREQ ( heldFor >= HELD_FOR_THRESHOLD )

struct _task {

	unsigned long elapsedTime; // total time elapsed between each tick
	unsigned long period; // the total amount of time to have elapsed before executing the task

	void ( *tick ) ( void ); // the events of a task

};


// possible states for each function

// global variables
static struct sncData gSnc;
static struct _task gTasks[ TOTAL_TASKS ];
static unsigned char gTimerFlag = 0;
static unsigned char gAutoIncrementFlag = 0;
static unsigned char gDecFlag = 0;
static unsigned char gIncFlag = 0;
static unsigned char gSaveFlag = 0;

// additional functions
void tick_timer ( void );
void tick_adjustData ( void );
void tick_display ( void );
void executeTasks ( void );
void initializeTasks ( void );
void initializePorts ( void );
void initializeTimer ( void );
void initialize_gSnc ( void );

// static definitions that need to be made
//static unsigned char gA1 = 0;
//static unsigned char gA0 = 0;


/******************************* MAIN FUNCTION *****************************/

int main ( void ) {

	initializeTimer();
	initializePorts();
	initializeTasks();
	initialize_gSnc();

	while ( true ) {
	
		while ( !gTimerFlag ); // will execute only at the frequency of PERIOD_GCD
		gTimerFlag = 0;

		// PORTA = PORTB = PORTD = ( ~PINC ); //test case
		executeTasks();
	}

	return 1;

}

/////////////////////////////////////////////////////////////////////////////


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

	gSnc.timeData = 13500;
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

	return;
}

void tick_timer ( void ) {
	gAutoIncrementFlag = 1;

	return;
}


void tick_adjustData ( void ) {

	if ( gIncFlag ) {
		setTime( &gSnc, INCREMENT );
		gIncFlag = 0;
	}
	else if ( gDecFlag ) {
		setTime( &gSnc, DECREMENT );
		gDecFlag = 0;
	}
	else if ( gSaveFlag ) {
		saveTimeToRTC( &gSnc );
		gSaveFlag = 0;
	}
	// ensure this case is the **FINAL** case
	else if ( gAutoIncrementFlag ) {
		autoIncrement( &gSnc );
		gAutoIncrementFlag = 0;
	}

	return;
}


void tick_display ( void ) {

	updateDisplaySegments( &gSnc );
	
	PORTA = gSnc.secondDisplay;
	PORTB = gSnc.minuteDisplay;
	PORTD = gSnc.hourDisplay;

	return;
}


void tick_buttons ( void ) { }


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

