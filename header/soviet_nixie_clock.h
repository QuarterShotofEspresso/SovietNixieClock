#ifndef _SOVIET_NIXIE_CLOCK_
#define _SOVIET_NIXIE_CLOCK_


/************ CODE MAINTAINING TIME **************/

struct timeData {
    unsigned char hour, minute, second;
    unsigned char timeUnitIndex;
    unsigned char timeData[3] = {hour, minute, second};
    
    // function selecting which unit of time to adjust
    void ( * shiftTimeUnit ) ( unsigned char * timeIndex );
    
}


void shiftTimeUnit ( unsigned char * timeIndex ) {
    *timeUnitIndex = ( *timeUnitIndex + 1 ) % 3;
}

///////////////////////////////////////////////////







#endif /* _SOVIET_NIXIE_CLOCK */
