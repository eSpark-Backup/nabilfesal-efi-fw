#include "button.h"
#include <string.h>
#include "../Flag/flag.h"
#include "../Relay/relay.h"
#include "../Declarations/declaration.h"

#define RESET_TIME_1     4*3600*1000    //  4hrs
#define RESET_TIME_2     8*3600*1000    //  8hrs
#define RESET_TIME_3    16*3600*1000    // 16hrs
#define RESET_TIME_4    32*3600*1000    // 32hrs

#define RESPONSE_TIME_1     50          //  50ms
#define RESPONSE_TIME_2    100          // 100ms
#define RESPONSE_TIME_3    150          // 150ms
#define RESPONSE_TIME_4    200          // 200ms

#define READ_CONFIG_SWITCH(pin, tris_bit, sw_member) \
    do { \
        tris_bit = 0; \
        pin = 1; \
        __delay_ms(1); \
        dip_sw_config_obj.sw_member = CONFIG_SW_COM ? SET : CLEAR; \
        pin = 0; \
        __delay_ms(1); \
        tris_bit = 1; \
    } while(0)

unsigned long current_time_rst = 0;
dip_sw_config_t dip_sw_config_obj;

void button_init()
{
    TRISC5 = 1;   // Battery Switch input
    TRISB0 = 1;   // Test Switch input
    TRISB3 = 1;   // External Reset input
    TRISB4 = 1;   // CT Wakeup
    TRISA2 = 1;   // VDC_110
    TRISA6 = 1;   // VAC_240
    TRISA7 = 1;   // Auto Reset
    
    TRISA7 = 0;   // RA7 Output
    TRISC6 = 0;   // RC6 Output

    /*DIP Switch Configuration*/
    TRISC4 = 1;
    TRISD0 = 1;
    TRISD1 = 1;
    TRISD2 = 1;
    TRISD3 = 1;
    TRISC2 = 1;
    TRISC3 = 1;

    RC6_OUTPUT = CLEAR;
    RA7_OUTPUT = CLEAR;

    memset(&dip_sw_config_obj, 0, sizeof(dip_sw_config_obj));
}

unsigned long get_time_ms()
{
    unsigned long t;
    INTCONbits.GIE = 0;
    t = timer0_ms;
    INTCONbits.GIE = 1;
    return t;
}

void dc_rst()
{
    RC6_OUTPUT = SET;
    __delay_ms(1);
    if(!VDC_110)
    {
        SystemState_obj.current_state = FLAG_RESET;
    }
    RC6_OUTPUT = CLEAR;
    __delay_ms(1);
}

void vac_rst()
{
    RA7_OUTPUT = SET;
    __delay_ms(1);
    if(!VAC_240)
    {
        SystemState_obj.current_state = FLAG_RESET;
    }
    RA7_OUTPUT = CLEAR;
    __delay_ms(1);
}

void dp3_remote_contact()
{
    if(dip_sw_config_obj.sw1)
    {
        unsigned long current_time = get_time_ms();
        while (get_time_ms() - current_time < 100)
        {
            NOP();
        }
        relay_trig(CLEAR);
    }
}

void dp3_rst_time()
{
    if (!one_time_rst)
    {
        current_time_rst = get_time_ms();
        ++one_time_rst;
    }
    
    if(!dip_sw_config_obj.sw2 && !dip_sw_config_obj.sw3)
    {
        if(get_time_ms() - current_time_rst >= RESET_TIME_1)
        {
            SystemState_obj.current_state = FLAG_RESET;
        }
    }
    else if(!dip_sw_config_obj.sw2 && dip_sw_config_obj.sw3)
    {
        if(get_time_ms() - current_time_rst >= RESET_TIME_2)
        {
            SystemState_obj.current_state = FLAG_RESET;
        }
    }
    else if(dip_sw_config_obj.sw2 && !dip_sw_config_obj.sw3)
    {
        if(get_time_ms() - current_time_rst >= RESET_TIME_3)
        {
            SystemState_obj.current_state = FLAG_RESET;
        }
    }
    else if(dip_sw_config_obj.sw2 && dip_sw_config_obj.sw3)
    {
        if(get_time_ms() - current_time_rst >= RESET_TIME_4)
        {
            SystemState_obj.current_state = FLAG_RESET;
        }
    }
}

void dp3_response_delay()
{
    if (!one_time_rsp)
    {
        current_time_rst = get_time_ms();
        ++one_time_rsp;
    }

    if(!dip_sw_config_obj.sw5 && !dip_sw_config_obj.sw6)
    {
        while(get_time_ms() - current_time_rst < RESPONSE_TIME_1)
        {
            NOP();
        }
    }
    else if(!dip_sw_config_obj.sw5 && dip_sw_config_obj.sw6)
    {
        while(get_time_ms() - current_time_rst < RESPONSE_TIME_2)
        {
            NOP();
        }
    }
    else if(dip_sw_config_obj.sw5 && !dip_sw_config_obj.sw6)
    {
        while(get_time_ms() - current_time_rst < RESPONSE_TIME_3)
        {
            NOP();
        }
    }
    else if(dip_sw_config_obj.sw5 && dip_sw_config_obj.sw6)
    {
        while(get_time_ms() - current_time_rst < RESPONSE_TIME_4)
        {
            NOP();
        }
    }
}

void dip3_config()
{
    READ_CONFIG_SWITCH(RMT_CNT_SW, TRISD3, sw1);
    READ_CONFIG_SWITCH(RST_TIME_1, TRISD2, sw2);
    READ_CONFIG_SWITCH(RST_TIME_2, TRISD1, sw3);
    READ_CONFIG_SWITCH(IN_RSH_CRNT, TRISD0, sw4);
    READ_CONFIG_SWITCH(RSP_DELAY_1, TRISC3, sw5);
    READ_CONFIG_SWITCH(RSP_DELAY_2, TRISC2, sw6);
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
