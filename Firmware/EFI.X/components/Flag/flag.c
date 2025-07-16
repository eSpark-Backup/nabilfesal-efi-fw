#include "flag.h"
#include "../Declarations/declaration.h"

static bool flag_state = 1;

/*FLAG SET
LDRV_1 = 0;
LDRV_2 = 1;
*/

/*FLAG RESET
LDRV_1 = 1;
LDRV_2 = 0;
*/

void flag_init()
{
    TRISB1  = 0;
    TRISB2  = 0;
    
    LDRV_1 = 0;
    LDRV_2 = 0;
}

void flag_trig()
{
    LDRV_1 = flag_state;
    LDRV_2 = !flag_state;
    __delay_ms(500);
    LDRV_1 = 0;
    LDRV_2 = 0;
    
    if(!flag_state)
    {
        SystemState_obj.current_state = FAULT;
    }
    else
    {
        SystemState_obj.current_state = SLEEP;
    }

    flag_state = !flag_state;
}