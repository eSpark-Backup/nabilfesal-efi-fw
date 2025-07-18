#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

void battery_init();
void battery_test();
void battery_ind();
void battery_check();
void event_count_inc();
void event_counter_reset();
uint16_t event_count_read();

#endif /* BATTERY_H */