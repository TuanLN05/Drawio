#ifndef STM32F103XB_H
#define STM32F103XB_H

#include <stdint.h>

#define __IO volatile

typedef struct {
    __IO uint32_t CRL;
    __IO uint32_t CRH;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t BRR;
    __IO uint32_t LCKR;
} GPIO_TypeDef;

typedef struct {
    __IO uint32_t CR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t APB2RSTR;
    __IO uint32_t APB1RSTR;
    __IO uint32_t AHBENR;
    __IO uint32_t APB2ENR;
    __IO uint32_t APB1ENR;
    __IO uint32_t BDCR;
    __IO uint32_t CSR;
    __IO uint32_t AHBRSTR;
    __IO uint32_t CFGR2;
} RCC_TypeDef;

typedef struct {
    __IO uint32_t ACR;
    __IO uint32_t KEYR;
    __IO uint32_t OPTKEYR;
    __IO uint32_t SR;
    __IO uint32_t CR;
    __IO uint32_t AR;
    __IO uint32_t RESERVED;
    __IO uint32_t OBR;
    __IO uint32_t WRPR;
} FLASH_TypeDef;

typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SMCR;
    __IO uint32_t DIER;
    __IO uint32_t SR;
    __IO uint32_t EGR;
    __IO uint32_t CCMR1;
    __IO uint32_t CCMR2;
    __IO uint32_t CCER;
    __IO uint32_t CNT;
    __IO uint32_t PSC;
    __IO uint32_t ARR;
    __IO uint32_t RCR;
    __IO uint32_t CCR1;
    __IO uint32_t CCR2;
    __IO uint32_t CCR3;
    __IO uint32_t CCR4;
} TIM_TypeDef;

typedef struct {
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t BRR;
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t CR3;
    __IO uint32_t GTPR;
} USART_TypeDef;

typedef struct {
    __IO uint32_t CTRL;
    __IO uint32_t LOAD;
    __IO uint32_t VAL;
    __IO uint32_t CALIB;
} SysTick_Type;

#define GPIOA   ((GPIO_TypeDef *)0x40010800UL)
#define RCC     ((RCC_TypeDef *)0x40021000UL)
#define FLASH   ((FLASH_TypeDef *)0x40022000UL)
#define TIM2    ((TIM_TypeDef *)0x40000000UL)
#define USART1  ((USART_TypeDef *)0x40013800UL)
#define SysTick ((SysTick_Type *)0xE000E010UL)

#define RCC_APB1ENR_TIM2EN   (1UL << 0)
#define RCC_APB2ENR_IOPAEN   (1UL << 2)
#define RCC_APB2ENR_USART1EN (1UL << 14)
#define GPIO_BSRR_BS10       (1UL << 10)

#define TIM_CCMR1_OC1M       (7UL << 4)
#define TIM_CCMR1_OC1M_Pos   4U
#define TIM_CCMR1_OC1PE      (1UL << 3)
#define TIM_CCER_CC1E        (1UL << 0)
#define TIM_CR1_CEN          (1UL << 0)
#define TIM_CR1_ARPE         (1UL << 7)
#define TIM_EGR_UG           (1UL << 0)

#define USART_SR_TXE         (1UL << 7)
#define USART_SR_RXNE        (1UL << 5)
#define USART_CR1_RE         (1UL << 2)
#define USART_CR1_TE         (1UL << 3)
#define USART_CR1_RXNEIE     (1UL << 5)
#define USART_CR1_UE         (1UL << 13)

#define RCC_CR_HSEON         (1UL << 16)
#define RCC_CR_HSERDY        (1UL << 17)
#define RCC_CR_PLLON         (1UL << 24)
#define RCC_CR_PLLRDY        (1UL << 25)
#define RCC_CFGR_SW          (3UL << 0)
#define RCC_CFGR_SWS         (3UL << 2)
#define RCC_CFGR_HPRE        (15UL << 4)
#define RCC_CFGR_PPRE1       (7UL << 8)
#define RCC_CFGR_PPRE2       (7UL << 11)
#define RCC_CFGR_PLLSRC      (1UL << 16)
#define RCC_CFGR_PLLXTPRE    (1UL << 17)
#define RCC_CFGR_PLLMULL     (15UL << 18)
#define RCC_CFGR_HPRE_Pos    4U
#define RCC_CFGR_PLLMULL_Pos 18U
#define RCC_CFGR_HPRE_DIV1   (0UL << 4)
#define RCC_CFGR_PPRE1_DIV2  (4UL << 8)
#define RCC_CFGR_PPRE2_DIV1  (0UL << 11)
#define RCC_CFGR_PLLMULL9    (7UL << 18)
#define RCC_CFGR_SW_PLL      (2UL << 0)
#define RCC_CFGR_SWS_HSI     (0UL << 2)
#define RCC_CFGR_SWS_HSE     (1UL << 2)
#define RCC_CFGR_SWS_PLL     (2UL << 2)
#define FLASH_ACR_PRFTBE     (1UL << 4)
#define FLASH_ACR_LATENCY_2  (2UL << 0)

typedef enum {
    USART1_IRQn = 37
} IRQn_Type;

static inline void NVIC_EnableIRQ(IRQn_Type irqn)
{
    uint32_t irq_number = (uint32_t)irqn;
    uint32_t register_offset = (irq_number >> 5U) * sizeof(uint32_t);
    uint32_t bit_mask = 1UL << (irq_number & 31U);

    *(volatile uint32_t *)(0xE000E100UL + register_offset) = bit_mask;
}

static inline void NVIC_ClearPendingIRQ(IRQn_Type irqn)
{
    uint32_t irq_number = (uint32_t)irqn;
    uint32_t register_offset = (irq_number >> 5U) * sizeof(uint32_t);
    uint32_t bit_mask = 1UL << (irq_number & 31U);

    *(volatile uint32_t *)(0xE000E280UL + register_offset) = bit_mask;
}

extern uint32_t SystemCoreClock;

void SystemInit(void);
void SystemCoreClockUpdate(void);

#endif
