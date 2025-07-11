#ifndef DECLARATION_H
#define DECLARATION_H

#include <xc.h>
#include <stdbool.h>

#define _XTAL_FREQ 2000000

#define CLEAR          0
#define SET            1
#define TOGGLE         2

/********INPUT********/
#define BATT_SW         RC5    //Active High
#define BATT_LOW_SNS    RB5

#define BOOST_FB1       RE1

/********OUTPUT********/
#define LAMP            RA0
#define STAT_LED        RA4
#define BATT_LED        RA3
#define BATT_LOAD       RA5

#define RLY_SET         RD4
#define RLY_RST         RD5

#define LDRV_1          RB1
#define LDRV_2          RB2

#define BOOST_PWR       RE0
#define BOOST_FB2       RE2

/********WAKEUP********/
#define TEST_RST        RB0
#define EXT_WAKEUP      RB3
#define CT_WAKEUP       RB4
//#define BATT_LOW_SNS    RB5

typedef enum {
    STATE_IDLE = 0,
    STARTUP_CYCLE,
    STATE_TEST_RESET,
    STATE_EXT_WAKEUP,
    STATE_CT_WAKEUP,
    FLAG_SET,
    FLAG_RESET,
    FAULT,
    FAULT_CLEAR,
    FAULT_TOGGLE,
    BATTERY_LOW,
    SLEEP
} SystemState_t;

typedef struct {
    SystemState_t current_state;
} StateMachine_t;

extern volatile StateMachine_t SystemState_obj;

#endif /* DECLARATION_H */