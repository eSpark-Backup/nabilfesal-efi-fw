#include "button.h"
#include <string.h>
#include "../Flag/flag.h"
#include "../Relay/relay.h"
#include "../Declarations/declaration.h"

// reset time definitions in milliseconds?
// #define RESET_TIME_4_HRS    S_TO_MS(10)             //  10secs
// #define RESET_TIME_8_HRS    S_TO_MS(20)             //  20secs
#define RESET_TIME_4_HRS    H_TO_MS(4)             //  4hrs
#define RESET_TIME_8_HRS    H_TO_MS(8)             //  8hrs
#define RESET_TIME_16_HRS   H_TO_MS(16)            // 16hrs
#define RESET_TIME_32_HRS   H_TO_MS(32)            // 32hrs
#define RESPONSE_TIME_2     MS_TO_US(100)                 // 100ms
#define RESPONSE_TIME_3     MS_TO_US(150)                 // 150ms
#define RESPONSE_TIME_4     MS_TO_US(200)                 // 200ms

#define IS_RESET_TIME_4_HRS(config_sw)    (config_sw.sw2 == CLEAR && config_sw.sw3 == CLEAR)
#define IS_RESET_TIME_8_HRS(config_sw)    (config_sw.sw2 == CLEAR && config_sw.sw3 == SET)
#define IS_RESET_TIME_16_HRS(config_sw)   (config_sw.sw2 == SET && config_sw.sw3 == CLEAR)
#define IS_RESET_TIME_32_HRS(config_sw)   (config_sw.sw2 == SET && config_sw.sw3 == SET)

#define READ_CONFIG_SWITCH(pin, tris_bit, sw_member) \
    do { \
        tris_bit = 0; \
        SET_GPIO_HIGH(pin); \
        __delay_ms(1); \
        dip_sw_config_obj.sw_member = CONFIG_SW_COM ? SET : CLEAR; \
        SET_GPIO_LOW(pin); \
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
    
    SET_GPIO_LOW(RC6_OUTPUT);
    SET_GPIO_LOW(RA7_OUTPUT);

    memset(&dip_sw_config_obj, 0, sizeof(dip_sw_config_obj));

#ifdef ENABLE_TEST_POINTS
    TRISD6 = 0;   // Output
    TRISD7 = 0;   // Output

    SET_GPIO_LOW(TEST_POINT_1);
    SET_GPIO_LOW(TEST_POINT_2);
#endif

}

void reset_timer0(void)
{
    INTCONbits.GIE = CLEAR;

    timer0_us = 0;
    TMR0 = 0;
    TMR0IF = CLEAR;
    TMR0IE = SET;

    INTCONbits.GIE = SET;
}

unsigned long get_time_us(void) 
{
    unsigned long t1, t2;
    uint8_t tmr;

    do {
        INTCONbits.GIE = CLEAR;   // disable interrupts
        t1 = timer0_us;
        tmr = TMR0;
        t2 = timer0_us;
        INTCONbits.GIE = SET;
    } while (t1 != t2);

    return (t1 + (unsigned long)(tmr * TMR0_TICK_US));  // full microseconds
}

void inrush_current()
{
    if (dip_sw_config_obj.sw4)
    {
        SET_GPIO_HIGH(RA7_OUTPUT);
        __delay_ms(1);
        if(!VAC_240)
        {
            SET_GPIO_HIGH(RC6_OUTPUT);
            __delay_ms(1);
            if(!AUTO_RST && CT_WAKEUP)
            {
                unsigned long current_time = get_time_us();
                while ((get_time_us() - current_time) < S_TO_US(3))
                {
                    NOP();
                }
            }
        }
        
        if(!AUTO_RST && !CT_WAKEUP)
        {
            update_state_machine_state(SLEEP);
        }

        SET_GPIO_LOW(RA7_OUTPUT);
        __delay_ms(1);
    }
}

void dc_rst()
{
    SET_GPIO_HIGH(RC6_OUTPUT);
    __delay_us(1);
    if(!VDC_110)
    {
        update_state_machine_state(FLAG_RESET);
    }
    SET_GPIO_LOW(RC6_OUTPUT);
    __delay_us(1);
}

void vac_rst()
{
    SET_GPIO_HIGH(RA7_OUTPUT);
    __delay_us(1);
    SET_GPIO_HIGH(RC6_OUTPUT);
    __delay_us(1);
    if(!VAC_240 && !AUTO_RST)
    {
        --rst_count;
        if(!rst_count)
        {
            update_state_machine_state(FLAG_RESET);
        }
    }
    SET_GPIO_LOW(RA7_OUTPUT);
    __delay_us(1);
    SET_GPIO_LOW(RC6_OUTPUT);
    __delay_us(1);
}

void dp3_remote_contact()
{
    if(dip_sw_config_obj.sw1)
    {
        unsigned long current_time = get_time_us();
        while ((get_time_us() - current_time) < MS_TO_US(100))
        {
            NOP();
        }
        relay_trig(CLEAR);
    }
}

void dp3_rst_time()
{
    static unsigned long reference_time = RESET_TIME_4_HRS;

    if (!one_time_rst)
    {
        ++one_time_rst;
        current_time_rst = US_TO_MS(get_time_us());

        if(IS_RESET_TIME_4_HRS(dip_sw_config_obj))
        {
            reference_time = RESET_TIME_4_HRS;
        }
        else if(IS_RESET_TIME_8_HRS(dip_sw_config_obj))
        {
            reference_time = RESET_TIME_8_HRS;
        }
        else if(IS_RESET_TIME_16_HRS(dip_sw_config_obj))
        {
            reference_time = RESET_TIME_16_HRS;
        }
        else if(IS_RESET_TIME_32_HRS(dip_sw_config_obj))
        {
            reference_time = RESET_TIME_32_HRS;
        }
    }

    if((US_TO_MS(get_time_us()) - current_time_rst) >= reference_time)
    {
        update_state_machine_state(FLAG_RESET);
    }
}


// also change this
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
    update_state_machine_state(STATE_TEST_RESET);
}

void handle_ext_wakeup()
{
    update_state_machine_state(STATE_EXT_WAKEUP);
}

void handle_ct_wakeup()
{
    update_state_machine_state(STATE_CT_WAKEUP);
}

void __interrupt() isr()
{
    // -------- TIMER0: INTERRUPT --------
    if (INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = CLEAR;
        timer0_us += TMR0_OVERFLOW_US;
        TOGGLE_GPIO(TEST_POINT_1);
    }

    // -------- INT0: TEST_RST (RB0) --------
    if (INTCONbits.INTF) {
        INTCONbits.INTF = CLEAR;
        if (PORTBbits.RB0 == 0) {
            handle_test_rst();
        }
    }

    // -------- IOC: EXT_WAKEUP (RB3) & CT_WAKEUP (RB4) --------
    if (INTCONbits.RBIF) {
        INTCONbits.RBIF = CLEAR;
        if (PORTBbits.RB3 == 0) {
            handle_ext_wakeup();
        }
        if (PORTBbits.RB4 == 1) {
            handle_ct_wakeup();
        }
    }
}
