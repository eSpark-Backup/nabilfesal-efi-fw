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

#define BAT_TEST_HOLD_TIME S_TO_US(3)

static bool test_reset_wakeup = CLEAR;
static bool ext_wakeup = CLEAR;
volatile StateMachine_t SystemState_obj = {
    .current_state = STARTUP_CYCLE
};

void main(void) {
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
    unsigned long calculated_time = 0;

    PORTA = CLEAR;
    PORTD = CLEAR;

    while (1) {
        switch (SystemState_obj.current_state) {
            case STARTUP_CYCLE:
                __delay_ms(500);
                dip3_config();
                boost_en = 1;
                control_boost_output();
                boost_en = 0;
                update_state_machine_state(FLAG_RESET);
                break;

            case STATE_TEST_RESET:
                test_reset_wakeup = SET;
                current_time = get_time_us();
                // SET_GPIO_HIGH(TEST_POINT_2);
                while (!TEST_RST) {
                    if ((get_time_us() - current_time) > BAT_TEST_HOLD_TIME) {
                        // SET_GPIO_LOW(TEST_POINT_2);
                        battery_test();
                        break;
                    }
                }
                one_time_rst = 0;
                dip3_config();
                boost_en = 1;
                control_boost_output();
                boost_en = 0;
                update_state_machine_state(FAULT_TOGGLE);
                break;

            case STATE_EXT_WAKEUP:
                current_time = get_time_us();
                while (!EXT_WAKEUP) {
                    NOP();
                }
                calculated_time = get_time_us() - current_time;        // calculate the ext button press duration for comparison
                if ((calculated_time >= EXT_WAKEUP_RESET_DUR_MIN) && (calculated_time <= EXT_WAKEUP_RESET_DUR_MAX)) {
                    update_state_machine_state(FLAG_RESET);
                }
                else if ((calculated_time >= EXT_WAKEUP_SET_DUR_MIN) && (calculated_time <= EXT_WAKEUP_SET_DUR_MAX)) {
                    update_state_machine_state(FLAG_SET);
                }
                else {
                    update_state_machine_state(get_previous_state());
                }
                one_time_rst = 0;
                dip3_config();
                break;

            case STATE_CT_WAKEUP:
                one_time_rst = 0;
                dip3_config();
                boost_en = 1;
                control_boost_output();
                boost_en = 0;
                update_state_machine_state(FLAG_SET);
                inrush_current();
                break;

            case FLAG_SET:
                one_time_rsp = 0;
                dp3_response_delay();
                relay_trig(SET);
                flag_set();
                BOOST_FB2 = 1;
                event_count_inc();
                break;

            case FLAG_RESET:
                relay_trig(CLEAR);
                flag_reset();
                BOOST_FB2 = 1;
                event_count_inc();
                break;

            case FAULT:
                if(CT_WAKEUP)
                {
                    blink_status_led(2);
                }
                else
                {
                    blink_status_led(1);
                }
                dp3_remote_contact();
                dp3_rst_time();
                dc_rst();
                vac_rst();
                if (test_reset_wakeup) {
                    test_reset_wakeup = event_counter_reset(current_time);
                }
                break;

            case FAULT_TOGGLE:
                flag_toggle();
                break;

            case BATTERY_LOW:
                blink_battery_led(5);
                update_state_machine_state(SLEEP);
                break;

            case SLEEP:
                sleep_func();
                break;

            default:
                break;
        }
    }
}

void update_state_machine_state(SystemState_t new_state) {
    if (SystemState_obj.current_state == new_state)
        return;

    // INTCONbits.GIE = 0;   // disable interrupts
    SystemState_obj.previous_state = SystemState_obj.current_state;
    SystemState_obj.current_state = new_state;
    // INTCONbits.GIE = 1;   // enable interrupts
}

SystemState_t get_current_state(void) {
    return SystemState_obj.current_state;
}

SystemState_t get_previous_state(void) {
    return SystemState_obj.previous_state;
}