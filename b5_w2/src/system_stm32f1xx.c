#include "stm32f103xb.h"

#define HSE_VALUE 8000000U
#define HSI_VALUE 8000000U
#define SYSCLK_HZ 72000000U

uint32_t SystemCoreClock = SYSCLK_HZ;

const uint8_t AHBPrescTable[16U] = {
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U
};

void SystemInit(void)
{
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;

    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0U) {
    }

    RCC->CFGR &= ~(RCC_CFGR_SW | RCC_CFGR_HPRE | RCC_CFGR_PPRE1 |
                   RCC_CFGR_PPRE2 | RCC_CFGR_PLLSRC |
                   RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 |
                 RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PLLSRC |
                 RCC_CFGR_PLLMULL9;

    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0U) {
    }

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
    }

    SystemCoreClock = SYSCLK_HZ;
}

void SystemCoreClockUpdate(void)
{
    uint32_t system_clock;
    uint32_t pll_multiplier;

    switch (RCC->CFGR & RCC_CFGR_SWS) {
    case RCC_CFGR_SWS_HSE:
        system_clock = HSE_VALUE;
        break;

    case RCC_CFGR_SWS_PLL:
        pll_multiplier =
            ((RCC->CFGR & RCC_CFGR_PLLMULL) >> RCC_CFGR_PLLMULL_Pos) + 2U;
        system_clock = HSE_VALUE * pll_multiplier;
        if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) != 0U)
            system_clock /= 2U;
        break;

    case RCC_CFGR_SWS_HSI:
    default:
        system_clock = HSI_VALUE;
        break;
    }

    SystemCoreClock = system_clock >>
        AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos];
}

void __libc_init_array(void)
{
}
