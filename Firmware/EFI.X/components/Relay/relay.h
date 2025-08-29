#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>

void relay_init(void);
void relay_set(void);
void relay_reset(void);
void relay_trig(bool state);

#endif /* RELAY_H */