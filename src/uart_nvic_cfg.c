#include "uart_nvic_cfg.h"


void USART1_IRQHandler(void)
{
    uint32_t status = USART1->SR;

    if (status & USART_SR_RXNE)
    {
        uint8_t c = (uint8_t)USART1->DR;

        if (c == 'H' || c == 'h')
        {
            GPIOC->BSRR = GPIO_BSRR_BR13;
        }
        else if (c == 'L' || c == 'l')
        {
            GPIOC->BSRR = GPIO_BSRR_BS13;
        }
    }
}
void uart_config(void)
{
    RCC->APB2ENR |= 1 << 2 | 1 << 4 | 1 << 14; // Enable GPIOA, GPIOC, and USART1 clocks

    /* PC13: output push-pull, 2 MHz */
    GPIOC->CRH &= ~(0xFU << 20);
    GPIOC->CRH |= (0x2U << 20);
    GPIOC->BSRR = GPIO_BSRR_BS13;

    /* PA9: USART1 TX, alternate-function push-pull */
    GPIOA->CRH &= ~(0xFU << 4);
    GPIOA->CRH |= (0xBU << 4);

    /* PA10: USART1 RX, input pull-up */
    GPIOA->CRH &= ~(0xFU << 8);
    GPIOA->CRH |= (0x8U << 8);
    GPIOA->BSRR = GPIO_BSRR_BS10; // led off

    USART1->CR1 = 0;
    USART1->BRR = 833; // 8MHz / 9600 baud

    USART1->CR1 = 1 << 13 | 1 << 3 | 1 << 2| 1 << 5; // Enable USART, RXNE interrupt, receiver, and transmitter

   NVIC_ClearPendingIRQ(USART1_IRQn); // xóa trạng thái "đang chờ xử lý" của một interrupt (ngắt) trong NVIC
   NVIC_EnableIRQ(USART1_IRQn);
}
void uartWrite(uint8_t c)
{
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}
