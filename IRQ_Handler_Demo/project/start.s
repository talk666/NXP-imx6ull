.global _start   

@要使用中断必须将其放在最前面
_start:
@设置中断函数入口
    ldr pc, =Reset_Handler      //复位中断
    ldr pc, =Undfine_Handler    //未定义指令终端服务函数
    ldr pc, =SVC_Handler        //SVC
    ldr pc, =PreAbort_Handler   //预取终止
    ldr pc, =DataAbort_Handler  //数据终止
    ldr pc, =NotUsed_Handler    //未使用
    ldr pc, =IRQ_Handler        //IRQ中断
    ldr pc, =FIQ_Handler        //FIQ中断

@中断函数相关处理
/*复位中断服务函数*/
Reset_Handler:
    cpsid i                  /*关闭IRQ*/
    /*关闭IDcache mmu  通过SCTLR寄存器*/
    MRC p15, 0, r0, c1, c0, 0 /*读取寄存器数据到r0*/
    /*清除r0数据到r0*/
    bic r0, r0, #(1<<12)      /*关闭Icache*/
    bic r0, r0, #(1<<11)      /*关闭分支预测*/
    bic r0, r0, #(1<<2)       /*关闭Dcache*/
    bic r0, r0, #(1<<1)       /*关闭对其*/
    bic r0, r0, #(1<<0)       /*关闭MMU*/

    MCR p15, 0, r0, c1, c0, 0 /*写r0数据写到寄存器数据到*/

    /*设置中断向量偏移*/
    ldr r0, =0x87800000
    dsb
    isb
    /*设置VBAR寄存器=r0*/
    MCR p15,0,r0,c12,c0,0
    dsb
    isb

.global _bss_start
_bss_start:
    .word __bss_start

.global _bss_end
_bss_end:
    .word __bss_end

    @清除bss段  !!!!!!!!!!!!!!!!!!链接脚本设置bss段的时候没有进行计数器四字节对齐 导致清除bss段地址不正确 多清除了data数据段 导致程序卡死！！！！！
    ldr r0, _bss_start
    ldr r1, _bss_end
    mov r2, #0 
bss_loop:
    stmia r0!, {r2} //将r2赋值到r0 r0自加1
    cmp r0, r1   //比较r0 r1 
    ble bss_loop  //如果r0地址小于等于r1 跳转继续

    /*设置IRQ下的 SP*/
    mrs r0, cpsr
    bic r0, r0, #0x1f /*清除cpsr的bit0-4*/
    orr r0, r0, #0x12 // 打开IRQ
    msr cpsr, r0
    ldr sp, =0x80600000
    
    /*设置SYS下的 SP*/
    mrs r0, cpsr
    bic r0, r0, #0x1f /*清除cpsr的bit0-4*/
    orr r0, r0, #0x1f //11111 打开SYS
    msr cpsr, r0
    ldr sp, =0x80400000

    /*设置SVC下的 SP*/
    mrs r0, cpsr
    bic r0, r0, #0x1f /*清除cpsr的bit0-4*/
    orr r0, r0, #0x13 //打开SVC
    msr cpsr, r0
    ldr sp, =0x80200000

    cpsie i                  /*打开IRQ*/

    b main

/*未定义指令中断服务函数*/
Undfine_Handler:
    ldr r0, =Undfine_Handler
    bx r0

/*SVC中断服务函数*/
SVC_Handler:
    ldr r0, =SVC_Handler
    bx r0

/*预期终止中断服务函数*/
PreAbort_Handler:
    ldr r0, =PreAbort_Handler
    bx r0

/*数据终止中断服务函数*/
DataAbort_Handler:
    ldr r0, =DataAbort_Handler
    bx r0

/*未使用中断服务函数*/
NotUsed_Handler:
    ldr r0, NotUsed_Handler
    bx r0

/*IRQ中断服务函数*/
IRQ_Handler:
    push {lr}					/* 保存lr地址 */
	push {r0-r3, r12}			/* 保存r0-r3，r12寄存器 */

	mrs r0, spsr				/* 读取spsr寄存器 */
	push {r0}					/* 保存spsr寄存器 */

	mrc p15, 4, r1, c15, c0, 0 /* 从CP15的C0寄存器内的值到R1寄存器中
								* 参考文档ARM Cortex-A(armV7)编程手册V4.0.pdf P49
								* Cortex-A7 Technical ReferenceManua.pdf P68 P138
								*/							
	add r1, r1, #0X2000			/* GIC基地址加0X2000，也就是GIC的CPU接口端基地址 */
	ldr r0, [r1, #0XC]			/* GIC的CPU接口端基地址加0X0C就是GICC_IAR寄存器，
								 * GICC_IAR寄存器保存这当前发生中断的中断号，我们要根据
								 * 这个中断号来绝对调用哪个中断服务函数
								 */
	push {r0, r1}				/* 保存r0,r1 */
	
	cps #0x13					/* 进入SVC模式，允许其他中断再次进去 */
	
	push {lr}					/* 保存SVC模式的lr寄存器 */
	ldr r2, =system_irqhandler	/* 加载C语言中断处理函数到r2寄存器中*/
	blx r2						/* 运行C语言中断处理函数，带有一个参数，保存在R0寄存器中 */

	pop {lr}					/* 执行完C语言中断服务函数，lr出栈 */
	cps #0x12					/* 进入IRQ模式 */
	pop {r0, r1}				
	str r0, [r1, #0X10]			/* 中断执行完成，写EOIR */

	pop {r0}						
	msr spsr_cxsf, r0			/* 恢复spsr */

	pop {r0-r3, r12}			/* r0-r3,r12出栈 */
	pop {lr}					/* lr出栈 */
	subs pc, lr, #4				/* 将lr-4赋给pc */
	
	
/*FIQ中断服务函数*/
FIQ_Handler:
    ldr r0, =FIQ_Handler
    bx r0

