#include "bsp_int.h"

//中断嵌套计数
static unsigned int irq_Nesting;

//中断处理函数表
static sys_irq_handler_t irqTable[NUMBER_OF_INT_VECTORS];

//中断初始化
void int_init(void)
{
    //初始化GIC
    GIC_Init();

    //初始化表
    system_irq_init();

    //中断向量表设置
    __set_VBAR((uint32_t)0x87800000);

}

//初始化表
void system_irq_init(void)
{
    unsigned int i = 0;
    
    irq_Nesting = 0;

    for(i = 0; i < NUMBER_OF_INT_VECTORS; i++)
    {
        irqTable[i].irqHandler = Default_system_irq_handler_t;
        irqTable[i].userParam = NULL;
    }
}



/*注册中断处理函数*/
void system_register_irq_handler_t(IRQn_Type irq, system_irq_handler_t handler, void *user_param)
{   

    irqTable[irq].irqHandler = handler;
    irqTable[irq].userParam = user_param;
}

/*默认中断处理函数*/
void Default_system_irq_handler_t(unsigned int gicciar, void * param)
{
    while(1);
}

/*具体的中断处理函数*/
void system_irqhandler(unsigned int gicciar)
{
    unsigned int intNum = gicciar & 0x3ff;//11 1111 1111
    //检查中断ID是否正常
    if(intNum > 160) //imx6ull芯片最多160个中断  从32开始给用户提供
    {
        return;
    }

    irq_Nesting++;

    /*根据中断号执行对应的回调函数*/
    irqTable[intNum].irqHandler(intNum, irqTable[intNum].userParam);

    irq_Nesting--;
}