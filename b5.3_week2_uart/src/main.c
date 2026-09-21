#include <stm32f103xb.h>
#include <stdint.h>
#include "uart.h"
#include "pwm.h"
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
	pwm_config();
	uart_config();
	uartWriteString("Commands: ON!, OFF!, PWM:<0..100>%!, Status!\r\n");

	while(1)
	{
		/* Command parsing and PWM updates occur in USART1_IRQHandler. */
		uartProcessResponses();
	}
}
