#include "button.h"

void button_init()
{
    TRISC5 = 1;   // Battery Switch input
    TRISB0 = 1;   // Test Switch input
}