#ifndef __BPS_GPIO_H__
#define __BPS_GPIO_H__

#include "imx6ul.h"

/*描述中断触发类型*/
typedef enum _gpio_interrupt_mode
{
    kGPIO_Nointmode= 0U, /*无触发*/
    kGPIO_IntLowmode = 1U, /*低电平触发*/
    kGPIO_IntHightmode = 2U, /*高电平触发*/
    kGPIO_IntRisingEdgemode = 3U, /*上升沿触发*/
    kGPIO_IntFallingEdgemod = 4U, /*下降沿触发*/
    kGPIO_IntFallingOrRisingEdgemode = 5U/*上升沿下降沿触发*/
}gpio_interrupt_mode;

//设置输出输入
typedef enum _gpio_pin_direction
{
    KGPIO_DigitalInput = 0U,
    KGPIO_DigitalOutput = 1U,
}gpio_pin_direction;

typedef struct  _gpio_pin_config
{
    unsigned char outputLogic; //默认电平
    gpio_pin_direction direction;
    gpio_interrupt_mode interrpt; //中断相关
}gpio_pin_config;


void gpio_init(GPIO_Type *base, int pin, gpio_pin_config *config);
void gpio_pinwrite(GPIO_Type *base, int pin, int value);
int gpio_pinread(GPIO_Type *base, int pin);

void gpio_ableInterrupt(GPIO_Type *base, int pin);
void gpio_disableInterrupt(GPIO_Type *base, int pin);
void gpio_clearintflags(GPIO_Type *base, int pin);

/*中断初始化*/
void gpio_interruptinit(GPIO_Type *base, int pin, gpio_interrupt_mode mode);
#endif
