#include "../Declarations/declaration.h"

#define TIMER1_DELAY_ON_MS     10
#define TIMER1_DELAY_OFF_MS   990

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

void timer1_delay_ms(unsigned int delay_ms)
{
    while (delay_ms--)
    {
        // Each ms = ~4.096 ticks ? use 4 ticks = ~976 �s (close enough to 1ms)
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
        timer1_delay_ms(TIMER1_DELAY_ON_MS);
        BATT_LED = 0;
        timer1_delay_ms(TIMER1_DELAY_OFF_MS);
    }
}

void blink_status_led(unsigned char time)
{
    if (time == 0)
        return;

    uint16_t totalTime = TIMER1_DELAY_ON_MS + TIMER1_DELAY_OFF_MS;
    uint16_t off_gap  = (totalTime / (time*2)) - TIMER1_DELAY_ON_MS;
    uint16_t final_off = (TIMER1_DELAY_ON_MS * time) + (off_gap * (time + 1));

    for(uint8_t i=1; i<=time; i++)
    {
        STAT_LED = 1;
        if(!BATT_LOW_SNS)
        {
            BATT_LED = 1;
        }
        timer1_delay_ms(TIMER1_DELAY_ON_MS);
        STAT_LED = 0;
        BATT_LED = 0;
        if (i == time)
        {
            // Extended OFF time at the end
            timer1_delay_ms(final_off);
        }
        else
        {
            timer1_delay_ms(off_gap);
        }
    }
}