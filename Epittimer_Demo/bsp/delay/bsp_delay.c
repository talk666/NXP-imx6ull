#include "bsp_delay.h"
#include "bsp_led.h"

/*
 * @description	: 短时间延时函数
 * @param - n	: 要延时循环次数(空操作循环次数，模式延时)
 * @return 		: 无
 */
void delay_short(volatile unsigned int n)
{
	while(n--){}
}

/*
 * @description	: 延时函数,在396Mhz的主频下
 * 			  	  延时时间大约为1ms
 * @param - n	: 要延时的ms数
 * @return 		: 无
 */
void delay(volatile unsigned int n)
{
	while(n--)
	{
		delay_short(0x7ff);
	}
}


void delay_init(void)
{
	GPT1->CR = 0;

//软复位
	GPT1->CR = (1 << 15);
	while ((GPT1->CR >> 15) & 0x01);//软复位完成
	
	//配置寄存器
	GPT1->CR |= (1 << 1);//counter value is reset to 0 when it is disabled
	GPT1->CR |= (1 << 6);// Peripheral Clock (ipg_clk)
	GPT1->CR |= (0 << 9);//Restart mode

	GPT1->PR = ((66 -1) << 0);//66分频

	/*输出比较寄存器1*/
	/*//When the counter value equals the COMP bit field value, 
	*a compare event is generated on Output Compare Channel 1.
	*/

#if 0  //GPT1 输出通道1 中断函数测试条件
	GPT1->OCR[0] = 0;
	GPT1->OCR[0] |= 1000000/2;  //500ms

	/*打开GPT1的比较1通道中断*/
	/*OF1IE Onput Capture 1 Interrupt Enable is enabled.*/
	GPT1->IR |= (1 << 0); 

	/*GIC*/
	GIC_EnableIRQ(GPT1_IRQn);

	system_register_irq_handler_t(GPT1_IRQn, gpt1_irqhandle, NULL);
#else
/*66分频后为1Mhz  也就是计数一次为1us  oxffffffff=4294967295us=71min*/
	GPT1->OCR[0] = 0;
	GPT1->OCR[0] |= 0xffffffff; 
#endif
	/*使能GPT1定时器*/
	GPT1->CR |= (1 << 0);
	
}


void delay_us(unsigned int usdelay)
{
	unsigned long oldcnt,newcnt;
	unsigned long tcntvalue = 0;

	/*通过读取CNT寄存器获取当前计数值*/
	oldcnt = GPT1->CNT;
	while(1)
	{
		newcnt = GPT1->CNT;
		if(newcnt != oldcnt)
		{
			if(newcnt > oldcnt)
			{
				//正常
				tcntvalue += newcnt - oldcnt;
			}
			else
			{
				//计时器溢出
				tcntvalue += 0xffffffff - oldcnt + newcnt;
			}

			oldcnt = newcnt;
			if(tcntvalue >= usdelay) 
				break;
		}
	}
}
void delay_ms(unsigned int msdelay)
{
	int i = 0;
	for(i = 0; i < msdelay; i++)
		delay_us(1000);
}

void delay_s(unsigned int sdelay)
{
	int i = 0;
	for(i = 0; i < sdelay; i++)
		delay_ms(1000);
}
#if 0
/*中断服务函数*/
void gpt1_irqhandle(unsigned int gicciar, void * param)
{
	static unsigned char status = 0;

	if(GPT1->SR & (1 << 0))
	{
		status = !status;
		led_Trun_status(status);
	}
//清除中断标志位
	GPT1->SR |= 1 << 0;
}
#endif