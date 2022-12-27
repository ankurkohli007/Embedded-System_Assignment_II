
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef TIMER_H
#define	TIMER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#define TIMER1 
#define TIMER2 

int tmr_setup_period(int timer, int ms);
int tmr_wait_period(int timer);
int tmr_wait_ms(int timer, int ms);

#endif	/* TIMER_H */

