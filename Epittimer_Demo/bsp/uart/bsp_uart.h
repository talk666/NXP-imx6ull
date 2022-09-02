#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#include "imx6ul.h"
#include "bsp_gpio.h"

//DAISY Register ????
#define UART1_RX_DATA_SELECT_INPUT        (*(volatile unsigned int *)0x20E0624)

void uart1_init(void);

void uart_io_init(void);

void uart_softset(UART_Type *base);

void disable_uart1(UART_Type *base);

void enable_uart1(UART_Type *base);


void puts(char *str);

unsigned char getc(void);

void putc(unsigned char c);

#endif