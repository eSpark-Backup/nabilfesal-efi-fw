#include "flag.h"
#include "../Declarations/declaration.h"

#define DELAY   400
static bool flag_state = CLEAR;

void flag_init()
{
    TRISB1  = 0;
    TRISB2  = 0;
    
    LDRV_1 = CLEAR;
    LDRV_2 = CLEAR;
}

/*FLAG SET*/
// LDRV_1 = 0;
// LDRV_2 = 1;

void flag_set()
{
    LDRV_1 = CLEAR;
    LDRV_2 = SET;
    __delay_us(DELAY);
    LDRV_1 = CLEAR;
    LDRV_2 = CLEAR;
    
    flag_state = SET;  //Set
    SystemState_obj.current_state = FAULT;
}

/*FLAG RESET*/
// LDRV_1 = 1;
// LDRV_2 = 0;

void flag_reset()
{
    LDRV_1 = SET;
    LDRV_2 = CLEAR;
    __delay_us(DELAY);
    LDRV_1 = CLEAR;
    LDRV_2 = CLEAR;
    
    flag_state = CLEAR;  //Reset
    SystemState_obj.current_state = SLEEP;
}

void flag_toggle()
{
    if(!flag_state)
    {
        SystemState_obj.current_state = FLAG_SET;
    }
    else
    {
        SystemState_obj.current_state = FLAG_RESET;
    }
}