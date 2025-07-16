#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

volatile bool pwm_state = 0;
volatile unsigned long timer0_ms = 0;
volatile unsigned int on_time_ticks = 0;
volatile unsigned int off_time_ticks = 0;

void button_init();
void handle_test_rst();
void handle_ext_wakeup();
void handle_ct_wakeup();
unsigned long get_time_ms();

#endif /* BUTTON_H */