#include "button.h"
#include "../Flag/flag.h"

void button_init()
{
    TRISC5 = 1;   // Battery Switch input
    TRISB0 = 1;   // Test Switch input
    TRISB3 = 1;   // External Reset input
    TRISB4 = 1;   // CT Wakeup
}

void handle_pwm_interupt()
{
    if (pwm_state == 0)
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

void handle_test_rst()
{
    SystemState_obj.current_state = STATE_TEST_RESET;
}

void handle_ext_wakeup()
{
    SystemState_obj.current_state = FAULT;
}

void handle_ct_wakeup()
{
    SystemState_obj.current_state = STATE_CT_WAKEUP;
}

void __interrupt() isr()
{
    // -------- TIMER1: Software PWM --------
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;
        handle_pwm_interupt();
    }

    // -------- INT0: TEST_RST (RB0) --------
    if (INTCONbits.INTF) {
        INTCONbits.INTF = 0;
        handle_test_rst();
    }

    // -------- IOC: EXT_WAKEUP (RB3) & CT_WAKEUP (RB4) --------
    if (INTCONbits.RBIF) {
        INTCONbits.RBIF = 0;

        if (PORTBbits.RB3 == 0) {
            handle_ext_wakeup();
        }
        
        if (PORTBbits.RB4 == 0) {
            handle_ct_wakeup();
        }
    }
}