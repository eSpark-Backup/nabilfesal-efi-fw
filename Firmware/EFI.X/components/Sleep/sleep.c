#include "sleep.h"
#include "../LED/led.h"
#include "../Declarations/declaration.h"

void sleep_init() {
    INTCONbits.PEIE = 1; // Enable peripheral interrupts
    INTCONbits.GIE = 1; // Enable global interrupts

    // INT0 config (RB0)
    INTCONbits.INTE = 1; // Enable external interrupt on RB0
    INTCONbits.INTF = 0; // Clear interrupt flag
    OPTION_REGbits.INTEDG = 0; // Falling edge (1 = rising)

    // IOC config
    IOCBbits.IOCB3 = 1;
    IOCBbits.IOCB4 = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;

    BOOST_PWR = 0;
}

void sleep_func() {
    T2CON = 0x00;
    TRISD = 0x00;
    CONFIG_SW_COM = CLEAR;
    BOOST_PWR = 0;
    SLEEP();
    NOP();
}