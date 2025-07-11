#include "../Battery/battery.h"
#include "../Declarations/declaration.h"

void led_init()
{
    TRISA3 = 0;   // Battery LED output
    TRISA4 = 0;   // Status LED output
    
    BATT_LED = 0;
    STAT_LED = 0;
}

void blink_battery_led(unsigned char time)
{
    for(int i=1; i<=time; i++)
    {
        BATT_LED = 1;
        __delay_ms(100);
        BATT_LED = 0;
        __delay_ms(100);
    }
}

void blink_status_led(unsigned char time)
{
    for(int i=1; i<=time; i++)
    {
        STAT_LED = 1;
        __delay_ms(200);
        STAT_LED = 0;
        __delay_ms(200);
    }
}

void led_off()
{
    BATT_LED = 0;
    STAT_LED = 0;
}