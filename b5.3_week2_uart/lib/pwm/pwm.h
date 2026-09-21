#ifndef PWM_H
#define PWM_H

#include "stm32f103xb.h"
#include <stdint.h>

void pwm_config(void);
void pwm_set_percent(uint8_t percent);
uint8_t pwm_get_percent(void);
void pwm_turn_on(void);
void pwm_turn_off(void);
uint8_t pwm_is_enabled(void);

#endif
