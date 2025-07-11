#include <xc.h>
#include "sleep.h"
#include "../Declarations/declaration.h"

void sleep_init()
{
    // INT0 config (RB0)
    INTCONbits.INTE = 1;          // Enable external interrupt on RB0
    INTCONbits.INTF = 0;          // Clear interrupt flag
    OPTION_REGbits.INTEDG = 0;    // Falling edge (1 = rising)

    // IOC config
    IOCBbits.IOCB3 = 1;
    IOCBbits.IOCB4 = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
}
            
void sleep_func()
{
    // Turn off Timer1 interrupt to prevent waking
    PIE1bits.TMR1IE = 0;
    T1CONbits.TMR1ON = 0;

    // Set BOOST_PWR pin LOW before sleep
    
    // Optional: Turn off ADC
    // ADCON0bits.ADON = 0;

    __delay_ms(200);
    BOOST_PWR = 0;
    // Now actually sleep
    SLEEP();
    NOP();  // Safety after wake-up
}
