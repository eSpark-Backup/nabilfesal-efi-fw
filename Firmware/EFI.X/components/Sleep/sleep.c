#include <xc.h>
#include "sleep.h"

void sleep_init()
{
    TRISB3 = 1;             // External interrupt pin
    OPTION_REGbits.nRBPU = 0;  // Enable pull-ups
    IOCBbits.IOCB3 = 1;     // Enable IOC on RB3
    INTCONbits.RBIF = 0;    // Clear interrupt-on-change flag
    INTCONbits.RBIE = 1;    // Enable Port B interrupts
    INTCONbits.GIE = 1;     // Global Interrupt Enable
}
            
void sleep_func()
{
    INTCONbits.RBIF = 0;  // Clear the flag
    SLEEP();              // Sleep until IOC occurs
    NOP();                // Required after wakeup on some chips
}