#include "dev_od_ctrl.h"
#include "modbus_rtu.h"
#include "app_control.h"



/**
 * @brief 读取光照，温度值
 * @param slave 从机地址
 * @param temperature 输出温度值 (单位 0.01°C，例如 2530 表示 25.30°C)
 * @return MODBUS_OK 成功，其他错误码
 */
int od_ctrl_read_value(uint16_t *od_data)
{
    int ret = modbus_read_holding_registers(MODBUS_ADDR_OD, 0x0000, 4, od_data);
    return ret;
}


/**
 * @brief 设置光源挡板1打开
 * @param void 
 * @return MODBUS_OK 成功，其他错误码
 */
int od_ctrl_set_shutter1_open(void)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_OD, SHUTTER1_ADDR, SHUTTER_ON);
    return ret;
}

/**
 * @brief 设置光源挡板1关闭
 * @param void
 * @return MODBUS_OK 成功，其他错误码
 */
int od_ctrl_set_shutter1_off(void)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_OD, SHUTTER1_ADDR, SHUTTER_OFF);
    return ret;
}

/**
 * @brief 设置光源挡板2打开
 * @param void 
 * @return MODBUS_OK 成功，其他错误码
 */
int od_ctrl_set_shutter2_open(void)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_OD, SHUTTER2_ADDR, SHUTTER_ON);
    return ret;
}

/**
 * @brief 设置光源挡板2关闭
 * @param void
 * @return MODBUS_OK 成功，其他错误码
 */
int od_ctrl_set_shutter2_off(void)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_OD, SHUTTER2_ADDR, SHUTTER_OFF);
    return ret;
}
