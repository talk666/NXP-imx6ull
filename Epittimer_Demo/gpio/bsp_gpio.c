#include "bsp_gpio.h"

void gpio_init(GPIO_Type *base, int pin, gpio_pin_config *config)
{   
    base->IMR &= ~(1U << pin);

    //input
    if (config->direction == KGPIO_DigitalInput)
    {
        base->GDIR &= ~(1<<pin);
    }
    //output
    if (config->direction == KGPIO_DigitalOutput)
    {
        base->GDIR |= (1<<pin);
        //设置默认输出电平
        if(config->outputLogic == 0)
        {
            base->DR &= ~(1 << pin);	
        }
        else if(config->outputLogic == 1)
            base->DR |= (1 << pin);	
    }
    //interrupt
    gpio_interruptinit(base, pin, config->interrpt);

}

//控制gpio高低电平
void gpio_pinwrite(GPIO_Type *base, int pin, int value)
{
    if(value == 1)
    {
        base->DR |= (1<<pin);
    }
    else if(value == 0)
        base->DR &= ~(1<<pin);
}

//读取IO 电平值
int gpio_pinread(GPIO_Type *base, int pin)
{
    return (((base->DR)>>pin) & (0x1));
}

//使能gpio中断功能
void gpio_ableInterrupt(GPIO_Type *base, int pin)
{
    base->IMR |= (1<<pin);
}
//失能gpio中断功能
void gpio_disableInterrupt(GPIO_Type *base, int pin)
{
    base->IMR &= ~(1<<pin);
}

/*
* @description : 清除中断标志位(写 1 清除)
* @param - base : 要清除的 IO 所在的 GPIO 组。
* @param - pin : 要清除的 GPIO 掩码。
* @return : 无
*/
void gpio_clearintflags(GPIO_Type *base, int pin)
{
    base->ISR |= (1<<pin);
}

//gpio中断初始化
void gpio_interruptinit(GPIO_Type *base, int pin, gpio_interrupt_mode mode)
{
    volatile uint32_t *icr;
    uint32_t icrshift;

    icrshift = pin;
    //上升下降沿触发控制寄存器  清零
    base->EDGE_SEL &= ~(1U << pin);

    if(pin<16) icr = &(base->ICR1);
    else 
    {
        icr = &(base->ICR2);
        icrshift -= 16;
    }
    
    switch (mode)
    {
    case kGPIO_IntLowmode:
        /* 低电平触发 */
        //清零
        *icr &= ~(3<<(2*icrshift));
        break;

    case kGPIO_IntHightmode:
        /* 高电平触发 */
        *icr &= ~(3<<(2*icrshift));
        *icr |= (1<<(2*icrshift));
        break;

    case kGPIO_IntRisingEdgemode:
        /* 上升沿触发 */
        *icr &= ~(3<<(2*icrshift));
        *icr |= (2<<(2*icrshift));
         break;
        
    case kGPIO_IntFallingEdgemod:
        /* 下降沿触发 */
        *icr &= ~(3<<(2*icrshift));
        *icr |= (3<<(2*icrshift));
         break;
        
    case kGPIO_IntFallingOrRisingEdgemode:
        /* 上升沿下降沿触发 */
        base->EDGE_SEL |= (1<<pin);
        break;

    default:
        break;
    }
}
