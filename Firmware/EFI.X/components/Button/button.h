#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

volatile bool pwm_state = 0;
volatile unsigned long timer0_ms = 0;
volatile unsigned int on_time_ticks = 0;
volatile unsigned int off_time_ticks = 0;

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

void button_init();
void handle_test_rst();
void handle_ext_wakeup();
void handle_ct_wakeup();
void dc_rst();
void vac_rst();
void dip3_config();
void dp3_remote_contact();
void dp3_rst_time();
void dp3_response_delay();
unsigned long get_time_ms();

#endif /* BUTTON_H */