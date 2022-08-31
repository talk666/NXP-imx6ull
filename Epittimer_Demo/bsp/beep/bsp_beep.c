#include "bsp_beep.h"

void Beep_Init(void)
{
    //gpio5-io01
    SW_MUX_CTL_PAD_SNVS_TAMPER1 = 0x5;

    //配置GPIO电气属性
    	/* 2、配置GPIO5_IO01的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 00 默认下拉
     *bit [13]: 0 kepper功能
     *bit [12]: 1 pull/keeper使能
     *bit [11]: 0 关闭开路输出
     *bit [7:6]: 10 速度100Mhz
     *bit [5:3]: 110 R0/6驱动能力
     *bit [0]: 0 低转换率
     */
    SW_PAD_CTL_PAD_SNVS_TAMPER1 = 0x10B0;

    GPIO5->GDIR |= (1<<1);//bit1 set 1  10输出模式
    GPIO5->DR |= (1<<1);//bit1 set 1 为关闭蜂鸣器
}

void Beep_On(void)
{
    GPIO5->DR &= ~(1<<1);//10--01
}

void Beep_Off(void)
{
    GPIO5->DR |= (1<<1);
}

void Beep_Trun(int status)
{
    if(status)//开蜂鸣器
        Beep_On();
    else   //关蜂鸣器
        Beep_Off();
}