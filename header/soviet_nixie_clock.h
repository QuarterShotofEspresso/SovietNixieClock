#ifndef _SOVIET_NIXIE_CLOCK_
#define _SOVIET_NIXIE_CLOCK_


/************ CODE MAINTAINING TIME **************/

struct sncTimeData {
    
    unsigned char timeUnitIndex;
    unsigned char timeData[3]; // {hour, minute, second} <-- the unitary order
    
    // function selecting which unit of time to adjust
    void ( *shiftUnitaryIndex ) ( unsigned char *unitaryIndex );
    void ( *setTime ) ( unsigned char unitaryIndex, unsigned char value );
}


void shiftUnitaryIndex ( unsigned char *unitaryIndex ); // pass by reference so to change the value of the original variable
void setTime ( unsigned char unitaryIndex, unsigned char value ); // this function sets the time to a specifiable value

///////////////////////////////////////////////////




/************ CODE MAINTAINING DISPLAY **************/

void startDisplay ( void ); // sets the display to the current data loaded into timeData;
void stopDisplay ( unsigned char segment ); // turns off the display

//////////////////////////////////////////////////////




/************ CODE MAINTAINING RTC **************/

void saveTime ( struct sncTimeData *time );
void recallTime ( struct sncTimeData *time );

///////////////////////////////////////////////////




/************ TICK FUNCTION DECLARATIONS ************/

void tick_timer ( void );
void tick_buttons ( void );

//////////////////////////////////////////////////////

#endif /* _SOVIET_NIXIE_CLOCK */
