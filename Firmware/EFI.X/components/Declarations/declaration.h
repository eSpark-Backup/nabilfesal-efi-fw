#ifndef DECLARATION_H
#define DECLARATION_H

#include <xc.h>
#include <stdbool.h>

#define _XTAL_FREQ 4000000

#define PRESCALAR_ON_WDT    1
#define PRESCALAR_ON_TMR    0

#define PRESCALAR_SELECTION PRESCALAR_ON_TMR
#define TMR0_PRESCALER      0b000   // only used when prescalar on TMR0

// ==================================
#if (PRESCALAR_SELECTION == PRESCALAR_ON_WDT)

/* Prescaler on WDT → TMR0 has NO prescaler */
    #define TMR0_TICK_US        1
    #define TMR0_OVERFLOW_US    256

#elif (PRESCALAR_SELECTION == PRESCALAR_ON_TMR)

    #if   (TMR0_PRESCALER == 0b000)   // 1:2
        #define TMR0_TICK_US        2
        #define TMR0_OVERFLOW_US    512
    #elif (TMR0_PRESCALER == 0b001)   // 1:4
        #define TMR0_TICK_US        4
        #define TMR0_OVERFLOW_US    1024
    #elif (TMR0_PRESCALER == 0b010)   // 1:8
        #define TMR0_TICK_US        8
        #define TMR0_OVERFLOW_US    2048
    #elif (TMR0_PRESCALER == 0b011)   // 1:16
        #define TMR0_TICK_US        16
        #define TMR0_OVERFLOW_US    4096
    #elif (TMR0_PRESCALER == 0b100)   // 1:32
        #define TMR0_TICK_US        32
        #define TMR0_OVERFLOW_US    8192
    #elif (TMR0_PRESCALER == 0b101)   // 1:64
        #define TMR0_TICK_US        64
        #define TMR0_OVERFLOW_US    16384
    #elif (TMR0_PRESCALER == 0b110)   // 1:128
        #define TMR0_TICK_US        128
        #define TMR0_OVERFLOW_US    32768
    #elif (TMR0_PRESCALER == 0b111)   // 1:256
        #define TMR0_TICK_US        256
        #define TMR0_OVERFLOW_US    65536
    #else
        #error "Invalid TMR0_PRESCALER value"
    #endif

#else
    #error "Invalid PRESCALAR_SELECTION"
#endif
// ==================================


#define US_TO_MS(us)    (us / 1000UL)
#define MS_TO_US(ms)    (ms * 1000UL)
#define S_TO_MS(s)      (s * 1000UL)
#define MS_TO_S(ms)     (ms / 1000UL)
#define S_TO_US(s)      (s * 1000000UL)
#define US_TO_S(us)     (us / 1000000UL)
#define M_TO_S(m)       (m * 60UL)
#define S_TO_M(s)       (s / 60UL)
#define M_TO_US(m)      (M_TO_S(m) * 1000000UL)
#define US_TO_M(us)     (US_TO_S(us) / 60UL)
#define H_TO_S(h)       (h * 3600UL)
#define S_TO_H(s)       (s / 3600UL)
#define H_TO_MS(h)      (H_TO_S(h) * 1000UL)
#define H_TO_US(h)      (H_TO_S(h) * 1000000UL)
#define US_TO_H(us)     (US_TO_S(us) / 3600UL)


#define CLEAR          0
#define SET            1
#define TOGGLE         2

#define SET_GPIO_LOW(gpio)   (gpio = CLEAR)
#define SET_GPIO_HIGH(gpio)  (gpio = SET)
#define TOGGLE_GPIO(gpio)    (gpio ^= 1)

/********TEST_POINTS********/
#define ENABLE_TEST_POINTS
#define TEST_POINT_1      RD6
#define TEST_POINT_2      RD7


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

#endif /* DECLARATION_H */