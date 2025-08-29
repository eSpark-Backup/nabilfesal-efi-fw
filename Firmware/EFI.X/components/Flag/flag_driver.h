#ifndef FLAG_DRIVER_H
#define FLAG_DRIVER_H

void adc_init(void);
void flagdriv_init(void);
bool control_boost_output(void);
void set_pwm_duty(float duty_percent);
float read_ADC_voltage(unsigned char channel);

#endif /* FLAG_DRIVER_H */