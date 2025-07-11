#ifndef FLAG_DRIVER_H
#define FLAG_DRIVER_H

void flagdriv_init();
bool control_boost_output();
void set_pwm_duty(float duty_percent);
float read_ADC_voltage(unsigned char channel);

#endif /* FLAG_DRIVER_H */