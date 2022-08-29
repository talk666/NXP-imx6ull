#ifndef __BPS_INT_H__
#define __BPS_INT_H__
#include "imx6ul.h"

/*定义中断处理函数*/
typedef void (*system_irq_handler_t)(unsigned int gicciar, void * param);

/*中断处理函数结构体*/
typedef struct _sys_irq_handler
{
    /* data */
    system_irq_handler_t irqHandler;   /*中断函数*/
    void *userParam;
}sys_irq_handler_t;

//初始化表
void system_irq_init(void);

/*中断初始化函数*/
void int_init(void);

/*注册中断处理函数*/
void system_register_irq_handler_t(IRQn_Type irq, system_irq_handler_t handler, void *user_param);

/*默认中断处理函数*/
void Default_system_irq_handler_t(unsigned int gicciar, void * param);

/*具体的中断处理函数*/
void system_irqhandler(unsigned int gicciar);
#endif