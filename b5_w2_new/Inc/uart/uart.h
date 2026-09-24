#ifndef UART_H
#define UART_H

#include "stm32f103xb.h"
#include <stdint.h>

void uart_config(void);
void uartWrite(uint8_t c);
void uartWriteString(const char *str);
void uartProcessResponses(void);
#endif
