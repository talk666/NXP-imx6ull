#include "bsp_clock.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_key_zero.h"
#include "bsp_int.h"
#include "bsp_exti.h"
#include "bsp_beep.h"
#include "bsp_epit.h"
#include "bsp_uart.h"
#include "bsp_rtc.h"
#include "bsp_ap3216c.h"
#include "bsp_icm20608.h"
#include "stdio.h"

/*
 * @description	: 使能I.MX6U的硬件NEON和FPU 和cp15有关
 * @param 		: 无
 * @return 		: 无
 */
 void imx6ul_hardfpu_enable(void)
{
	uint32_t cpacr;
	uint32_t fpexc;

	/* 使能NEON和FPU */
	cpacr = __get_CPACR();
	cpacr = (cpacr & ~(CPACR_ASEDIS_Msk | CPACR_D32DIS_Msk))
		   |  (3UL << CPACR_cp10_Pos) | (3UL << CPACR_cp11_Pos);
	__set_CPACR(cpacr);
	fpexc = __get_FPEXC();
	fpexc |= 0x40000000UL;	
	__set_FPEXC(fpexc);
}

/*
 * @description : mian函数
 * @param               : 无
 * @return              : 无
 */
int main(void)
{

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

	RTC_Init();                      /*rtc初始化*/

	ap3216c_init();                   /*ap3216c传感器初始化*/
	
	icm20608_init();               /*icm20608模块初始化*/

	imx6ul_hardfpu_enable();      //I.MX6U的硬件NEON和FPU 和cp15有关

	while(icm20608_init())		/* 初始化ICM20608	 			*/
	{	
		printf("ICM20608 Check Failed!Please Check!\r\n");
		delay_ms(500);
	}	
	
	printf("ICM20608 Check secceed!!!\r\n");

	while(1)
	{	
		icm20608_getdata();
		delay_ms(50);

		printf("accel x = %d,accel y = %d,accel z = %d\r\n",icm20608_dev.accel_x_adc, icm20608_dev.accel_y_adc, icm20608_dev.accel_z_adc);
		printf("gyrp  x = %d,gyro  y = %d,gyro  z = %d\r\n",icm20608_dev.gyro_x_adc, icm20608_dev.gyro_y_adc, icm20608_dev.gyro_z_adc);
		printf("temp    = %d\r\n",icm20608_dev.temp_adc);
	}
	return 0;
}
