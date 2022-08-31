#ifndef __BPS_EXTI_H__
#define __BPS_EXTI_H__
#include "imx6ul.h"

#include "bsp_gpio.h"
#include "bsp_beep.h"
#include "bsp_int.h"
#include "bsp_delay.h"
#include "bsp_key_zero.h"
#include "bsp_epit.h"

//初始化外部中断 gpio18
void exti_init(void);

/*中断处理函数*/
void gpio1_io18_irqhandle(unsigned int gicciar, void * param);

/*按键过滤定时器初始化*/
void filtertimer_init(unsigned int value);
#endif