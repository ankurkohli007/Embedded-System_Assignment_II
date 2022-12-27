#include"timer.h"

void choose_prescaler(int ms, int* tckps, int* pr) {
    // Fcy = 1843200 Hz ?> 1843,2 clock ticks in 1 ms
    long ticks = 1843.2 * ms; // there can be an approximation
    if (ticks <= 65535) { // if ticks is > 65535 it cannot be put in PR1 (only 16 bits )
        *tckps = 0;
        *pr = ticks;
        return;
    }
    ticks = ticks / 8; // prescaler 1:8;
    if (ticks <= 65535) {
        *tckps = 1;
        *pr = ticks;
        return;
    }
    ticks = ticks / 8; // prescaler 1:64;
    if (ticks <= 65535) {
        *tckps = 2;
        *pr = ticks;
        return;
    }
    ticks = ticks / 4; // prescaler 1:256;
    *tckps = 3;
    *pr = ticks;
    return;
}


int tmr_setup_period(int timer, int ms) {
    switch (timer) {
        case 1:
        {
            int pr, tckps;
            choose_prescaler(ms, &pr, &tckps);
            PR1 = pr;
            T1CONbits.TCKPS = tckps;
            T1CONbits.TCS = 0;
            T1CONbits.TGATE = 0;
            TMR1 = 0;
            T1CONbits.TON = 1;

            break;
        }
        case 2:
        {
            int pr, tckps;
            choose_prescaler(ms, &pr, &tckps);
            PR2 = pr;
            T2CONbits.TCKPS = tckps;
            T2CONbits.TCS = 0;
            T2CONbits.TGATE = 0;
            TMR2 = 0;
            T2CONbits.TON = 1;

            break;
        }
    }
    return 0;
}

int tmr_wait_period(int timer) {
    switch (timer) {
        case 1:
        {
            if (IFS0bits.T1IF == 1) {
                IFS0bits.T1IF = 0;
                return 1;
            }

            while (IFS0bits.T1IF == 0);
            IFS0bits.T1IF = 0;
            break;
        }
        case 2:
        {
            if (IFS0bits.T2IF == 1) {
                IFS0bits.T2IF = 0;
                return 1;
            }

            while (IFS0bits.T2IF == 0);
            IFS0bits.T2IF = 0;
            break;
        }
    }
    return 0;
}

int tmr_wait_ms(int timer, int ms) {
    switch (timer) {
        case 1:
        {
            int pr, tckps;
            choose_prescaler(ms, &pr, &tckps);
            PR1 = pr;
            T1CONbits.TCKPS = tckps;
            T1CONbits.TCS = 0;
            T1CONbits.TGATE = 0;
            
            T1CONbits.TON = 0;
            IFS0bits.T1IF = 0;
            TMR1 = 0;
            T1CONbits.TON = 1;
            while (IFS0bits.T1IF == 0);
            IFS0bits.T1IF = 0;
            T1CONbits.TON = 0;
            break;
        }
        case 2:
        {
            int pr, tckps;
            choose_prescaler(ms, &pr, &tckps);
            PR2 = pr;
            T2CONbits.TCKPS = tckps;
            T2CONbits.TCS = 0;
            T2CONbits.TGATE = 0;
            
            T2CONbits.TON = 0;
            IFS0bits.T2IF = 0;
            TMR2 = 0;
            T2CONbits.TON = 1;
            while (IFS0bits.T2IF == 0);
            IFS0bits.T2IF = 0;
            T2CONbits.TON = 0;
            break;
        }
    }
    return 0;
}

