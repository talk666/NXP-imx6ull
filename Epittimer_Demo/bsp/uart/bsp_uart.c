#include "bsp_uart.h"

/*初始化uart1*/
void uart1_init(void)
{
    /*初始化IO*/
    uart_io_init();

    /*初始化uart1*/
    disable_uart1(UART1);
    uart_softset(UART1);

    //配置uart1
    UART1->UCR1 = 0;
    UART1->UCR1 &= ~(1<<14); /* 关闭自动波特率检测 */
    
    //Receiver、Transmitter Enable
    UART1->UCR2 &= ~(3 << 1);
    UART1->UCR2 |= (3 << 1);
    //8-bit transmit
    UART1->UCR2 |= (1 << 5);
    //The transmitter sends 1 stop bit
    UART1->UCR2 &= ~(1 << 6);
    //奇偶校验关闭默认
    //Ignore the RTS pin
    UART1->UCR2 |= (1 << 14);

    // In this chip, UARTs are used in MUXED mode, so that this bit should always be set
    UART1->UCR3 |= (1 << 2);

    /*波特率设置*/
    UART1->UFCR &= ~(0x7 << 7); //清零7-9bit 111
    UART1->UFCR |= (0x5 << 7);//设置一分频 101 uart1_clk = 80M

    UART1->UBIR = 71;
    UART1->UBMR = 3124;

    /*baudrate = 频率/(16*((ubMR+1)/(ubir+1)))
    115200 = 80000000/(16*((3124+1)/(71+1)))
    */

    enable_uart1(UART1);
}

void disable_uart1(UART_Type *base)
{
    /*关闭串口*/
    base->UCR1 &= ~(1 << 0);
}

void enable_uart1(UART_Type *base)
{
    /*开启串口*/
    base->UCR1 &= ~(1 << 0);
    base->UCR1 |= (1 << 0);
}

void uart_softset(UART_Type *base)
{
    /*软件复位*/
    base->UCR2 &= ~(1 << 0);
    /*复位等待*/
    //while ((base->UTS & 0x01) == 0);
    while((base->UCR2 & 0x1) == 0); /* 等待复位完成 */
    
}

void uart_io_init(void)
{
    /* 1、初始化IO复用 */
	IOMUX_SW_MUX->UART1_TX_DATA = 0;/* 复用为Select mux mode: ALT0 mux port: UART1_TX of instance: uart1 */
	IOMUX_SW_MUX->UART1_RX_DATA = 0;/* 复用为Select mux mode: ALT0 mux port: UART1_RX of instance: uart1 */
	
    
    /*注意！！！！！需要进行配置输入引脚？？？不配置这里不会接收信息*/
    //11 UART1_RX_DATA_ALT0 — Selecting Pad: UART1_RX_DATA for Mode: ALT0
    UART1_RX_DATA_SELECT_INPUT = 0x3;

	/* 2、配置uart1/uart2的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 00 默认下拉
     *bit [13]: 0 kepper功能
     *bit [12]: 1 pull/keeper使能
     *bit [11]: 0 关闭开路输出
     *bit [7:6]: 10 速度100Mhz
     *bit [5:3]: 110 R0/6驱动能力
     *bit [0]: 0 低转换率
     */
    IOMUX_SW_PAD->UART1_TX_DATA = 0X10B0;//0001 0000 1011 0000
    IOMUX_SW_PAD->UART1_RX_DATA = 0X10B0;
}



/*通过UART1发送一个字符*/
void putc(unsigned char c)
{
    /*判断串口1是否发送完上次数据*/
    while (((UART1->USR2 >> 3) & 0x1) == 0);

    /*填充数据准备发送*/
    //UART1->UTXD = 0;  ?????为什么加上就乱码
    UART1->UTXD = c & 0xff;

}

/*通过串口接收数据*/
unsigned char getc(void)
{
    /*判断是否有数据读取*/
    while ((UART1->USR2 & 0x1) == 0);//Receive Data Ready
    return UART1->URXD;
}

/*通过串口发送一串字符*/
void puts(char *str)
{
    char *p = str;
    while(*p)
    {
        putc(*p);
        p++;
    }
}