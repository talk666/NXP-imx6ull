
#include "bsp_clock.h"

/*
 * @description	: 使能I.MX6U所有外设时钟
 * @param 		: 无
 * @return 		: 无
 */
void clk_enable(void)
{
	CCM->CCGR0 = 0XFFFFFFFF;
	CCM->CCGR1 = 0XFFFFFFFF;
	CCM->CCGR2 = 0XFFFFFFFF;
	CCM->CCGR3 = 0XFFFFFFFF;
	CCM->CCGR4 = 0XFFFFFFFF;
	CCM->CCGR5 = 0XFFFFFFFF;
	CCM->CCGR6 = 0XFFFFFFFF;
}

//初始化时钟函数  pll1  给cpu提供时钟   基于外部24M晶振进行倍频操作 Fin = 24MHZ
void Imx6u_clkinit(void)
{
	if(((CCM->CCSR >> 2) & 0x1) == 0)//也就是说pll_sw_clk手使用pll1_main_clk来源将其修改为晶振暂时
	{
		//当前时钟选择的pll1时钟来源改为外部晶振24M   改为1为step_clk
		CCM->CCSR &= ~(1<<8);//bit8置0 step_clk = osc_in = 24M
		CCM->CCSR |= (1<<2);//pll_sw_clk=step_clk = osc_in = 24M
	}

	//设置CCM_ANALOG->PLL_ARM寄存器 修改PLL1的初始频率 ->696Mhz   fout = fin*div_select/2.0     //div_select为设置系数  fout为输出频率
	CCM_ANALOG->PLL_ARM =( (1<<13) | ((58 << 0) & 0x7f));
	CCM->CACRR = 0; //设置一分频
	CCM->CCSR &= ~(1<<2);//设置pll_sw_clk = pll1_main_clk = 696/1 = 696

	//设置pll2/pll3 的pfd0-3
	Set_pll2_pfd();
	Set_pll3_pfd();
	
	AHB_CLK_ROOT_Init();
	IPG_CLK_ROOT_Init();
	PERCLK_CLK_ROOT_Init();

	/*使能串口时钟*/
	Uart_clk_init();
}

//CCM_ANALOG_PFD_528n 设置pll2的4路fd时钟
//比如 pfd0_FPAC = 528*18/PFD0_FRAC    设置PFD0_FRAC   例如352 = 528*18/PFD0_FRAC  PFD0_FRAC= 27
void Set_pll2_pfd(void)
{
	unsigned int reg = 0;
	reg = CCM_ANALOG->PFD_528;

	reg &= ~(0x3f3f3f3f);//bit0-5   8-13  46-21  24-29   寄存器CCM_ANALOG_PFD_528n
	reg |= (32<<24);     //初始化pll2 pfd3 = 297MHZ     297 = 528*18/PFD0_FRAC  PFD0_FRAC= 32     bit24-29
	reg |= (24<<16);     //初始化pll2 pfd2 = 396MHZ     396 = 528*18/PFD0_FRAC  PFD0_FRAC= 24     bit16-21
	reg |= (16<<8);     //初始化pll2 pfd1 = 597MHZ     597 = 528*18/PFD0_FRAC  PFD0_FRAC= 16	  bit8-13
	reg |= (27<<0);     //初始化pll2 pfd0 = 352MHZ     352 = 528*18/PFD0_FRAC  PFD0_FRAC= 27     bit0-5
	
	CCM_ANALOG->PFD_528 = reg;
}

void Set_pll3_pfd(void)
{
	unsigned int reg = 0;
	reg = CCM_ANALOG->PFD_480;

	reg &= ~(0x3f3f3f3f);//bit0-5   8-13  46-21  24-29   寄存器CCM_ANALOG_PFD_480n
	reg |= (19<<24);     //初始化pll3 pfd3 = 454.7MHZ     454.7 = 480*18/PFD0_FRAC  PFD0_FRAC= 19     bit24-29
	reg |= (17<<16);     //初始化pll3 pfd2 = 508.2MHZ     508.2 = 480*18/PFD0_FRAC  PFD0_FRAC= 24     bit16-21
	reg |= (16<<8);     //初始化pll3 pfd1 = 540MHZ     540 = 480*18/PFD0_FRAC  PFD0_FRAC= 16	  bit8-13
	reg |= (12<<0);     //初始化pll3 pfd0 = 720MHZ     720 = 480*18/PFD0_FRAC  PFD0_FRAC= 27     bit0-5
	CCM_ANALOG->PFD_480 = reg;
}

//AHB_CLK_ROOT  IPG_CLK_ROOT PERCLK_CLK_ROOT 初始化
//132Hz
void AHB_CLK_ROOT_Init(void)
{
	//CCM_CBCMR bit18-19 ->PLL2 PFD2  132MHZ
	//清零
	CCM->CBCMR &= ~(3<<18);
	//设置bit18-19
	CCM->CBCMR |= (1<<18);

	//CCM_CBCDR  bit25  0
	//清零 	//设置bit18-19
	CCM->CBCDR &= ~(1<<25);
	while (CCM->CDHIPR & (1<<5));//等待握手信号完成
#if 0
	// bit 10-12 	//清零
	CCM->CBCDR &= ~(7<<10);
	CCM->CBCDR |= (2<<10); //三分频  396/3=132M   pfd2 = 396MHZ
	while (CCM->CDHIPR & (1<<1));//等待握手信号完成
#endif
}

//66Hz
void IPG_CLK_ROOT_Init(void)
{
	//CCM_CBCDR bit8-9 清零  分频设置
	CCM->CBCDR &= ~(3<<8);
	CCM->CBCDR |= (1<<8); //2分频  用的AHB_CLK_ROOT_Init = 132/2 = 66hz
}

//66Hz
void PERCLK_CLK_ROOT_Init(void)
{
	//CCM_CSCMR1 bit6 清零加 设置来源为IPG_CLK_ROOT_Init  bit6正好为0选择ipg时钟
	CCM->CSCMR1 &= ~(1<<6);
	//CCM_CSCMR1 bit0-5 清零  分频设置
	CCM->CSCMR1 &= ~(0x3f<<0);
	//CCM->CSCMR1 |= (0<<0); //1分频 不用写  IPG_CLK_ROOT_Init = 66/1 = 66hz
}

/*配置uart根时钟源*/
void Uart_clk_init(void)
{
	CCM->CSCDR1 &= ~(1 << 6);/*uart_clk = pll3_clk = 480M  进行6分频后为80M*/
	CCM->CSCDR1 &= ~0x3f;/*一分频*/
}