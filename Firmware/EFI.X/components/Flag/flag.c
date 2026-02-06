#include "flag.h"
#include "../Declarations/declaration.h"

#define DELAY   400
static bool flag_state = CLEAR;

void flag_init()
{
    TRISB1  = 0;
    TRISB2  = 0;

    SET_GPIO_LOW(LDRV_1);
    SET_GPIO_LOW(LDRV_2);
}

/*FLAG SET*/
// LDRV_1 = 0;
// LDRV_2 = 1;

void flag_set()
{
    SET_GPIO_LOW(LDRV_1);
    SET_GPIO_HIGH(LDRV_2);
    __delay_us(DELAY);
    SET_GPIO_LOW(LDRV_1);
    SET_GPIO_LOW(LDRV_2);

    flag_state = SET;  //Set
    update_state_machine_state(FAULT);
}

/*FLAG RESET*/
// LDRV_1 = 1;
// LDRV_2 = 0;

void flag_reset()
{
    SET_GPIO_HIGH(LDRV_1);
    SET_GPIO_LOW(LDRV_2);
    __delay_us(DELAY);
    SET_GPIO_LOW(LDRV_1);
    SET_GPIO_LOW(LDRV_2);

    flag_state = CLEAR;  //Reset
    update_state_machine_state(SLEEP);
}

void flag_toggle()
{
    if(!flag_state)
    {
        update_state_machine_state(FLAG_SET);
    }
    else
    {
        update_state_machine_state(FLAG_RESET);
    }
}