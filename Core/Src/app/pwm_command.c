#include "pwm_command.h"
#include <stddef.h>

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

    while ((*cursor >= '0') && (*cursor <= '9'))
    {
        if (pulse_count > PWM_COMMAND_MAX_PULSE_COUNT)
        {
            return PWM_COMMAND_RANGE_ERROR;
        }
        pulse_count = (pulse_count * 10U) + (uint32_t)(*cursor - '0');
        cursor++;
    }

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

    parsed_command->frequency_hz = frequency_khz * PWM_COMMAND_FREQUENCY_HZ;
    parsed_command->pulse_count = pulse_count;

    return PWM_COMMAND_OK;
}
