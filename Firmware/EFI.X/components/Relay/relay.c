#include "relay.h"
#include "../Battery/battery.h"
#include "../Declarations/declaration.h"

void relay_init()
{
    TRISD4 = 0;   // Relay SET output
    TRISD5 = 0;   // Relay RESET output
    
    RLY_SET = 0;
    RLY_RST = 0;
}

void relay_set()
{
    // SET relay
    RLY_SET = 1;
    __delay_ms(10);
    RLY_SET = 0;
    __delay_ms(10);
}

void relay_reset()
{
    // RESET relay
    RLY_RST = 1;
    __delay_ms(10);
    RLY_RST = 0;
    __delay_ms(10);
}

void relay_toggle()
{
    // RESET relay
    RLY_RST = 1;
    __delay_ms(10);
    RLY_RST = 0;

    // Delay between RESET and SET
    __delay_ms(500);

    // SET relay
    RLY_SET = 1;
    __delay_ms(10);
    RLY_SET = 0;

    // Delay before next cycle
    __delay_ms(500);
}