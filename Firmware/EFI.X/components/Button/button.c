#include "button.h"
#include "../Flag/flag.h"
#include "../Declarations/declaration.h"

void button_init()
{
    TRISC5 = 1;   // Battery Switch input
    TRISB0 = 1;   // Test Switch input
    TRISB3 = 1;   // External Reset input
    TRISB4 = 1;   // CT Wakeup

    /*DIP Switch Configuration*/
    TRISC4 = 1;
    TRISD0 = 0;
    TRISD1 = 0;
    TRISD2 = 0;
    TRISD3 = 0;
    TRISC2 = 0;
    TRISC3 = 0;

    RMT_CNT_SW  = CLEAR;
    RST_TIME_1  = CLEAR;
    RST_TIME_2  = CLEAR;
    IN_RSH_CRNT = CLEAR;
    RSP_DELAY_1 = CLEAR;
    RSP_DELAY_2 = CLEAR;
}

void handle_pwm_interupt()
{
    if (pwm_state == 0 && boost_en)
    {
        BOOST_PWR = 1;
        TMR2 = 0;
        PR2 = on_time_ticks / 2;  // Convert ticks back to µs (0.5 µs per tick)
        pwm_state = 1;
    }
    else
    {
        BOOST_PWR = 0;
        TMR2 = 0;
        PR2 = off_time_ticks / 2; // Convert ticks back to µs
        pwm_state = 0;
    }
}

unsigned long get_time_ms()
{
    unsigned long t;
    INTCONbits.GIE = 0;
    t = timer0_ms;
    INTCONbits.GIE = 1;
    return t;
}

void handle_test_rst()
{
    SystemState_obj.current_state = STATE_TEST_RESET;
}

void handle_ext_wakeup()
{
    SystemState_obj.current_state = STATE_EXT_WAKEUP;
}

void handle_ct_wakeup()
{
    SystemState_obj.current_state = STATE_CT_WAKEUP;
}

void __interrupt() isr()
{
    // -------- TIMER2: Software PWM --------
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        handle_pwm_interupt();
    }
    // -------- TIMER1: INTERRUPT --------
    if (INTCONbits.TMR0IF)
    {
        TMR0 = 240;
        timer0_ms++;
        INTCONbits.TMR0IF = 0;
    }
    // -------- INT0: TEST_RST (RB0) --------
    if (INTCONbits.INTF) {
        INTCONbits.INTF = 0;
        if (PORTBbits.RB0 == 0) {
            handle_test_rst();
        }
    }

    // -------- IOC: EXT_WAKEUP (RB3) & CT_WAKEUP (RB4) --------
    if (INTCONbits.RBIF) {
        INTCONbits.RBIF = 0;
        if (PORTBbits.RB3 == 0) {
            handle_ext_wakeup();
        }
        if (PORTBbits.RB4 == 1) {
            handle_ct_wakeup();
        }
    }
}
