//#include "simAVRHeader.h" // not including simulator
#include "soviet_nixie_clock.h"

// defines
#define true 1
#define HELD_FOR_THRESHOLD 10
#define TIMER_COUNT_THRESHOLD 10
#define MAX_TIME 43200
#define TOTAL_TASKS 3
#define PERIOD_resetClk
#define PERIOD_shiftUnit
#define PERIOD_
#define PERIOD_GCD

// additional macros
#define unt ( !A0 && !A1 )
#define bot ( A0 && A1 )
#define inc ( A0 && !A1 )
#define dec ( !A0 && A1 )
#define HREQ ( heldFor >= HELD_FOR_THRESHOLD )

// additional functions
void samplePins ( void );
void tick_timer ( void );
void tick_buttons ( void );
void initializeTasks ( void );
void initializePORTS ( void );

struct task {

	unsigned long elapsedTime; // total time elapsed between each tick
	const unsigned long period; // the total amount of time to have elapsed before executing the task

	void ( *tick ) ( void ); // the events of a task

};

// global task array
struct task gTasks[ TOTAL_TASKS ];
unsigned char gA0 = 0;
unsigned char gA1 = 0;

int main ( void ) {

	struct sncData sncInstance;

	// initialize everything
	void TimerSet ( PERIOD_GCD );
	void TimerOn();

	initializePORTS();
	initializeTasks();

	while ( true ) { }

	return 1;

}

/////////////////////////////////////////////

unsigned char samplePins ( unsigned char bitSelect ) {

	unsigned char bitMask = ( 0x01 ) << bitSelect;
	return ( ~PINA & bitMask ) >> bitSelect;

}

void initializePORTS ( void ) {
	// PORT initializations
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0x00; PORTC = 0xFF; // other ports are used to display time. Becuase PORTC uses I2C, button inputs are mapped into this port aswell
	DDRD = 0xFF; PORTD = 0x00;

	return;
}

void initializeTasks ( void ) { }
void tick_timer ( void ) { }
void tick_buttons ( void ) { }
