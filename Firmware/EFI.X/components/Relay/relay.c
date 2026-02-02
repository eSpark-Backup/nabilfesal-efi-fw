#include "relay.h"
#include "../Declarations/declaration.h"

#define RELAY_DELAY     4

void relay_init()
{
    TRISD4 = 0;   // Relay SET output
    TRISD5 = 0;   // Relay RESET output

    SET_GPIO_LOW(RLY_SET);
    SET_GPIO_LOW(RLY_RST);
}

void relay_trig(bool state)
{
    switch (state)
    {
        case CLEAR:
            SET_GPIO_HIGH(RLY_RST);
            __delay_ms(RELAY_DELAY);
            SET_GPIO_LOW(RLY_RST);
            __delay_ms(RELAY_DELAY);
            break;
        case SET:
            // SET relay
            SET_GPIO_HIGH(RLY_SET);
            __delay_ms(RELAY_DELAY);
            SET_GPIO_LOW(RLY_SET);
            __delay_ms(RELAY_DELAY);
            break;
    default:
        break;
    }
}