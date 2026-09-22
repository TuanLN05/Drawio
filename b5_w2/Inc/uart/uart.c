#include "uart.h"
#include "pwm.h"

#define UART_RX_BUFFER_SIZE 32U
#define RESPONSE_QUEUE_SIZE 8U

typedef enum
{
    RESPONSE_OK,
    RESPONSE_STATUS,
    RESPONSE_ERROR
} response_type_t;

typedef struct
{
    response_type_t type;
    uint8_t percent;
    uint8_t enabled;
} response_t;

/* ISR stores every received byte here until it receives '!'. */
static char rx_buffer[UART_RX_BUFFER_SIZE];
static uint8_t rx_length = 0U;
static uint8_t rx_overflow = 0U;

static volatile response_t response_queue[RESPONSE_QUEUE_SIZE];
static volatile uint8_t response_head = 0U;
static volatile uint8_t response_tail = 0U;

static const char command_on[] = "ON";
static const char command_off[] = "OFF";
static const char command_status[] = "Status";

static uint8_t buffer_equals(const char *buffer, const char *command)
{
    uint8_t i = 0U;

    while ((buffer[i] != '\0') && (command[i] != '\0'))
    {
        if (buffer[i] != command[i])
            return 0U;
        i++;
    }

    return ((buffer[i] == '\0') && (command[i] == '\0')) ? 1U : 0U;
}

static void queue_response(response_type_t type, uint8_t percent, uint8_t enabled)
{
    uint8_t next_head = (uint8_t)((response_head + 1U) % RESPONSE_QUEUE_SIZE);

    if (next_head != response_tail)
    {
        response_queue[response_head].type = type;
        response_queue[response_head].percent = percent;
        response_queue[response_head].enabled = enabled;
        response_head = next_head;
    }
}

static uint8_t parse_pwm_percent(const char *buffer,
                                 uint8_t length,
                                 uint8_t *percent)
{
    uint16_t value = 0U;
    uint8_t i;

    /* Minimum valid command is PWM:0%. */
    if ((length < 6U) ||
        (buffer[0] != 'P') ||
        (buffer[1] != 'W') ||
        (buffer[2] != 'M') ||
        (buffer[3] != ':') ||
        (buffer[length - 1U] != '%'))
    {
        return 0U;
    }

    /* Extract the decimal characters between ':' and '%'. */
    for (i = 4U; i < (length - 1U); i++)
    {
        if ((buffer[i] < '0') || (buffer[i] > '9'))
            return 0U;

        value = (uint16_t)(value * 10U + (uint16_t)(buffer[i] - '0'));
        if (value > 100U)
            return 0U;
    }

    *percent = (uint8_t)value;
    return 1U;
}

/* The ISR calls this as soon as it receives the terminating '!'. */
static void process_rx_buffer(void)
{
    uint8_t percent;

    if (buffer_equals(rx_buffer, command_on))
    {
        pwm_turn_on();
        queue_response(RESPONSE_OK, pwm_get_percent(), pwm_is_enabled());
    }
    else if (buffer_equals(rx_buffer, command_off))
    {
        pwm_turn_off();
        queue_response(RESPONSE_OK, pwm_get_percent(), pwm_is_enabled());
    }
    else if (buffer_equals(rx_buffer, command_status))
    {
        queue_response(RESPONSE_STATUS, pwm_get_percent(), pwm_is_enabled());
    }
    else if (parse_pwm_percent(rx_buffer, rx_length, &percent))
    {
        pwm_set_percent(percent);
        queue_response(RESPONSE_OK, pwm_get_percent(), pwm_is_enabled());
    }
    else
    {
        queue_response(RESPONSE_ERROR, 0U, 0U);
    }
}

static void reset_rx_buffer(void)
{
    rx_length = 0U;
    rx_overflow = 0U;
}

void USART1_IRQHandler(void)
{
    uint32_t status = USART1->SR;

    if ((status & USART_SR_RXNE) != 0U)
    {
        uint8_t received_byte = (uint8_t)USART1->DR;

        if (received_byte == '!')
        {
            if ((rx_length > 0U) && (rx_overflow == 0U))
            {
                rx_buffer[rx_length] = '\0';
                process_rx_buffer();
            }
            else
            {
                queue_response(RESPONSE_ERROR, 0U, 0U);
            }

            reset_rx_buffer();
        }
        else if ((received_byte == '\r') || (received_byte == '\n'))
        {
            /* A non-empty line without a terminating '!' is invalid. */
            if ((rx_length > 0U) || (rx_overflow != 0U))
                queue_response(RESPONSE_ERROR, 0U, 0U);

            reset_rx_buffer();
        }
        else if ((received_byte == '\b') || (received_byte == 0x7FU))
        {
            if (rx_length > 0U)
                rx_length--;
        }
        else if ((rx_overflow == 0U) &&
                 (rx_length < (UART_RX_BUFFER_SIZE - 1U)))
        {
            rx_buffer[rx_length] = (char)received_byte;
            rx_length++;
        }
        else
        {
            /* Wait for '!' or CR/LF before reporting/resetting the command. */
            rx_overflow = 1U;
        }
    }
}

void uart_config(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    /* PA9: USART1 TX, alternate-function push-pull. */
    GPIOA->CRH &= ~(0xFU << 4);
    GPIOA->CRH |= (0xBU << 4);

    /* PA10: USART1 RX, input pull-up. */
    GPIOA->CRH &= ~(0xFU << 8);
    GPIOA->CRH |= (0x8U << 8);
    GPIOA->BSRR = GPIO_BSRR_BS10;

    USART1->CR1 = 0U;
    USART1->BRR = 7500U; /* 72 MHz / 9600 baud, oversampling by 16. */
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE |
                  USART_CR1_RE | USART_CR1_RXNEIE;

    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART1_IRQn);
}

void uartWrite(uint8_t c)
{
    while ((USART1->SR & USART_SR_TXE) == 0U)
    {
    }
    USART1->DR = c;
}

void uartWriteString(const char *str)
{
    while (*str != '\0')
        uartWrite((uint8_t)*str++);
}

static void uartWritePercent(uint8_t percent)
{
    if (percent >= 100U)
        uartWrite('1');
    if (percent >= 10U)
        uartWrite((uint8_t)('0' + ((percent / 10U) % 10U)));
    uartWrite((uint8_t)('0' + (percent % 10U)));
}

void uartProcessResponses(void)
{
    response_t response;

    while (response_tail != response_head)
    {
        response.type = response_queue[response_tail].type;
        response.percent = response_queue[response_tail].percent;
        response.enabled = response_queue[response_tail].enabled;
        response_tail = (uint8_t)((response_tail + 1U) % RESPONSE_QUEUE_SIZE);

        if (response.type == RESPONSE_ERROR)
        {
            uartWriteString("ERROR: invalid command\r\n");
        }
        else if (response.type == RESPONSE_STATUS)
        {
            uartWriteString("Status: ");
            uartWriteString(response.enabled ? "ON, PWM:" : "OFF, PWM:");
            uartWritePercent(response.percent);
            uartWriteString("%\r\n");
        }
        else
        {
            uartWriteString("OK, Status: ");
            uartWriteString(response.enabled ? "ON, PWM:" : "OFF, PWM:");
            uartWritePercent(response.percent);
            uartWriteString("%\r\n");
        }
    }
}
