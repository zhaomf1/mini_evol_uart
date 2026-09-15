#include <assert.h>
#include <stdint.h>

#include "pwm_timing_marker.h"

typedef struct
{
    uint8_t last_level;
    uint8_t write_count;
} MarkerOutput_t;

static void record_marker_level(void *context, uint8_t high)
{
    MarkerOutput_t *output = (MarkerOutput_t *)context;

    output->last_level = high;
    output->write_count++;
}

static void test_marker_is_low_at_initialization_and_after_pwm_update(void)
{
    MarkerOutput_t output = { 1U, 0U };
    PWM_TimingMarker_t marker;

    PWM_TimingMarkerInit(&marker, record_marker_level, &output);
    assert(output.last_level == 0U);

    PWM_TimingMarkerBegin(&marker);
    assert(output.last_level == 1U);

    PWM_TimingMarkerEnd(&marker);
    assert(output.last_level == 0U);
    assert(output.write_count == 3U);
}

int main(void)
{
    test_marker_is_low_at_initialization_and_after_pwm_update();
    return 0;
}
