#include "bsp_spi.h"

/*spi初始化*/
void spi_init(ECSPI_Type *base)
{
    base->CONREG = 0;//清零
    base->CONREG |= (1 << 0);//SPI enable 
    base->CONREG |= (1 << 3); /*Immediately starts a SPI burst when data is written in TXFIFO.*/    
    base->CONREG |= (1 << 4);/*master mode*/
    /*bit18-19 0默认为 channel 0*/
    base->CONREG |= (7 << 20);/*Number of Valid Bits in a SPI burs*/
    base->CONREG |= (9 << 12);    /*clock setting bit8-11(0 1分频) 12-15(1010 10分频 60M/10=6Mhz) set clock for Spi*/

    base->CONFIGREG = 0; /*默认 pol pha*/

    /*  
     * ECSPI通道0设置，设置采样周期
     * bit[14:0] :	0X2000  采样等待周期，比如当SPI时钟为10MHz的时候
     *  		    0X2000就等于1/10000 * 0X2000 = 0.8192ms，也就是连续
     *          	读取数据的时候每次之间间隔0.8ms
     * bit15	 :  0  采样时钟源为SPI CLK
     * bit[21:16]:  0  片选延时，可设置为0~63
	 */
	base->PERIODREG = 0X2000;		/* 设置采样周期寄存器 */

}

/*SPI 发送 接收接口*/
unsigned char spi0_readwrite_byte(ECSPI_Type *base, unsigned char txdata)
{
    uint32_t spirxdate = 0;
    uint32_t spitxdate = txdata;

    /*bit18-19 0默认为 channel 0   在初始化的时候已经重置0默认channel 0 此处可以不写*/
    base->CONREG &= ~(3 << 18);
    base->CONREG |= (0 << 18);

    //数据发送 This bit is set if the TXFIFO is empty
    while(!(base->STATREG & (1 << 0)));
    base->TXDATA = spitxdate;

    /*数据接收  RXFIFO Ready.*/
    while(!(base->STATREG & (1 << 3)));
    spirxdate = base->RXDATA;

    return spirxdate;
}
