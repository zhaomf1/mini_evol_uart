#include "pwm_command.h"
#include <stddef.h>

/* 接受的命令格式："PWM <频率kHz>,<脉冲数>"，例如 "PWM 10,10000" = 10kHz、10000 个周期。
 * 用逐字符游标解析而非 sscanf，体积小、行为确定，并能显式拒绝多余的尾随字符。 */
#define PWM_COMMAND_MIN_VALUE        1U
#define PWM_COMMAND_MAX_VALUE        100U
#define PWM_COMMAND_MAX_PULSE_COUNT  1000000U
#define PWM_COMMAND_FREQUENCY_HZ      1000U

PWM_CommandStatus_t PWM_CommandParse(const char *command, PWM_Command_t *parsed_command)
{
    const char *cursor;
    uint32_t frequency_khz = 0U;
    uint32_t pulse_count = 0U;

    if ((command == NULL) || (parsed_command == NULL))
    {
        return PWM_COMMAND_FORMAT_ERROR;
    }

    if ((command[0] != 'P') || (command[1] != 'W') ||
        (command[2] != 'M') || (command[3] != ' '))
    {
        return PWM_COMMAND_FORMAT_ERROR;
    }

    cursor = &command[4];
    if ((*cursor < '0') || (*cursor > '9'))
    {
        return PWM_COMMAND_FORMAT_ERROR;
    }

    /* 累加 kHz 字段；先判断再乘，溢出或越界时直接拒绝，不会产生回绕后的结果。 */
    while ((*cursor >= '0') && (*cursor <= '9'))
    {
        if (frequency_khz > PWM_COMMAND_MAX_VALUE)
        {
            return PWM_COMMAND_RANGE_ERROR;
        }
        frequency_khz = (frequency_khz * 10U) + (uint32_t)(*cursor - '0');
        cursor++;
    }

    if (*cursor != ',')
    {
        return PWM_COMMAND_FORMAT_ERROR;
    }
    cursor++;

    if ((*cursor < '0') || (*cursor > '9'))
    {
        return PWM_COMMAND_FORMAT_ERROR;
    }

    /* 用同样的溢出保护累加脉冲数字段。 */
    while ((*cursor >= '0') && (*cursor <= '9'))
    {
        if (pulse_count > PWM_COMMAND_MAX_PULSE_COUNT)
        {
            return PWM_COMMAND_RANGE_ERROR;
        }
        pulse_count = (pulse_count * 10U) + (uint32_t)(*cursor - '0');
        cursor++;
    }

    /* 允许以 CRLF 或单独的 LF 作为行结束符。 */
    if ((*cursor == '\r') && (cursor[1] == '\n'))
    {
        cursor += 2;
    }
    else if (*cursor == '\n')
    {
        cursor++;
    }

    if (*cursor != '\0')
    {
        return PWM_COMMAND_FORMAT_ERROR;
    }

    if ((frequency_khz < PWM_COMMAND_MIN_VALUE) ||
        (frequency_khz > PWM_COMMAND_MAX_VALUE) ||
        (pulse_count < PWM_COMMAND_MIN_VALUE) ||
        (pulse_count > PWM_COMMAND_MAX_PULSE_COUNT))
    {
        return PWM_COMMAND_RANGE_ERROR;
    }

    /* 命令里频率以 kHz 表示；此处换算成 Hz 供定时器层使用。 */
    parsed_command->frequency_hz = frequency_khz * PWM_COMMAND_FREQUENCY_HZ;
    parsed_command->pulse_count = pulse_count;

    return PWM_COMMAND_OK;
}
