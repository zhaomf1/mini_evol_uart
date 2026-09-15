#include "pwm_output.h"

#include "tim.h"
#include "pwm_timer_config.h"

static uint8_t pwm_active;

void PWM_OutputInit(void)
{
    /* TIM2 is reserved exclusively as the 32-bit pulse counter.
       No TIM2 channel pin is enabled (in particular PA5/STEP2). */
    TIM2->CR1 = 0U;
    TIM2->DIER = 0U;
    TIM2->SMCR = 0U;
    TIM2->CCER = 0U;
    TIM4->CR1 = 0U;
    TIM4->DIER = 0U;
    TIM4->SMCR = 0U;
    /* CH2 forced inactive, active-high polarity: PD13 is driven LOW. */
    TIM4->CCMR1 = TIM_OCMODE_FORCED_INACTIVE << 8U;
    TIM4->CCMR2 = 0U;
    TIM4->CCER = TIM_CCER_CC2E;
    pwm_active = 0U;
}

uint8_t PWM_OutputBusy(void)
{
    return (uint8_t)((pwm_active != 0U) && (TIM2->CNT < TIM2->CCR2));
}

uint32_t PWM_OutputStart(uint32_t frequency_hz, uint32_t pulse_count)
{
    PWM_TimerConfig_t config;
    volatile uint32_t settle;

    if ((pulse_count == 0U) || (pulse_count > 1000000U) ||
        (PWM_TimerConfigGet(frequency_hz, &config) != PWM_TIMER_CONFIG_OK) ||
        (PWM_OutputBusy() != 0U))
    {
        return 0U;
    }

    PWM_OutputInit();
    TIM4->CR2 = TIM_TRGO_RESET;
    TIM4->PSC = config.prescaler;
    TIM4->ARR = config.auto_reload;
    TIM4->CCR2 = ((uint32_t)config.auto_reload + 1U) / 2U;
    TIM4->CNT = 0U;
    TIM4->EGR = TIM_EGR_UG;
    /* PWM2: low first, high second, falling edge at overflow.
       Counting overflow therefore counts COMPLETE high pulses. */
    TIM4->CCMR1 = (TIM_OCMODE_PWM2 << 8U) | TIM_CCMR1_OC2PE;

    TIM2->CR2 = TIM_TRGO_OC2REF;
    TIM2->PSC = 0U;
    TIM2->ARR = 0xFFFFFFFFU;
    TIM2->CCR2 = pulse_count;
    TIM2->CNT = 0U;
    TIM2->CCMR1 = TIM_OCMODE_PWM1 << 8U;
    TIM2->CCMR2 = 0U;
    TIM2->EGR = TIM_EGR_UG;
    /* OC2REF = (CNT < requested count). No GPIO output is required.
       RM0090 table 97: TIM2 ITR3 <- TIM4; TIM4 ITR1 <- TIM2.
       Keep MSM disabled: each trigger has its own independent role. */
    TIM2->SMCR = TIM_TS_ITR3;
    TIM4->SMCR = TIM_TS_ITR1;
    TIM4->CR2 = TIM_TRGO_UPDATE;
    /* Let the initialization UG pulse and trigger muxes settle while
       both counters are disabled, so UG is never counted as a pulse. */
    __DSB();
    for (settle = 0U; settle < 32U; ++settle)
    {
        __NOP();
    }
    TIM2->SR = 0U;
    TIM4->SR = 0U;
    TIM2->SMCR |= TIM_SLAVEMODE_EXTERNAL1;
    TIM4->SMCR |= TIM_SLAVEMODE_GATED;
    pwm_active = 1U;
    TIM2->CR1 = TIM_CR1_CEN;
    __DSB();
    TIM4->CR1 = TIM_CR1_CEN;
    /* At the Nth overflow, PD13 first falls LOW, TIM2 then reaches N,
       OC2REF closes the gate. TIM4 freezes in its low half-period.
       At 100 kHz that half-period is 5 us, providing ample time for
       the internal trigger synchronization. No stop ISR/DMA is used.
       CEN intentionally remains set after completion; Busy reads CNT. */
    return config.actual_frequency_hz;
}
