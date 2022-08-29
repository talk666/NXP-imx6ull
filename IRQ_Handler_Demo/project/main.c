#include <stdio.h>
#include "bsp_clock.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_key_zero.h"
#include "bsp_int.h"
#include "bsp_exti.h"
#include "bsp_beep.h"

/*
 * @description : mian函数
 * @param               : 无
 * @return              : 无
 */
int main(void)
{
	int_init();                     /*中断处理*/

	Imx6u_clkinit();
	clk_enable();          			/* 使能所有的时钟*/

	led_init();            			/* 初始化led*/
	Key_Init();            			/*按键初始化*/
	Beep_Init();                    /*蜂鸣器初始化*/

	exti_init();                   /*按键中断初始化*/

	while(1)                  
	{
		led_turn();
		delay(500);
	}

	return 0;
}
