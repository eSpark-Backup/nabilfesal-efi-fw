#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>

void relay_init();
void relay_set();
void relay_reset();
void relay_trig(bool state);

#endif /* RELAY_H */