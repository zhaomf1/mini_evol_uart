#include "pwm_uart.h"

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "pwm_command.h"
#include "pwm_output.h"
#include "pwm_timing_marker.h"
#include "usart.h"

#define PWM_COMMAND_BUFFER_SIZE  32U

static uint8_t pwm_rx_byte;
static char pwm_command_buffer[PWM_COMMAND_BUFFER_SIZE];
static volatile uint8_t pwm_command_length;
static volatile uint8_t pwm_command_ready;
static volatile uint8_t pwm_command_overflow;
static PWM_TimingMarker_t pwm_timing_marker;

static void PWM_TimingMarkerWriteGpio(void *context, uint8_t high)
{
    (void)context;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,
                      (high != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void PWM_Uart1Init(void)
{
    PWM_TimingMarkerInit(&pwm_timing_marker, PWM_TimingMarkerWriteGpio, NULL);
    PWM_OutputInit();
    (void)HAL_UART_Receive_IT(&huart1, &pwm_rx_byte, 1U);
}

void PWM_Uart1Process(void)
{
    uint32_t actual_frequency_hz;
    uint32_t saved_primask;
    PWM_Command_t parsed_command;
    PWM_CommandStatus_t status;
    char command[PWM_COMMAND_BUFFER_SIZE];
    char response[48];
    int response_length;

    if (pwm_command_ready == 0U)
    {
        return;
    }

    /* Retain ready until marker end: RX must not begin another marker
       while this command is being applied. Host waits for the reply. */
    (void)memcpy(command, pwm_command_buffer, sizeof(command));
    status = PWM_CommandParse(command, &parsed_command);

    if (status == PWM_COMMAND_OK)
    {
        actual_frequency_hz = PWM_OutputStart(parsed_command.frequency_hz,
                                              parsed_command.pulse_count);
        PWM_TimingMarkerEnd(&pwm_timing_marker);
        if (actual_frequency_hz != 0U)
        {
            response_length = snprintf(response, sizeof(response),
                                       "OK PWM %lu Hz x%lu\r\n",
                                       (unsigned long)actual_frequency_hz,
                                       (unsigned long)parsed_command.pulse_count);
        }
        else
        {
            response_length = snprintf(response, sizeof(response), "ERR PWM busy\r\n");
        }
    }
    else if (status == PWM_COMMAND_RANGE_ERROR)
    {
        PWM_TimingMarkerEnd(&pwm_timing_marker);
        response_length = snprintf(response, sizeof(response),
                                   "ERR PWM range 1..100 kHz, 1..1000000 cycles\r\n");
    }
    else
    {
        PWM_TimingMarkerEnd(&pwm_timing_marker);
        response_length = snprintf(response, sizeof(response),
                                   "ERR use: PWM <1..100>,<1..1000000>\r\n");
    }

    saved_primask = __get_PRIMASK();
    __disable_irq();
    pwm_command_ready = 0U;
    __set_PRIMASK(saved_primask);
    if ((response_length > 0) && ((size_t)response_length < sizeof(response)))
    {
        (void)HAL_UART_Transmit(&huart1, (uint8_t *)response,
                                (uint16_t)response_length, 100U);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (pwm_command_ready == 0U)
        {
            if (pwm_rx_byte == '\n')
            {
                if (pwm_command_overflow != 0U)
                {
                    /* Reject the whole overlong line, never its suffix. */
                    pwm_command_buffer[0] = '\0';
                }
                else
                {
                    pwm_command_buffer[pwm_command_length++] = '\n';
                    pwm_command_buffer[pwm_command_length] = '\0';
                }
                pwm_command_ready = 1U;
                PWM_TimingMarkerBegin(&pwm_timing_marker);
                pwm_command_length = 0U;
                pwm_command_overflow = 0U;
            }
            else if (pwm_command_overflow == 0U)
            {
                if (pwm_command_length < (PWM_COMMAND_BUFFER_SIZE - 2U))
                {
                    pwm_command_buffer[pwm_command_length++] = (char)pwm_rx_byte;
                }
                else
                {
                    pwm_command_overflow = 1U;
                }
            }
        }

        (void)HAL_UART_Receive_IT(&huart1, &pwm_rx_byte, 1U);
    }
}
