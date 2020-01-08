#ifndef __USART_H
#define __USART_H
#include "stdio.h"	

void uart_init(unsigned int bound);
void USART1_PrintI(long int data,unsigned char newline);
void USART1_PrintF(double data,unsigned char precision,unsigned char newline);
#endif


