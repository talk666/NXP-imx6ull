#include "bsp_exti.h"

//初始化外部中断 gpio18   //通过定时器添加过滤功能
void exti_init(void)
{
    //复用为gpio  为gpio1-io18     GPIO1_IO18 of instance: gpio1
    SW_MUX_CTL_PAD_UART1_CTS_B = 0x5; //101  
	/* 配置GPIO1_IO18的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 11 默认上拉
     *bit [13]: 1 kepper功能
     *bit [12]: 1 pull/keeper使能
     *bit [11]: 0 关闭开路输出
     *bit [7:6]: 10 速度100Mhz
     *bit [5:3]: 0 R0/6驱动能力
     *bit [0]: 0 低转换率
     */
    SW_PAD_CTL_PAD_UART1_CTS_B = 0xF080;//0 11 1 1 0 000 10 000 00 0
#if IFCONFIG_INTERFACE
    //IO初始化
    GPIO1->GDIR &= ~(1<<18); //bit18 设置为输入
#else
    gpio_pin_config gpio_pin_config_t;
    //接口初始化
    gpio_pin_config_t.direction = KGPIO_DigitalInput;
    gpio_pin_config_t.interrpt = kGPIO_IntFallingEdgemod;
    gpio_pin_config_t.outputLogic = 1;
    gpio_init(GPIO1, 18, &gpio_pin_config_t);

    GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn); //32+67

    system_register_irq_handler_t(GPIO1_Combined_16_31_IRQn, gpio1_io18_irqhandle, NULL);
    
    gpio_ableInterrupt(GPIO1, 18);
#endif

#if IFCONFIG_EXITFILTER
    /*初始化定时器*/
    filtertimer_init(1000000/100); //10ms
#endif

}

/*初始化定时器*/
void filtertimer_init(unsigned int value)
{
    epit_init(66 - 1, value);
}

/*中断处理函数*/
void gpio1_io18_irqhandle(unsigned int gicciar, void * param) //按键中断函数
{
#if IFCONFIG_EXITFILTER
    /*开启定时器  开启10ms结束后会进入定时器中断函数*/ 
    restart_epit1(1000000/100); //10ms

    /*清除中断标志位*/
    gpio_clearintflags(GPIO1, 18);
#else
    static unsigned char state = 0;
    delay(30); /*在实际的开发中 禁止在中断函数中使用delay函数*/
    if(gpio_pinread(GPIO1, 18) == 0)
    {
        state = !state;
        Beep_Trun(state);
    }

    //清除中断标志位
    gpio_clearintflags(GPIO1, 18);
#endif
}


