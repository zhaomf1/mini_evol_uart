#ifndef PWM_OUTPUT_H
#define PWM_OUTPUT_H

#include <stdint.h>

void PWM_OutputInit(void);
uint8_t PWM_OutputBusy(void);
/* Returns the actual frequency, or zero if invalid/busy. */
uint32_t PWM_OutputStart(uint32_t frequency_hz, uint32_t pulse_count);

#endif
