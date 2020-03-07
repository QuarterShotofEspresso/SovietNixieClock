#ifndef _SOVIET_NIXIE_CLOCK_
#define _SOVIET_NIXIE_CLOCK_

// enumerations
enum UnitsOfTime { HOUR, MINUTE, SECOND, ALL };
enum TimeAdjustments { INCREMENT = 60, DECREMENT = -60 };

/************ CODE MAINTAINING TIME **************/
// instance providing all the data required to operate 
struct sncData {
    
    // NOTE:: timeData is not unsigned for increment and decrement operations
    unsigned short timeData; // stores time in seconds
    unsigned char hourDisplay, minuteDisplay, secondDisplay; // values to display on 8-bit PORTS

};

/************ CODE MAINTAINING TIME *****************/

void setTime ( struct sncData *snc, short adjustment ); // adjust time based on selected units
void autoIncrement ( struct sncData *snc );

/************ CODE MAINTAINING DISPLAY **************/

void updateDisplaySegments ( struct sncData *snc ); // sets the display to the current data loaded into timeData;
void clearDisplaySegments ( struct sncData *snc ); // turns off the display

/************** CODE MAINTAINING RTC ****************/

void initialize__RTC ( void );
void saveTimeToRTC ( struct sncData *snc );
void recallTimeFromRTC ( struct sncData *snc );

//////////////////////////////////////////////////////

#endif /* _SOVIET_NIXIE_CLOCK */
