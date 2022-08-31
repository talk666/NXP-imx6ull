#include "bsp_epit.h"

//初始化epit
void epit_init(unsigned int frac, unsigned int value)
{
	if(frac > 4095) frac = 4095;
	if(value > 0xffffffff) value = 0xffffffff;
	/*配置EPIT_1_CR寄存器*/
	EPIT1->CR = 0;

	EPIT1->CR |= (1 << 1); //定时器启动时重新从load value计数  0为上次关闭时进行计数
	EPIT1->CR |= (1 << 2); //中断功能
	EPIT1->CR |= (1 << 3); //load value从LR寄存器中获取
	EPIT1->CR |= (frac << 4); //分频
	EPIT1->CR |= (1 << 24); //时钟选择 pkg_clk

	EPIT1->LR = value;

	EPIT1->CMPR = 0;

	/*初始化中断*/
	GIC_EnableIRQ(EPIT1_IRQn); //使能中断
	system_register_irq_handler_t(EPIT1_IRQn, epit1_irqhandle, NULL);

#if !IFCONFIG_EXITFILTER
	//开启epit1定时器
	EPIT1->CR |= (1 << 0);
#endif

}


/*EPIT1中断服务函数*/
void epit1_irqhandle(unsigned int gicciar, void * param) //定时器中断函数
{
#if IFCONFIG_EXITFILTER
	static unsigned char status = 0;

	if(EPIT1->SR & (1 << 0))
	{
		//关闭定时器
		disable_epit1();
		if(gpio_pinread(GPIO1, 18) == 0)//为按下状态
		{
			status = !status;
			led_Trun_status(status);
		}
	}

	/*清除中断标志位*/
	EPIT1->SR |= (1 << 0);
#else
	if(EPIT1->SR & (1 << 0))
	{
		//有中断发生
		led_turn();
	}

	/*清除中断标志位*/
	EPIT1->SR |= (1 << 0);
#endif
}

/*EPIT1定时器关闭*/
void disable_epit1(void)
{
	EPIT1->CR &= ~(1 << 0);
}

/*EPIT1定时器重启*/
void restart_epit1(unsigned value)
{
	disable_epit1();
	EPIT1->LR = value;
	//开启epit1定时器
	EPIT1->CR |= (1 << 0);
}
