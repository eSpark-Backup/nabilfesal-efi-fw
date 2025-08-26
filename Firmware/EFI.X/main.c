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

#define BAT_TEST_HOLD_TIME (3UL * 1000UL * 1000UL) / 2

static bool test_reset_wakeup = CLEAR;
volatile StateMachine_t SystemState_obj = {
    .current_state = STARTUP_CYCLE};

void main(void)
{
    ANSEL = CLEAR;
    ANSELH = 0b00000001;

    adc_init();
    led_init();
    flag_init();
    relay_init();
    sleep_init();
    button_init();
    battery_init();
    flagdriv_init();

    unsigned long current_time = 0;

    PORTA = CLEAR;
    PORTD = CLEAR;

    while (1)
    {
        switch (SystemState_obj.current_state)
        {
        case STARTUP_CYCLE:
            __delay_ms(500);
            dip3_config();
            boost_en = 1;
            control_boost_output();
            boost_en = 0;
            SystemState_obj.current_state = FLAG_RESET;
            break;

        case STATE_TEST_RESET:
            TMR0 = 0;  
            TMR0IF = 0;
            TMR0IE = 1;
            test_reset_wakeup = SET;
            current_time = get_time_us();
            while (!TEST_RST)
            {
                if ((get_time_us() - current_time) > BAT_TEST_HOLD_TIME)
                {
                    battery_test();
                    break;
                }
            }

        case STATE_EXT_WAKEUP:
            one_time_rst = 0;
            dip3_config();
            boost_en = 1;
            control_boost_output();
            boost_en = 0;
            SystemState_obj.current_state = FAULT_TOGGLE;
            break;

        case STATE_CT_WAKEUP:
            one_time_rst = 0;
            dip3_config();
            boost_en = 1;
            control_boost_output();
            boost_en = 0;
            SystemState_obj.current_state = FLAG_SET;
            inrush_current();
            break;

        case FLAG_SET:
            one_time_rsp = 0;
            dp3_response_delay();
            relay_trig(SET);
            flag_set();
            event_count_inc();
            break;

        case FLAG_RESET:
            relay_trig(CLEAR);
            flag_reset();
            event_count_inc();
            break;

        case FAULT:
            blink_status_led(1);
            dp3_remote_contact();
            dp3_rst_time();
            dc_rst();
            vac_rst();
            if (test_reset_wakeup)
            {
                test_reset_wakeup = event_counter_reset(current_time);
            }
            break;

        case FAULT_TOGGLE:
            flag_toggle();
            break;

        case BATTERY_LOW:
            blink_battery_led(5);
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