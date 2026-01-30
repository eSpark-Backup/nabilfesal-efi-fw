#ifndef DECLARATION_H
#define DECLARATION_H

#include <xc.h>
#include <stdbool.h>

#define _XTAL_FREQ 4000000

#define CLEAR          0
#define SET            1
#define TOGGLE         2

/********INPUT********/
#define BATT_SW         RC5    //Active High
#define BATT_LOW_SNS    RB5

#define BOOST_FB1       RE1

#define VDC_110         RA2
#define VAC_240         RA6

#define RMT_CNT_SW      RD3
#define RST_TIME_1      RD2
#define RST_TIME_2      RD1
#define IN_RSH_CRNT     RD0
#define RSP_DELAY_1     RC3
#define RSP_DELAY_2     RC2

#define AUTO_RST        RC7

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

#define CONFIG_SW_COM   RC4

#define RA7_OUTPUT      RA7
#define RC6_OUTPUT      RC6

/********WAKEUP********/
#define TEST_RST        RB0
#define EXT_WAKEUP      RB3
#define CT_WAKEUP       RB4

bool boost_en = 0;

typedef enum {
    STARTUP_CYCLE = 0,
    STATE_TEST_RESET,
    STATE_EXT_WAKEUP,
    STATE_CT_WAKEUP,
    FLAG_SET,
    FLAG_RESET,
    FAULT,
    FAULT_TOGGLE,
    BATTERY_LOW,
    SLEEP,
    HELLO
} SystemState_t;

typedef struct {
    SystemState_t previous_state;
    SystemState_t current_state;
} StateMachine_t;

extern volatile StateMachine_t SystemState_obj;

void update_state_machine_state(SystemState_t new_state);
SystemState_t get_current_state(void);
SystemState_t get_previous_state(void);

#define US_TO_MS(us)    (us / 1000UL)
#define MS_TO_US(ms)    (ms * 1000UL)

#endif /* DECLARATION_H */