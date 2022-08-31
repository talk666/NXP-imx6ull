#ifndef __BPS_DELAY_H__
#define __BPS_DELAY_H__

#include "imx6ul.h"
#include "bsp_int.h"


void delay_short(volatile unsigned int n);
void delay(volatile unsigned int n);
void delay_init(void);
void gpt1_irqhandle(unsigned int gicciar, void * param);

void delay_us(unsigned int usdelay);
void delay_ms(unsigned int usdelay);
void delay_s(unsigned int usdelay);

#endif