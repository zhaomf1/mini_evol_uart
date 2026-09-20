#include "pwm_timing_marker.h"

/* 计时标记在命令处理期间把 GPIO 拉高、其余时间拉低，示波器或逻辑分析仪据此
 * 测量「命令→输出」的延时：高电平脉冲宽度即处理耗时。 */
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
