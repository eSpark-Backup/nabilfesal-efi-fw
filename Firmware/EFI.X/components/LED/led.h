#ifndef LED_H
#define LED_H

void led_init();
void led_off();
void blink_battery_led(unsigned char time);
void blink_status_led(unsigned char time);

#endif /* LED_H */