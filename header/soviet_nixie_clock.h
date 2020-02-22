#ifndef _SOVIET_NIXIE_CLOCK_
#define _SOVIET_NIXIE_CLOCK_

// enumerations
enum UnitsOfTime { HOUR, MINUTE, SECOND, ALL };
enum TimeAdjustments { INCREMENT, DECREMENT, CLEAR };

/************ CODE MAINTAINING TIME **************/
// instance providing all the data required to operate 
struct sncData {
    
    // note:: timeData is not unsigned for increment and decrement operations
    short timeData; // stores time in seconds
    unsigned char hourDisplay, minuteDisplay, secondDisplay; // values to display on 8-bit PORTS

};

/************ CODE MAINTAINING TIME *****************/

void setTime ( struct sncData *snc, enum TimeAdjustments adjustment ); // this function sets the time to a specifiable value

/************ CODE MAINTAINING DISPLAY **************/

void updateDisplaySegments ( struct sncData *snc ); // sets the display to the current data loaded into timeData;
void clearDisplaySegment ( struct sncData *snc, enum UnitsOfTime segment ); // turns off the display

/************** CODE MAINTAINING RTC ****************/

void saveTimeToRTC ( struct sncData *snc );
void recallTimeFromRTC ( struct sncData *snc );

//////////////////////////////////////////////////////

#endif /* _SOVIET_NIXIE_CLOCK */
