#ifndef __BPS_LED_H__
#define __BPS_LED_H__
#include "imx6ul.h"
#include "bsp_gpio.h"

void led_init(void);

void led_on(void);

void led_off(void);

void led_Trun_status(unsigned char status);

void led_turn(void);
#endif