#include "../Declarations/declaration.h"

#define TIMER1_DELAY_MS     100

void led_init()
{
    TRISA3 = 0;   // Battery LED output
    TRISA4 = 0;   // Status LED output
    
    T1CONbits.TMR1CS = 0b01;    // Clock from T1 oscillator
    T1CONbits.T1OSCEN = 1;      // Enable external oscillator (32.768kHz crystal typically)
    T1CONbits.T1CKPS = 0b11;    // Prescaler 1:8
    T1CONbits.TMR1ON = 1;       // Turn on Timer1

    BATT_LED = 0;
    STAT_LED = 0;
}

void timer1_delay_ms(unsigned int delay_ms) {
    while (delay_ms--) {
        TMR1H = 0x3C;    // Preload for ~1ms @ 32.768kHz with 1:8 prescaler
        TMR1L = 0xB0;
        PIR1bits.TMR1IF = 0;
        while (!PIR1bits.TMR1IF);
    }
}

void blink_battery_led(unsigned char time)
{
    for(int i=1; i<=time; i++)
    {
        BATT_LED = 1;
        timer1_delay_ms(TIMER1_DELAY_MS);
        BATT_LED = 0;
        timer1_delay_ms(TIMER1_DELAY_MS);
    }
}

void blink_status_led(unsigned char time)
{
    for(int i=1; i<=time; i++)
    {
        STAT_LED = 1;
        timer1_delay_ms(TIMER1_DELAY_MS);
        STAT_LED = 0;
        timer1_delay_ms(TIMER1_DELAY_MS);
    }
}

void led_off()
{
    BATT_LED = 0;
    STAT_LED = 0;
}