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
    .current_state = STARTUP_CYCLE
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
                dip3_config();
                SystemState_obj.current_state = FLAG_RESET;
                break;
                
            case STATE_TEST_RESET:
            case STATE_EXT_WAKEUP:
                one_time_rst = 0;
                battery_check();
                // boost_en = 1;
                // control_boost_output();
                // boost_en = 0;
                dip3_config();
                SystemState_obj.current_state = FAULT_TOGGLE;
                break;
                
            case STATE_CT_WAKEUP:
                one_time_rst = 0;
                battery_check();
                // boost_en = 1;
                // control_boost_output();
                // boost_en = 0;
                dip3_config();
                SystemState_obj.current_state = FLAG_SET;
                break;
                
            case FLAG_SET:
                one_time_rsp = 0;
                dp3_response_delay();
                relay_trig(SET);
                flag_set();
                break;
                
            case FLAG_RESET:
                relay_trig(CLEAR);
                flag_reset();
                break;
            
            case FAULT:
                blink_status_led(1);
                dp3_remote_contact();
                dp3_rst_time();
                dc_rst();
                break;
                
            case FAULT_TOGGLE:
                flag_toggle();
                break;
                
            case BATTERY_LOW:
                blink_battery_led(1);
                break;
            
            case SLEEP:
                sleep_func();
                break;

            default:
                break;
        }
    }
}