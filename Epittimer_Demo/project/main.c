#include "bsp_clock.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_key_zero.h"
#include "bsp_int.h"
#include "bsp_exti.h"
#include "bsp_beep.h"
#include "bsp_epit.h"
#include "bsp_uart.h"

/*
 * @description : mian函数
 * @param               : 无
 * @return              : 无
 */
int main(void)
{

	unsigned char a = 0;

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

	while(1){

		puts("请输入 -1- 个字符:");
		a=getc();
		putc(a); /* 回显功能 */
		puts("\r\n");

		/* 显示输入的字符 */
		puts("您输入的字符为:");
		putc(a);
		puts("\r\n\r\n");

		led_turn();
	}

	return 0;
}
