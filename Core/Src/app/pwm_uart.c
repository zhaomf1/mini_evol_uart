#include "pwm_uart.h"
#include <string.h>
#include "main.h"
#include "pwm_output.h"
#include "pwm_timing_marker.h"
#include "usart.h"

#define PWM_FRAME_SIZE 14U
#define PWM_DEVICE_ID 0x07U
#define CMD_FINITE 0x01U
#define CMD_CONT 0x02U
#define CMD_STOP 0x03U
#define CMD_VERSION 0xFFU

static uint8_t rx_byte, rx_frame[PWM_FRAME_SIZE], tx_frame[PWM_FRAME_SIZE];
static uint8_t rx_index;
static volatile uint8_t frame_ready;
static PWM_TimingMarker_t timing_marker;

static void marker_write(void *context, uint8_t high)
{
    (void)context;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, high ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static uint32_t read_be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

static void make_error(const uint8_t *request, uint8_t error)
{
    (void)memcpy(tx_frame, request, PWM_FRAME_SIZE);
    tx_frame[3] = (uint8_t)(request[3] | 0x80U);
    tx_frame[4] = error;
}

static uint8_t valid_frame(const uint8_t *frame)
{
    return (uint8_t)(frame[0] == 0x55U && frame[1] == 0xAAU &&
                     frame[2] == PWM_DEVICE_ID && frame[12] == 0x5AU &&
                     frame[13] == 0xA5U);
}

void PWM_Uart1Init(void)
{
    PWM_TimingMarkerInit(&timing_marker, marker_write, NULL);
    PWM_OutputInit();
    rx_index = 0U;
    frame_ready = 0U;
    (void)HAL_UART_Receive_IT(&huart1, &rx_byte, 1U);
}

void PWM_Uart1Process(void)
{
    uint8_t cmd;
    uint8_t error = 0U;
    uint32_t frequency_hz;
    uint32_t cycles;

    PWM_OutputPoll();
    if (frame_ready == 0U) return;
    frame_ready = 0U;
    (void)memcpy(tx_frame, rx_frame, PWM_FRAME_SIZE);
    cmd = rx_frame[3];

    if (valid_frame(rx_frame) == 0U)
    {
        make_error(rx_frame, 0x01U);
    }
    else if ((cmd == CMD_FINITE) || (cmd == CMD_CONT))
    {
        frequency_hz = read_be32(&rx_frame[4]);
        cycles = read_be32(&rx_frame[8]);
        if ((frequency_hz < 1U) || (frequency_hz > 100000U)) error = 0x02U;
        else if ((cmd == CMD_FINITE && (cycles < 1U || cycles > 1000000U)) ||
                 (cmd == CMD_CONT && cycles != 0U)) error = 0x03U;
        else if (PWM_OutputBusy() != 0U) error = 0x05U;
        else if (cmd == CMD_FINITE) (void)PWM_OutputStart(frequency_hz, cycles);
        else (void)PWM_OutputStartContinuous(frequency_hz);
        if (error != 0U) make_error(rx_frame, error);
    }
    else if (cmd == CMD_STOP)
    {
        PWM_OutputInit();
    }
    else if (cmd == CMD_VERSION)
    {
        tx_frame[4] = 1U; tx_frame[5] = 1U; tx_frame[6] = 1U;
        tx_frame[7] = 0U; tx_frame[8] = 0U; tx_frame[9] = 0U;
        tx_frame[10] = 0U; tx_frame[11] = 0U;
    }
    else
    {
        make_error(rx_frame, 0x06U);
    }

    PWM_TimingMarkerEnd(&timing_marker);
    (void)HAL_UART_Transmit(&huart1, tx_frame, PWM_FRAME_SIZE, 100U);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (frame_ready == 0U)
        {
            if ((rx_index == 0U) && (rx_byte != 0x55U)) { }
            else
            {
                rx_frame[rx_index++] = rx_byte;
                if (rx_index == PWM_FRAME_SIZE)
                {
                    frame_ready = 1U;
                    rx_index = 0U;
                    PWM_TimingMarkerBegin(&timing_marker);
                }
            }
        }
        (void)HAL_UART_Receive_IT(&huart1, &rx_byte, 1U);
    }
}
