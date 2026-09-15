#include <assert.h>
#include <stdint.h>

#include "pwm_command.h"

static void test_counted_commands_map_from_khz_to_hz(void)
{
    PWM_Command_t command;

    assert(PWM_CommandParse("PWM 1,1\r\n", &command) == PWM_COMMAND_OK);
    assert(command.frequency_hz == 1000U);
    assert(command.pulse_count == 1U);
    assert(PWM_CommandParse("PWM 100,10000\n", &command) == PWM_COMMAND_OK);
    assert(command.frequency_hz == 100000U);
    assert(command.pulse_count == 10000U);
}

static void test_invalid_or_out_of_range_commands_are_rejected(void)
{
    PWM_Command_t command;

    assert(PWM_CommandParse("PWM 0,1\r\n", &command) == PWM_COMMAND_RANGE_ERROR);
    assert(PWM_CommandParse("PWM 101,1\r\n", &command) == PWM_COMMAND_RANGE_ERROR);
    assert(PWM_CommandParse("PWM 1,1000001\r\n", &command) == PWM_COMMAND_RANGE_ERROR);
    assert(PWM_CommandParse("PWM 100\r\n", &command) == PWM_COMMAND_FORMAT_ERROR);
    assert(PWM_CommandParse("PWM abc,1\r\n", &command) == PWM_COMMAND_FORMAT_ERROR);
}

int main(void)
{
    test_counted_commands_map_from_khz_to_hz();
    test_invalid_or_out_of_range_commands_are_rejected();
    return 0;
}
