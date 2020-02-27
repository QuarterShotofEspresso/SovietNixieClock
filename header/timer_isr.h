#ifndef _TIMER_ISR_H_
#define _TIMER_ISR_H_

#include <avr/io.h>
#include <avr/interrupt.h>


// global variables
extern unsigned long _avr_timer_M;
extern unsigned long _avr_timer_cntcurr;

// timer interrupt functions
void TimerSet ( unsigned long M );
void TimerOn ( void );
void TimerOff ( void );

#endif      /* _TIMER_ISR_H_ */
