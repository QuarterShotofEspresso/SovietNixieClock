#ifndef _SOVIET_NIXIE_CLOCK_
#define _SOVIET_NIXIE_CLOCK_

// global variables
enum state_TimeIncrementer {START_timer, COUNT, TIME_INC} state_TIMER;
enum state_buttonSignals {START_button, INIT, IDLE, INC_WAIT, MAN_INC, SHIFT_LEFT, RESET_WAIT, SAVE_TIME, RESET, DECREMENT_WAIT, SHIFT_RIGHT, MAN_INC, MAST_WAIT} state_BUTTON;

unsigned char heldFor = 0, timerCount = 0;
unsigned char A0 = 0, A1 = 0;

// defines
#define HELD_FOR_THRESHOLD 10
#define TIMER_COUNT_THRESHOLD 10

// additional macros
#define unt ( !A0 && !A1 )
#define bot ( A0 && A1 )
#define inc ( A0 && !A1 )
#define dec ( !A0 && A1 )
#define HREQ ( heldFor >= HELD_FOR_THRESHOLD )

// additional functions
void samplePins ( void );


/************ CODE MAINTAINING TIME **************/

struct sncData {
    
    unsigned char timeUnitIndex;
    unsigned char timeData[3]; // {hour, minute, second} <-- the unitary order
    
    // functions dealing with time Data specificially
    void ( *shiftUnitaryIndex ) ( unsigned char *unitaryIndex );
    void ( *setTime ) ( unsigned char unitaryIndex, unsigned char value );
    // functions dealing with nixie driver display
    void ( *startDisplay ) ( void );
    void ( *stopDisplay ) ( unsigned char segment );
    // functions dealing with RTC module
    void ( *saveTime ) ( struct sncData *data );
    void ( *recallTime ) ( struct sncData *time );

};


void shiftUnitaryIndex ( unsigned char *unitaryIndex ); // pass by reference so to change the value of the original variable
void setTime ( unsigned char unitaryIndex, unsigned char value ); // this function sets the time to a specifiable value

/************ CODE MAINTAINING DISPLAY **************/

void startDisplay ( void ); // sets the display to the current data loaded into timeData;
void stopDisplay ( unsigned char segment ); // turns off the display

/************** CODE MAINTAINING RTC ****************/

void saveTime ( struct sncTimeData *time );
void recallTime ( struct sncTimeData *time );

//////////////////////////////////////////////////////



/************ TICK FUNCTION DECLARATIONS ************/



struct task {

    unsigned char task_state; // current state in the task
    unsigned long elapsedTime; // total time elapsed between each tick
    unsigned long period; // the total amount of time to have elapsed before executing the task

    void ( *tick ) ( unsigned char task_state ); // the events of a task

};

// all snc tasks available
void tick_timer ( unsigned char task_state );
void tick_buttons ( unsigned char task_state );

//////////////////////////////////////////////////////

#endif /* _SOVIET_NIXIE_CLOCK */
