#ifndef LED_H
#define LED_H

void led_init(void);
void blink_battery_led(unsigned char time);
void blink_status_led(unsigned char time);
void timer1_delay_ms(unsigned int delay_ms);

#endif /* LED_H */