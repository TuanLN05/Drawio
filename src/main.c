#include <stm32f103xb.h>
#include <stdint.h>
#include "uart_nvic_cfg.h"
void delay(uint32_t time)
{
	while(time)
	{
		SysTick -> LOAD = 72000 - 1;
		SysTick -> CTRL = 5;
		SysTick -> VAL = 0;
		while(!(SysTick -> CTRL & (1 << 16)));
		time--;
	}
}

int main(void)
{
	uart_config();
	while(1)
	{


		uartWrite('o');
		for(volatile int i = 0; i < 1000000; i++);
	}
}