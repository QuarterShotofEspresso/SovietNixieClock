#ifndef _SOVIET_NIXIE_CLOCK_
#define _SOVIET_NIXIE_CLOCK_

// includes
#include "i2c.h"

// enumerations
enum UnitsOfTime { HOUR, MINUTE, SECOND, ALL };
enum timeAdjustments { INCREMENT, DECREMENT, CLEAR };

/************ CODE MAINTAINING TIME **************/

struct sncData {
    // unitsOfTime
    enum UnitsOfTime unitOfTime; // listing all accesible indicies
    unsigned short timeData; // stores the current time
    unsigned char hourDisplay, minuteDisplay, secondDisplay; // values to display on 8-bit PORTS

};


void selectUnitOfTime ( struct sncData *snc, enum UnitsOfTime unit );
void setTime ( struct sncData *snc, enum timeAdjustments adjustment ); // this function sets the time to a specifiable value

/************ CODE MAINTAINING DISPLAY **************/

void updateDisplaySegments ( struct sncData *snc ); // sets the display to the current data loaded into timeData;
void clearDisplaySegment ( struct sncData *snc, enum UnitsOfTime unit ); // turns off the display

/************** CODE MAINTAINING RTC ****************/

void saveTimeToRTC ( struct sncData *snc );
void recallTimeFromRTC ( struct sncData *snc );

//////////////////////////////////////////////////////


#endif /* _SOVIET_NIXIE_CLOCK */
