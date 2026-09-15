# TIM1 Counted PWM Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Drive J3/PE9/TIM1_CH1 with exactly the requested number of continuous PWM periods from UART1 commands of the form `PWM <1..100>,<1..1000000>`.

**Architecture:** Parse frequency and pulse count into a command struct. TIM1_CH1 generates the waveform; TIM1 RCR produces 256-period update boundaries, while a small state machine preloads the final remainder and enables one-pulse mode so the final boundary stops the timer in hardware. The TIM1 update interrupt only prepares the next segment at least one segment before it is used; it never counts individual PWM periods.

**Tech Stack:** STM32F405 HAL, TIM1 advanced timer, UART1 interrupt reception, FreeRTOS task command processing, Keil MDK.

**Spec:** Conversation-approved design on 2026-09-14: only `PWM <frequency_kHz>,<count>`; frequency 1..100 kHz; count 1..1,000,000; PE9/J3 output; continuous waveform; output low when complete; no completion response.

## Global Constraints

- J3 PWM output is PE9 / TIM1_CH1; PD13/TIM4 is no longer the command-controlled PWM output.
- Every command starts a finite burst and returns only a launch acknowledgement.
- The PWM frequency is 1..100 kHz and duty cycle is 50%.
- A burst must contain exactly the requested count without segment gaps.
- After the final period, PE9 must be low.
- Maximum count is 1,000,000 and must not allocate a per-period RAM buffer.

---

### Task 1: Parse finite PWM commands

**Files:**
- Modify: `Core/Inc/app/pwm_command.h`
- Modify: `Core/Src/app/pwm_command.c`
- Modify: `tests/test_pwm_command.c`

**Interfaces:**
- Produces `PWM_Command_t { uint32_t frequency_hz; uint32_t pulse_count; }`.
- Produces `PWM_CommandParse(const char *command, PWM_Command_t *parsed_command)`.

- [ ] **Step 1: Write failing parser tests**

```c
PWM_Command_t command;
assert(PWM_CommandParse("PWM 100,10000\r\n", &command) == PWM_COMMAND_OK);
assert(command.frequency_hz == 100000U);
assert(command.pulse_count == 10000U);
assert(PWM_CommandParse("PWM 101,1\n", &command) == PWM_COMMAND_RANGE_ERROR);
assert(PWM_CommandParse("PWM 1,1000001\n", &command) == PWM_COMMAND_RANGE_ERROR);
assert(PWM_CommandParse("PWM 100\n", &command) == PWM_COMMAND_FORMAT_ERROR);
```

- [ ] **Step 2: Run the parser test and verify the new command test fails because the old parser has no count field.**

- [ ] **Step 3: Implement decimal parsing for frequency, comma, and count.**

```c
typedef struct
{
    uint32_t frequency_hz;
    uint32_t pulse_count;
} PWM_Command_t;
```

Reject missing commas, non-digits, trailing data, frequency outside 1..100 kHz, and count outside 1..1,000,000.

- [ ] **Step 4: Re-run parser tests and verify all valid and invalid cases pass.**

### Task 2: Unit-test RCR segment scheduling

**Files:**
- Create: `Core/Inc/app/pwm_burst_plan.h`
- Create: `Core/Src/app/pwm_burst_plan.c`
- Create: `tests/test_pwm_burst_plan.c`

**Interfaces:**
- Produces `PWM_BurstPlan_t` with `full_segments`, `final_segment_pulses`, and `final_segment_is_needed`.
- Produces `PWM_BurstPlanCreate(uint32_t pulse_count, PWM_BurstPlan_t *plan)`.

- [ ] **Step 1: Write failing segment tests**

```c
PWM_BurstPlan_t plan;
assert(PWM_BurstPlanCreate(10000U, &plan));
assert(plan.full_segments == 39U);
assert(plan.final_segment_pulses == 16U);
assert(PWM_BurstPlanCreate(256U, &plan));
assert(plan.full_segments == 1U);
assert(plan.final_segment_pulses == 0U);
assert(PWM_BurstPlanCreate(1U, &plan));
assert(plan.full_segments == 0U);
assert(plan.final_segment_pulses == 1U);
```

- [ ] **Step 2: Run the test and verify it fails because the planner module is absent.**

- [ ] **Step 3: Implement the plan using quotient and remainder by 256.**

- [ ] **Step 4: Re-run the test and verify it passes.**

### Task 3: Configure TIM1 PWM output and update interrupt

**Files:**
- Modify: `Core/Src/peripheral/tim.c`
- Modify: `Core/Src/system/stm32f4xx_it.c`
- Modify: `Core/Inc/system/stm32f4xx_it.h`

**Interfaces:**
- TIM1 uses PWM mode 1 on CH1 and PE9 AF1.
- `TIM1_UP_TIM10_IRQHandler` invokes `HAL_TIM_IRQHandler(&htim1)`.

- [ ] **Step 1: Change TIM1 from forced active output-compare mode to PWM mode 1 on channel 1.**

Use `HAL_TIM_PWM_Init`, `TIM_OCMODE_PWM1`, a low idle state, and retain PE9 AF1.

- [ ] **Step 2: Enable `TIM1_UP_TIM10_IRQn` in TIM1 initialization.**

```c
HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 5U, 0U);
HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
```

- [ ] **Step 3: Add the interrupt declaration and HAL dispatch handler.**

```c
void TIM1_UP_TIM10_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);
}
```

- [ ] **Step 4: Build the project and verify TIM1 PWM and interrupt source compile without warnings.**

### Task 4: Start, segment, and finish bursts using TIM1 RCR

**Files:**
- Modify: `Core/Src/app/pwm_uart.c`
- Modify: `Core/Inc/app/pwm_uart.h`
- Modify: `Core/Src/app/main.c`

**Interfaces:**
- Produces `PWM_BurstStart(uint32_t frequency_hz, uint32_t pulse_count)`.
- Produces `PWM_Tim1UpdateCallback(void)` called by `HAL_TIM_PeriodElapsedCallback` for TIM1.

- [ ] **Step 1: Replace TIM4 CH2 register writes with TIM1 CH1 register writes.**

Use the 168 MHz TIM1 input clock with prescaler 3 for a 42 MHz counter clock. Set ARR, CCR1 at 50%, RCR, and counter to zero before enabling CH1.

- [ ] **Step 2: Implement burst state for full 256-period segments and a final 1..255-period segment.**

For counts <=256, set `RCR=count-1`, enable one-pulse mode, and start TIM1. For larger counts, run normally with `RCR=255`; preload the final remainder one full segment early; then set one-pulse mode at the boundary immediately before that final segment.

- [ ] **Step 3: Handle TIM1 update callbacks without stopping or restarting between full segments.**

The callback changes only future RCR/OPM state. It must not stop CH1 or reset CNT while full segments remain.

- [ ] **Step 4: On the final hardware update, stop PWM CH1 and force PE9 low.**

The timer is already stopped by one-pulse mode; the completion callback clears CH1 and leaves the output low. It does not send a UART completion message.

- [ ] **Step 5: Preserve PD15 marker timing.**

PD15 rises when the complete UART line is received and falls once the TIM1 burst configuration/start sequence is complete; it does not measure burst duration.

### Task 5: UART response and integration verification

**Files:**
- Modify: `Core/Src/app/pwm_uart.c`
- Modify: `Core/Src/app/main.c`
- Test: `tests/test_pwm_command.c`, `tests/test_pwm_burst_plan.c`

- [ ] **Step 1: Return an immediate launch response only after TIM1 has started.**

```text
OK PWM 100000 Hz x10000
```

- [ ] **Step 2: Return format and range errors for invalid finite commands.**

```text
ERR use: PWM <1..100>,<1..1000000>
ERR PWM range 1..100 kHz, 1..1000000 cycles
```

- [ ] **Step 3: Build with Keil and verify `0 Error(s), 0 Warning(s)`.**

- [ ] **Step 4: Hardware-check with logic analyzer on J3/PE9 and PD15.**

For `PWM 100,10000`, verify 100 kHz, 50% duty, exactly 10000 rising edges, then persistent low level. Also verify `PWM 1,1`, `PWM 1,256`, `PWM 1,257`, and `PWM 100,1000000` behavior.
