#include "button.h"
#include <string.h>
#include "../Flag/flag.h"
#include "../Relay/relay.h"
#include "../Declarations/declaration.h"

#define RESET_TIME_1    ( 4UL*3600UL*1000UL*1000UL)/2    //  4hrs
#define RESET_TIME_2    ( 8UL*3600UL*1000UL*1000UL)/2    //  8hrs
#define RESET_TIME_3    (16UL*3600UL*1000UL*1000UL)/2    // 16hrs
#define RESET_TIME_4    (32UL*3600UL*1000UL*1000UL)/2    // 32hrs

#define RESPONSE_TIME_1     (50UL*1000UL)/2              //  50ms
#define RESPONSE_TIME_2    (100UL*1000UL)/2              // 100ms
#define RESPONSE_TIME_3    (150UL*1000UL)/2              // 150ms
#define RESPONSE_TIME_4    (200UL*1000UL)/2              // 200ms

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

static uint8_t rst_count = 3;
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

unsigned long get_time_us(void) 
{
    unsigned long t1, t2;
    uint8_t tmr;

    do {
        INTCONbits.GIE = 0;   // disable interrupts
        t1 = timer0_us;
        tmr = TMR0;
        t2 = timer0_us;
        INTCONbits.GIE = 1;
    } while (t1 != t2);

    return (t1 + (unsigned long)tmr);  // full microseconds
}

void inrush_current()
{
    if (dip_sw_config_obj.sw4)
    {
        RA7_OUTPUT = SET;
        __delay_ms(1);
        if(!VAC_240)
        {
            RC6_OUTPUT = SET;
            __delay_ms(1);
            if(!AUTO_RST && CT_WAKEUP)
            {
                unsigned long current_time = get_time_us();
                while (get_time_us() - current_time < ((3UL*1000UL*1000UL)/2))
                {
                    NOP();
                }
            }
        }
        
        if(!AUTO_RST && !CT_WAKEUP)
        {
            SystemState_obj.current_state = SLEEP;
        }
        
        RA7_OUTPUT = CLEAR;
        __delay_ms(1);
    }
}

void dc_rst()
{
    RC6_OUTPUT = SET;
    __delay_us(1);
    if(!VDC_110)
    {
        SystemState_obj.current_state = FLAG_RESET;
    }
    RC6_OUTPUT = CLEAR;
    __delay_us(1);
}

void vac_rst()
{
    RA7_OUTPUT = SET;
    __delay_us(1);
    RC6_OUTPUT = SET;
    __delay_us(1);
    if(!VAC_240 && !AUTO_RST)
    {
        --rst_count;
        if(!rst_count)
        {
            SystemState_obj.current_state = FLAG_RESET;
        }
    }
    RA7_OUTPUT = CLEAR;
    __delay_us(1);
    RC6_OUTPUT = CLEAR;
    __delay_us(1);
}

void dp3_remote_contact()
{
    if(dip_sw_config_obj.sw1)
    {
        unsigned long current_time = get_time_us();
        while (get_time_us() - current_time < ((100UL*1000UL)/2))
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
        current_time_rst = get_time_us();
        ++one_time_rst;
    }
    
    if(!dip_sw_config_obj.sw2 && !dip_sw_config_obj.sw3)
    {
        if(get_time_us() - current_time_rst >= RESET_TIME_1)
        {
            SystemState_obj.current_state = FLAG_RESET;
        }
    }
    else if(!dip_sw_config_obj.sw2 && dip_sw_config_obj.sw3)
    {
        if(get_time_us() - current_time_rst >= RESET_TIME_2)
        {
            SystemState_obj.current_state = FLAG_RESET;
        }
    }
    else if(dip_sw_config_obj.sw2 && !dip_sw_config_obj.sw3)
    {
        if(get_time_us() - current_time_rst >= RESET_TIME_3)
        {
            SystemState_obj.current_state = FLAG_RESET;
        }
    }
    else if(dip_sw_config_obj.sw2 && dip_sw_config_obj.sw3)
    {
        if(get_time_us() - current_time_rst >= RESET_TIME_4)
        {
            SystemState_obj.current_state = FLAG_RESET;
        }
    }
}

void dp3_response_delay()
{
    if (!one_time_rsp)
    {
        current_time_rst = get_time_us();
        ++one_time_rsp;
    }

    if(!dip_sw_config_obj.sw5 && !dip_sw_config_obj.sw6)
    {
        while(get_time_us() - current_time_rst < RESPONSE_TIME_1)
        {
            NOP();
        }
    }
    else if(!dip_sw_config_obj.sw5 && dip_sw_config_obj.sw6)
    {
        while(get_time_us() - current_time_rst < RESPONSE_TIME_2)
        {
            NOP();
        }
    }
    else if(dip_sw_config_obj.sw5 && !dip_sw_config_obj.sw6)
    {
        while(get_time_us() - current_time_rst < RESPONSE_TIME_3)
        {
            NOP();
        }
    }
    else if(dip_sw_config_obj.sw5 && dip_sw_config_obj.sw6)
    {
        while(get_time_us() - current_time_rst < RESPONSE_TIME_4)
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
    // -------- TIMER1: INTERRUPT --------
    if (INTCONbits.TMR0IF)
    {
        timer0_us += 256;
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
