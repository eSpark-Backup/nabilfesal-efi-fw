#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

#define EXT_WAKEUP_RESET_DUR_MIN        (1500UL*1000UL)/2       // 500ms
#define EXT_WAKEUP_RESET_DUR_MAX        (3500UL*1000UL)/2      // 3.5s
#define EXT_WAKEUP_SET_DUR_MIN          (5000UL*1000UL)/2      // 5s
#define EXT_WAKEUP_SET_DUR_MAX          (10000UL*1000UL)/2     // 10s

volatile unsigned long timer0_us = 0;

bool one_time_rst = 0;
bool one_time_rsp = 0;

typedef struct {
    bool sw1;
    bool sw2;
    bool sw3;
    bool sw4;
    bool sw5;
    bool sw6;
} dip_sw_config_t;

extern dip_sw_config_t dip_sw_config_obj;

void button_init(void);
void handle_test_rst();
void handle_ext_wakeup();
void handle_ct_wakeup();
void dc_rst(void);
void vac_rst(void);
void inrush_current(void);
void dip3_config(void);
void dp3_remote_contact(void);
void dp3_rst_time(void);
void dp3_response_delay(void);
unsigned long get_time_us(void);

#endif /* BUTTON_H */