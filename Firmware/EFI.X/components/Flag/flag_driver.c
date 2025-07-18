#include "flag.h"
#include "../LED/led.h"
#include "flag_driver.h"
#include "../Relay/relay.h"
#include "../Sleep/sleep.h"
#include "../Button/button.h"
#include "../Declarations/declaration.h"

#define KP   150.0     // More = faster response, too high = overshoot
#define KI   10.0      // More = better stability, too high = slow or unstable
#define DT   0.01      // 10ms loop time

#define VREF_SET    3.7
#define VREF_CLEAR  3.6
#define ADC_CHANNEL  6 
#define RESOLUTION  1023.0 

#define PWM_CLAMP_H   40.0
#define PWM_CLAMP_L   0.0
/*
    #define BOOST_SUPPLY_R2             220
    #define BOOST_SUPPLY_R3             47.0
    #define BOOST_SUPPLY_EQ (BOOST_SUPPLY_R2 + BOOST_SUPPLY_R3)
    #define V_TARGET_SET    ((BOOST_SUPPLY_R3/BOOST_SUPPLY_EQ) * VREF_SET)
    #define V_TARGET_CLEAR  ((BOOST_SUPPLY_R3/BOOST_SUPPLY_EQ) * VREF_SET)
*/

#define V_TARGET_SET    0.61
#define V_TARGET_CLEAR  0.64


static bool one_shot = false;

void adc_init()
{
    ADCON0 = 0b00011001;     // Channel AN6 (RE1), ADC ON
    ADCON1 = 0b10000000;     // Configure all ANx as analog, Vref = VDD
}

void pwm_timer_init()
{
    PR2 = 24;                  // PWM period ~51 µs
    TMR2 = 0;                  // Clear counter
    T2CON = 0b00000101;        // Prescaler 1:4, Timer2 ON (POST = 1:1, PRES = 1:4)
    PIR1bits.TMR2IF = 0;       // Clear interrupt flag
    PIE1bits.TMR2IE = 1;       // Enable Timer2 interrupt

    pwm_state = 0;
    BOOST_PWR = 0;
}

void flagdriv_init()
{
    TRISE0  =   0; // BOOST_PWR
    TRISE1  =   1; // BOOST_FB1
    TRISE2  =   0; // BOOST_FB2

    BOOST_PWR = 0;
    BOOST_FB2 = 0;
}

void set_pwm_duty(float duty_percent)
{
    if (duty_percent > PWM_CLAMP_H) duty_percent = PWM_CLAMP_H;
    if (duty_percent < PWM_CLAMP_L) duty_percent = PWM_CLAMP_L;

    const unsigned int period_us = 51;  // 1 cycle = 51 �s
    float on_time = (duty_percent / 100.0) * period_us;
    float off_time = period_us - on_time;

    on_time_ticks = (unsigned int)(on_time * 2);   // since 1 tick = 0.5 �s
    off_time_ticks = (unsigned int)(off_time * 2);
}


float read_ADC_voltage(unsigned char channel) {
    ADCON0bits.CHS = channel;
    __delay_us(10);
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    uint16_t result = ((ADRESH << 8) | ADRESL);
    float value = result * VREF_CLEAR;
    float voltage = value/RESOLUTION;
    return voltage;
}

bool control_boost_output()
{
    BOOST_FB2 = 0;
    static float pwm_duty = 0;
    static float integral = 0;
    
    if(!one_shot)
    {
        pwm_timer_init();
        adc_init();
        ++one_shot;
    }
    
    while (1)
    {
        float v_feedback = read_ADC_voltage(ADC_CHANNEL);
        if(v_feedback < V_TARGET_SET)
        {
            float error = V_TARGET_SET - v_feedback;
    
            // Basic PI logic
            integral += error * DT;
            float output = (KP * error) + (KI * integral);
    
            if (output > PWM_CLAMP_H) output = PWM_CLAMP_H;
            if (output < PWM_CLAMP_L) output = PWM_CLAMP_H;
            pwm_duty = output;
            set_pwm_duty(pwm_duty);
        }
        else
        {
            blink_battery_led(5);
            set_pwm_duty(0);
            __delay_ms(100);
            BOOST_PWR = 0;
            break;
        }
    }
    
}