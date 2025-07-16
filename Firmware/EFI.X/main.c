#include "components/LED/led.h"
#include "components/Flag/flag.h"
#include "components/Relay/relay.h"
#include "components/Sleep/sleep.h"
#include "components/Button/button.h"
#include "components/Battery/battery.h"
#include "components/Flag/flag_driver.h"
#include "components/Declarations/declaration.h"

// CONFIG BITS
#pragma config FOSC = INTRC_NOCLKOUT
#pragma config WDTE = OFF, PWRTE = OFF, MCLRE = OFF
#pragma config CP = OFF, CPD = OFF, BOREN = OFF, IESO = OFF
#pragma config FCMEN = OFF, LVP = OFF
#pragma config BOR4V = BOR40V
#pragma config WRT = OFF

volatile StateMachine_t SystemState_obj = {
    .current_state = LED_TEST
};

void main(void)
{
    ANSEL = 0;
    ANSELH = 0b00000001;
    
    led_init();
    flag_init();
    relay_init();
    sleep_init();
    button_init();
    battery_init();
    flagdriv_init();

    PORTA = 0; 
    PORTD = 0;

    while (1)
    {
       switch (SystemState_obj.current_state)
       {
            case STARTUP_CYCLE:
                __delay_ms(500);
                battery_test();
                // boost_en = 1;
                // control_boost_output();
                // boost_en = 0;
                flag_trig();
                break;
                
            case STATE_TEST_RESET:
            case STATE_EXT_WAKEUP:
                SystemState_obj.current_state = LED_TEST;
                // battery_check();
                // boost_en = 1;
                // control_boost_output();
                // boost_en = 0;
                // SystemState_obj.current_state = FAULT_TOGGLE;
                break;
                
            case STATE_CT_WAKEUP:
                battery_check();
                // boost_en = 1;
                // control_boost_output();
                // boost_en = 0;
                SystemState_obj.current_state = FLAG_SET;
                break;
                
            case FLAG_SET:
                relay_trig(SET);
                flag_trig();
                break;
                
            case FLAG_RESET:
                relay_trig(CLEAR);
                flag_trig();
                break;
            
            case FAULT:
                blink_status_led(5);
                break;
                
            case FAULT_TOGGLE:
                flag_trig();
                break;
                
            case BATTERY_LOW:
                blink_battery_led(5);
                break;
            
            case LED_TEST:
                blink_battery_led(5);
                blink_status_led(5);
                SystemState_obj.current_state = SLEEP;
                break;
            
            case SLEEP:
                sleep_func();
                break;

            default:
                break;
        }
    }
}