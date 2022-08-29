#include "bsp_key_zero.h"
#include "bsp_delay.h"
#include <stdio.h>
void Key_Init(void)
{
    gpio_pin_config gpio_pin_config_t;

    //复用为gpio  为gpio1-io18
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
    //接口初始化
    gpio_pin_config_t.direction = KGPIO_DigitalInput;
    gpio_init(GPIO1, 18, &gpio_pin_config_t);
#endif

}

//获取按键值状态
int read_key(void)
{
    int ret = 0;

#if IFCONFIG_INTERFACE
    ret = (GPIO1->DR)>>18 & 0x1;
#else
    ret = gpio_pinread(GPIO1, 18);
#endif
    return ret;
}

int key_getvalue(void)
{
    int ret = 0;
    static unsigned char release  = 1;//1为释放

    if((release == 1) && (read_key() == 0))
    {
        release  = 0;
        delay(10); //消除抖动
        if(read_key() == 0)
        {
            ret = KEY0_VALUE;
        }
    }else  if(read_key() == 1){
        release  = 1;
        ret = KEY_NONE;
    }

    return ret;
}