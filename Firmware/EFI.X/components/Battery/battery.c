#include <stdbool.h>
#include "battery.h"
#include "../LED/led.h"
#include "../Declarations/declaration.h"

#define BAT_OK      true
#define BAT_LOW     false

static bool batteryLow = 0;

void battery_init()
{
    TRISB5 = 1;   // Battery sense as input
    TRISA5 = 0;   // Battery Load
    
    BATT_LOAD = 0;
}

void delay_5ms_with_timer0()
{
    // Assuming 4MHz -> Instruction cycle = 1�s (Fosc/4)
    // Prescaler 1:64 -> 1 tick = 64�s
    // For 5ms ? 5000�s / 64�s ? 78 counts (255 - 78 = 177 preload)

    OPTION_REGbits.T0CS = 0;   // Timer0 uses internal instruction cycle clock (Fosc/4)
    OPTION_REGbits.PSA = 0;    // Prescaler is assigned to Timer0
    OPTION_REGbits.PS = 0b101; // 1:64 prescaler

    TMR0 = 177;                // Preload for ~5ms
    INTCONbits.TMR0IF = 0;     // Clear overflow flag

    while (!INTCONbits.TMR0IF)
    {
        if(!BATT_LOW_SNS)
        {
            batteryLow =1;
            break;
        }
    }
    INTCONbits.TMR0IF = 0;     // Clear flag just in case
}

void battery_test()
{   
    BATT_LOAD = 1;
    delay_5ms_with_timer0();
    BATT_LOAD = 0;
    if(batteryLow)
    {
        SystemState_obj.current_state = BATTERY_LOW;
        batteryLow = 0;
    }
    else
    {
        blink_battery_led(1);
        SystemState_obj.current_state = SLEEP;
    }
}