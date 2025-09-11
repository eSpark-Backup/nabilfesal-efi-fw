#include "flag.h"
#include "../LED/led.h"
#include "flag_driver.h"
#include "../Relay/relay.h"
#include "../Sleep/sleep.h"
#include "../Button/button.h"
#include "../Declarations/declaration.h"

#define KP            150.0
#define KI            10.0 
#define DT            0.01 
#define PWM_CLAMP_L   0.00
#define PWM_CLAMP_H   30.0

#define ADC_CHANNEL    6
#define VREF_SET       3.7
#define VREF_CLEAR     3.6 //3.6
#define RESOLUTION     1023.0 

/*
    #define BOOST_SUPPLY_R2             220
    #define BOOST_SUPPLY_R3             47.0
    #define BOOST_SUPPLY_EQ (BOOST_SUPPLY_R2 + BOOST_SUPPLY_R3)
    #define V_TARGET_SET    ((BOOST_SUPPLY_R3/BOOST_SUPPLY_EQ) * VREF_SET)
    #define V_TARGET_CLEAR  ((BOOST_SUPPLY_R3/BOOST_SUPPLY_EQ) * VREF_SET)
*/

#define V_TARGET_SET    0.6 //0.65131
#define V_TARGET_CLEAR  0.64

void adc_init()
{
    BOOST_PWR = 0;
    ADCON0 = 0b00011001;     // Channel AN6 (RE1), ADC ON
    ADCON1 = 0b10000000;     // Configure all ANx as analog, Vref = VDD
}

void flagdriv_init()
{
    TRISE0  =   0; // BOOST_PWR
    TRISE1  =   1; // BOOST_FB1
    TRISE2  =   0; // BOOST_FB2

    BOOST_PWR = CLEAR;
    BOOST_FB2 = CLEAR;
}

void set_pwm_duty(float duty_percent)
{
    if (duty_percent > PWM_CLAMP_H) duty_percent = PWM_CLAMP_H;
    if (duty_percent < PWM_CLAMP_L) duty_percent = PWM_CLAMP_L;

    const unsigned int period_us = 50;
    unsigned long on_time = ((duty_percent / 100.0) * period_us);
    unsigned long off_time = (period_us - on_time);
    
    unsigned long now = get_time_us();
    BOOST_PWR = SET;
    while ((get_time_us() - now) < on_time);

    BOOST_PWR = CLEAR;
    while ((get_time_us() - now) < (on_time + off_time));
}

float read_ADC_voltage(unsigned char channel)
{
    ADCON0bits.CHS = channel;
    __delay_us(25);
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
    BOOST_PWR = 0;

    static float pwm_duty = 0;
    static float integral = 0;
    
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
            if (output < PWM_CLAMP_L) output = PWM_CLAMP_L;
            pwm_duty = output;
             set_pwm_duty(pwm_duty);
        }
        else
        {
            set_pwm_duty(0);
            __delay_us(100);
            BOOST_PWR = 0;
            break;
        }
    }
}