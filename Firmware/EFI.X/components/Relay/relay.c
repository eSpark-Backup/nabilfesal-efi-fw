#include "relay.h"
#include "../Declarations/declaration.h"

#define RELAY_DELAY     4

void relay_init()
{
    TRISD4 = 0;   // Relay SET output
    TRISD5 = 0;   // Relay RESET output
    
    RLY_SET = CLEAR;
    RLY_RST = CLEAR;
}

void relay_trig(bool state)
{
    switch (state)
    {
        case CLEAR:
            RLY_RST = !CLEAR;
            __delay_ms(RELAY_DELAY);
            RLY_RST = CLEAR;
            __delay_ms(RELAY_DELAY);
            break;
        case SET:
            // SET relay
            RLY_SET = SET;
            __delay_ms(RELAY_DELAY);
            RLY_SET = !SET;
            __delay_ms(RELAY_DELAY);
            break;
    default:
        break;
    }
}