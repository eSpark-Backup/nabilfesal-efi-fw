#include <xc.h>

#define _XTAL_FREQ 4000000

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