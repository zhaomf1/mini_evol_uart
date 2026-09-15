#ifndef PWM_COMMAND_H
#define PWM_COMMAND_H

#include <stdint.h>

typedef enum
{
    PWM_COMMAND_OK = 0,
    PWM_COMMAND_FORMAT_ERROR,
    PWM_COMMAND_RANGE_ERROR
} PWM_CommandStatus_t;

typedef struct
{
    uint32_t frequency_hz;
    uint32_t pulse_count;
} PWM_Command_t;

PWM_CommandStatus_t PWM_CommandParse(const char *command, PWM_Command_t *parsed_command);

#endif /* PWM_COMMAND_H */
