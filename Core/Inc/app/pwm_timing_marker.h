#ifndef PWM_TIMING_MARKER_H
#define PWM_TIMING_MARKER_H

#include <stdint.h>

typedef void (*PWM_TimingMarkerWrite_t)(void *context, uint8_t high);

typedef struct
{
    PWM_TimingMarkerWrite_t write;
    void *context;
} PWM_TimingMarker_t;

void PWM_TimingMarkerInit(PWM_TimingMarker_t *marker,
                          PWM_TimingMarkerWrite_t write,
                          void *context);
void PWM_TimingMarkerBegin(PWM_TimingMarker_t *marker);
void PWM_TimingMarkerEnd(PWM_TimingMarker_t *marker);

#endif
