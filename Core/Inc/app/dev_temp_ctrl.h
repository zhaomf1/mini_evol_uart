#ifndef __DEV_TEMP_CTRL_H
#define __DEV_TEMP_CTRL_H

#include <stdint.h>

typedef enum
{
    TEMP_ALARM,         // 温度报警
    TEMP_NOT_READY,     //长时间不就绪
    TEMP_SHORT,         //探头短路
    TEMP_OPEN           //探头没接
} temp_alarm_t;

int temp_ctrl_switch_ctrl_temperature(uint16_t state);
int temp_ctrl_set_temperature(uint16_t temperature);
int temp_ctrl_read_temperature(uint16_t *temperature);
int temp_ctrl_read_alarm(void);
int temp_ctrl_set_timeout(uint16_t second);
void dev_temp_init(void);


#endif
