#ifndef __DEV_OD_CTRL_H
#define __DEV_OD_CTRL_H

#include <stdint.h>

/**
 * @brief 读取当前OD值
 * @param slave 从机地址
 * @param temperature 输出温度值 (单位 0.01°C，例如 2530 表示 25.30°C)
 * @return MODBUS_OK 成功，其他错误码
 */
int od_ctrl_read_value(uint16_t *od);

#endif
