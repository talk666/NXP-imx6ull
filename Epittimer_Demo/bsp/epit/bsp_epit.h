#ifndef __BPS_EPIT_H__
#define __BPS_EPIT_H__

#include "imx6ul.h"
#include "bsp_int.h"
#include "bsp_led.h"

/*参数为分频系数和装载值*/
void epit_init(unsigned int frac, unsigned int value);
void epit1_irqhandle(unsigned int gicciar, void * param); //定时器中断函数

#endif