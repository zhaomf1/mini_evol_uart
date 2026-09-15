#include "pwm_timing_marker.h"

void PWM_TimingMarkerInit(PWM_TimingMarker_t *marker,
                          PWM_TimingMarkerWrite_t write,
                          void *context)
{
    marker->write = write;
    marker->context = context;
    marker->write(marker->context, 0U);
}

void PWM_TimingMarkerBegin(PWM_TimingMarker_t *marker)
{
    marker->write(marker->context, 1U);
}

void PWM_TimingMarkerEnd(PWM_TimingMarker_t *marker)
{
    marker->write(marker->context, 0U);
}
