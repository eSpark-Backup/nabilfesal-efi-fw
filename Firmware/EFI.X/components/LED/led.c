#include "../Declarations/declaration.h"

#define TIMER1_DELAY_S     100

void led_init()
{
    TRISA3 = 0;   // Battery LED output
    TRISA4 = 0;   // Status LED output
    
    T1CONbits.TMR1CS = 0b01;    // External 32.768kHz crystal
    T1CONbits.T1OSCEN = 1;      // Enable T1 oscillator
    T1CONbits.T1CKPS = 0b11;    // Prescaler 1:8
    T1CONbits.TMR1ON = 1;       // Turn on Timer1

    BATT_LED = 0;
    STAT_LED = 0;
}

void timer1_delay_ms(unsigned int delay_ms) {
    while (delay_ms--)
    {
        // Each ms = ~4.096 ticks ? use 4 ticks = ~976 µs (close enough to 1ms)
        // 65536 - 4 = 65532 = 0xFFFC
        TMR1H = 0xFF;
        TMR1L = 0xFC;
        PIR1bits.TMR1IF = 0;
        while (!PIR1bits.TMR1IF);  // Wait for overflow
    }
}

void blink_battery_led(unsigned char time)
{
    for(int i=1; i<=time; i++)
    {
        BATT_LED = 1;
        // __delay_ms(100);
        timer1_delay_ms(TIMER1_DELAY_S);
        BATT_LED = 0;
        // __delay_ms(100);
        timer1_delay_ms(TIMER1_DELAY_S);
    }
}

void blink_status_led(unsigned char time)
{
    for(int i=1; i<=time; i++)
    {
        STAT_LED = 1;
        // __delay_ms(100);
        timer1_delay_ms(TIMER1_DELAY_S);
        STAT_LED = 0;
        // __delay_ms(100);
        timer1_delay_ms(TIMER1_DELAY_S);
    }
}

void led_off()
{
    BATT_LED = 0;
    STAT_LED = 0;
}