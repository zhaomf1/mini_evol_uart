#ifndef __DEV_BLDC_CTRL_H
#define __DEV_BLDC_CTRL_H

#include <stdint.h>


int bldc_ctrl_enable(void);
int bldc_ctrl_set_speed(uint16_t speed);
int bldc_ctrl_set_dir(uint16_t dir);
int bldc_ctrl_switch(uint16_t switch_ctrl);
int bldc_ctrl_set_speed_up_time(uint16_t time);
int bldc_ctrl_set_slow_down_time(uint16_t time);
int bldc_ctrl_set_protocol_trans(uint16_t trans);


#endif
