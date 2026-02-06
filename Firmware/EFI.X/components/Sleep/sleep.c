#include "sleep.h"
#include "../Button/button.h"
#include "../LED/led.h"
#include "../Declarations/declaration.h"

void sleep_init() {
    INTCONbits.PEIE = SET; // Enable peripheral interrupts
    INTCONbits.GIE = SET; // Enable global interrupts

    // INT0 config (RB0)
    INTCONbits.INTE = SET; // Enable external interrupt on RB0
    INTCONbits.INTF = CLEAR; // Clear interrupt flag
    OPTION_REGbits.INTEDG = 0; // Falling edge (1 = rising)

    // IOC config
    IOCBbits.IOCB3 = SET;
    IOCBbits.IOCB4 = SET;
    INTCONbits.RBIE = SET;
    INTCONbits.RBIF = CLEAR;

    SET_GPIO_LOW(BOOST_PWR);
}

void sleep_func() {
    T2CON = 0x00;
    TRISD = 0x00;
    SET_GPIO_LOW(CONFIG_SW_COM);
    SET_GPIO_LOW(BOOST_PWR);
    SLEEP();
    NOP();   // required after SLEEP


    // ---- MCU has just woken up ----
    reset_timer0();
}