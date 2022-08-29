#ifndef __BPS_KEY_H__
#define __BPS_KEY_H__
#include "imx6ul.h"
#include "bsp_gpio.h"


#define SW_MUX_CTL_PAD_UART1_CTS_B       *((volatile unsigned long *)0x20E008)
#define SW_PAD_CTL_PAD_UART1_CTS_B       *((volatile unsigned long *)0x20E0318)

//配置为 gpio的话 为GPIO1_IO18


//按键值
enum keyvalue{
    KEY_NONE = 0,
    KEY0_VALUE,
};

void Key_Init(void);
int read_key(void);
int key_getvalue(void);

#endif