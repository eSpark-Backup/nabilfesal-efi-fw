#include <stdbool.h>
#include "battery.h"
#include "../LED/led.h"
#include "../Button/button.h"
#include "../Declarations/declaration.h"

static bool batteryLow = 0;

void battery_init()
{
    TRISB5 = 1;   // Battery sense as input
    TRISA5 = 0;   // Battery Load
    
    BATT_LOAD = CLEAR;
    OPTION_REGbits.T0CS = 0;   // Internal instruction cycle
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b101; // Prescaler 1:64
    
    TMR0 = 240;                // Preload for ~1ms overflow
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    INTCONbits.GIE = 1;
}

void battery_check()
{
    if(!BATT_LOW_SNS)
    {
        SystemState_obj.current_state = BATTERY_LOW;
    }
}

void battery_test()
{   
    BATT_LOAD = SET;
    unsigned long start = get_time_ms();
    while (get_time_ms() - start < 5)
    {
        if(!BATT_LOW_SNS)
        {
            batteryLow = SET;
            break;
        }
    }
    BATT_LOAD = CLEAR;
    if(batteryLow)
    {
        SystemState_obj.current_state = BATTERY_LOW;
        batteryLow = CLEAR;
    }
    else
    {
        blink_battery_led(1);
    }
}