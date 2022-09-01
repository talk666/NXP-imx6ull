#ifndef __BPS_CLK_H__
#define __BPS_CLK_H__

#include "imx6ul.h"

void clk_enable(void);

//初始化时钟函数
void Imx6u_clkinit(void);
void Set_pll2_pfd(void);
void Set_pll3_pfd(void);
void Uart_clk_init(void);

void AHB_CLK_ROOT_Init(void);
void IPG_CLK_ROOT_Init(void);
void PERCLK_CLK_ROOT_Init(void);
#endif
