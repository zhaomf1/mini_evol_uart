#include "dev_od_ctrl.h"
#include "modbus_rtu.h"
#include "app_control.h"



/**
 * @brief 读取当前OD值
 * @param slave 从机地址
 * @param temperature 输出温度值 (单位 0.01°C，例如 2530 表示 25.30°C)
 * @return MODBUS_OK 成功，其他错误码
 */
int od_ctrl_read_value(uint16_t *od)
{
    int ret = modbus_read_holding_registers(MODBUS_ADDR_OD, 0x0000, 1, od);

    return ret;
}
