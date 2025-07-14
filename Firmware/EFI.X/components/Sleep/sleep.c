#include "sleep.h"
#include "../LED/led.h"
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
     // Disable all timers
    INTCONbits.TMR0IE = 0;
    T1CONbits.TMR1ON = 0;
    PIE1bits.TMR1IE = 0;
    T2CONbits.TMR2ON = 0;
    PIE1bits.TMR2IE = 0;

    // Optional: Disable ADC & Comparators to save power
    ADCON0bits.ADON = 0;
    CM1CON0bits.C1ON = 0;
    CM2CON0bits.C2ON = 0;

    __delay_ms(200);  // Let voltages settle
    
    BOOST_PWR = 0;
    led_off();
    // Now actually sleep
    SLEEP();
    NOP();  // Safety after wake-up

    // Re-enable Timer1 (software PWM)
    T1CONbits.TMR1ON = 1;
    PIE1bits.TMR1IE = 1;

    // Re-enable Timer2 (if used)
    T2CONbits.TMR2ON = 1;
    PIE1bits.TMR2IE = 1;

    // Re-enable ADC (if needed)
    ADCON0bits.ADON = 1;
}
