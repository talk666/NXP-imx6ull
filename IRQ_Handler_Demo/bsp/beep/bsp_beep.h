#ifndef __BPS_BEEP_H__
#define __BPS_BEEP_H__

#include "imx6ul.h"

#define SW_MUX_CTL_PAD_SNVS_TAMPER1  *((volatile unsigned long *)0x229000C)

#define SW_PAD_CTL_PAD_SNVS_TAMPER1  *((volatile unsigned long *)0x2290050)

// 0110100010110000 0x68B0

void Beep_Init(void);
void Beep_On(void);
void Beep_Off(void);
void Beep_Trun(int status);

#endif
