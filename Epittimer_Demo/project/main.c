#include "bsp_clock.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_key_zero.h"
#include "bsp_int.h"
#include "bsp_exti.h"
#include "bsp_beep.h"
#include "bsp_epit.h"
#include "bsp_uart.h"
#include "stdio.h"

/*
 * @description : mian函数
 * @param               : 无
 * @return              : 无
 */
int main(void)
{

	int a, b;

	int_init();                     /*中断处理*/

	Imx6u_clkinit();				/*时钟使能初始化*/

	clk_enable();          			/* 使能所有的时钟*/

	led_init();            			/* 初始化led*/
	//Key_Init();            		/*按键初始化*/

	Beep_Init();                    /*蜂鸣器初始化*/

	exti_init();                   /*按键中断初始化*/

	delay_init();                  /*延迟函数使用GPT定时器初始化*/

	uart1_init();                    /*uart1初始化*/
#if !IFCONFIG_EXITFILTER
	//66MHZ 设置为66分频 时钟为1MHZ    也就是说1s -> 1000000次    装载值为1000000为1s
	epit_init(66 - 1, 1000000/2);
#endif

	while(1)					
	{	
		printf("输入两个整数，使用空格隔开:");
		scanf("%d %d", &a, &b);					 		/* 输入两个整数 */
		printf("\r\n数据%d + %d = %d\r\n\r\n", a, b, a+b);	/* 输出两个数相加的和 */
		printf("%d的16进制数据为%#x\r\n", a+b, a+b);
		led_turn();
	}

	return 0;
}
