#include <stdbool.h>
#include "battery.h"
#include "../LED/led.h"
#include "../Button/button.h"
#include "../Relay/relay.h"
#include "../Flag/flag.h"
#include "../Declarations/declaration.h"

#define EEPROM_RST_VAL           1080
#define EEPROM_ADDR_LSB          0x00
#define EEPROM_ADDR_MSB          0x01

#define IND_MODE_TIME           10*1000*1000
#define BAT_TEST_HOLD_TIME      3*1000*1000
#define COUNTER_RST_HOLD_TIME   5*1000*1000

static bool batteryLow = 0;
static uint16_t event_count = 0;

void battery_init()
{
    TRISB5 = 1;   // Battery sense as input
    TRISA5 = 0;   // Battery Load
    
    BATT_LOAD = CLEAR;
    OPTION_REGbits.T0CS = 0;   // Internal instruction cycle
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b000; // Prescaler 1:64
    
    TMR0 = 240;                // Preload for ~1ms overflow
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    INTCONbits.GIE = 1;
}

void event_count_inc()
{
    if (event_count_read() >= EEPROM_RST_VAL)
    {
        SystemState_obj.current_state = BATTERY_LOW;
    }

    ++event_count;
    eeprom_write(EEPROM_ADDR_LSB, (uint8_t)(event_count & 0xFF));
    eeprom_write(EEPROM_ADDR_MSB, (uint8_t)((event_count >> 8) & 0xFF));
}

uint16_t event_count_read()
{
    uint8_t lsb = eeprom_read(EEPROM_ADDR_LSB);
    uint8_t msb = eeprom_read(EEPROM_ADDR_MSB);
    return ((uint16_t)msb << 8) | lsb;
}

bool event_counter_reset(unsigned long current_time)
{
    bool press_detected = false;
    unsigned long press_start = 0;

    if (get_time_us() - current_time < IND_MODE_TIME)
    {
        while (BATT_SW == 0) // Button pressed
        {
            if (!press_detected)
            {
                press_start = get_time_us();
                press_detected = true;
            }

            if (get_time_us() - press_start > COUNTER_RST_HOLD_TIME)
            {
                event_count = 0;
                eeprom_write(EEPROM_ADDR_LSB, (uint8_t)(event_count & 0xFF));
                eeprom_write(EEPROM_ADDR_MSB, (uint8_t)((event_count >> 8) & 0xFF));
                relay_trig(CLEAR);
                flag_reset();
                BATT_LED = SET;
                timer1_delay_ms(3000);
                BATT_LED = CLEAR;
                break;
            }
            else if (get_time_us() - press_start >= BAT_TEST_HOLD_TIME && get_time_us() - press_start < COUNTER_RST_HOLD_TIME)
            {
                battery_test();
            }
        }
        press_detected = false;
        return SET;
    }
    else
    {
        return CLEAR;
    }
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
    unsigned long start = get_time_us();
    while (get_time_us() - start < 5*1000)
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