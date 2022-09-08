#include "bsp_led.h"

/*
 * @description	: 初始化LED对应的GPIO
 * @param 		: 无
 * @return 		: 无
 */
void led_init(void)
{
	/* 1、初始化IO复用 */
	IOMUX_SW_MUX->GPIO1_IO03 = 0X5;		/* 复用为GPIO1_IO03 */


	/* 2、配置GPIO1_IO03的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 00 默认下拉
     *bit [13]: 0 kepper功能
     *bit [12]: 1 pull/keeper使能
     *bit [11]: 0 关闭开路输出
     *bit [7:6]: 10 速度100Mhz
     *bit [5:3]: 110 R0/6驱动能力
     *bit [0]: 0 低转换率
     */
    IOMUX_SW_PAD->GPIO1_IO03 = 0X10B0;

#if IFCONFIG_INTERFACE
	/* 3、初始化GPIO */
	GPIO1->GDIR |= (1<<3);	/* GPIO1_IO03设置为输出 */
	/* 4、设置GPIO1_IO03输出低电平，打开LED0 */	
	GPIO1->DR &= ~(1 << 3);	
#else
	gpio_pin_config gpio_pin_config_t;
	gpio_pin_config_t.direction = KGPIO_DigitalOutput;
	gpio_pin_config_t.outputLogic = 1; //设置默认电平 低电平，打开LED0  高电平，关闭LED0 
	
	gpio_init(GPIO1, 3, &gpio_pin_config_t);
#endif


}


/*
 * @description	: 打开LED灯
 * @param 		: 无
 * @return 		: 无
 */
void led_on(void)
{
#if IFCONFIG_INTERFACE
	/* 将GPIO1_DR的bit3清零 	*/
	GPIO1->DR &= ~(1<<3); 
#else
	gpio_pinwrite(GPIO1, 3, 0);
#endif
}

/*
 * @description	: 关闭LED灯
 * @param 		: 无
 * @return 		: 无
 */
void led_off(void)
{
#if IFCONFIG_INTERFACE
	/* 将GPIO1_DR的bit3置1 */
	GPIO1->DR |= (1<<3); 
#else
	gpio_pinwrite(GPIO1, 3, 1);
#endif

}

void led_turn(void)
{
	int status = 0;
    status = (GPIO1->DR)>>3 & 0x1;
    if(status == 0)//亮的状态
	{
		//关闭
		led_off();
	}
	else if(status == 1)
	{
		//开灯
		led_on();
	}
}

void led_Trun_status(unsigned char status)
{
    if(status)//开
        led_on();
    else   //关
        led_off();

}