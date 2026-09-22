#include "pwm.h"

static volatile uint8_t configured_percent = 100U;
static volatile uint8_t led_enabled = 0U;

static void pwm_apply_output(void)
{
    uint8_t output_percent = led_enabled ? configured_percent : 0U;
    TIM2->CCR1 = ((uint32_t)(TIM2->ARR + 1U) * output_percent) / 100U;
}

void pwm_config(void)
{
    /* PA0 = TIM2_CH1, alternate-function push-pull. */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL &= ~(0xFU << 0);
    GPIOA->CRL |= (0xBU << 0);

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    /* TIM2 clock is 72 MHz; divide by 72 to get a 1 MHz counter. */
    TIM2->PSC = 71U;
    TIM2->ARR = 999U;
    TIM2->CCR1 = 0U;
    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM2->CCMR1 |= (6U << TIM_CCMR1_OC1M_Pos); /* PWM mode 1 */
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM2->CCER |= TIM_CCER_CC1E;
    TIM2->CR1 |= TIM_CR1_ARPE;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void pwm_set_percent(uint8_t percent)
{
    if (percent > 100U)
        percent = 100U;

    configured_percent = percent;
    pwm_apply_output();
}

uint8_t pwm_get_percent(void)
{
    return configured_percent;
}

void pwm_turn_on(void)
{
    led_enabled = 1U;
    pwm_apply_output();
}

void pwm_turn_off(void)
{
    led_enabled = 0U;
    pwm_apply_output();
}

uint8_t pwm_is_enabled(void)
{
    return led_enabled;
}
