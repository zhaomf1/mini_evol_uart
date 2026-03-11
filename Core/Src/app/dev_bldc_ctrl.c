#include "dev_bldc_ctrl.h"
#include "modbus_rtu.h"
#include "app_control.h"


/**
 * @brief 使能电机 第一步调用
 * @param none
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_enable(void)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_BLDC1, 0x00B6, 1);
    return ret;
}

/**
 * @brief 设置电机转速
 * @param speed 电机转速 0-60000
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_speed(uint16_t speed)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_BLDC1, 0x0056, speed);
    return ret;
}

/**
 * @brief 设置是否回传协议
 * @param trans 0回传 1不回传
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_protocol_trans(uint16_t trans)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_BLDC1, 0x0040, trans);
    return ret;
}

/**
 * @brief 设置电机正反转模式
 * @param dir 00正转 01反转
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_dir(uint16_t dir)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_BLDC1, 0x006D, dir);
    return ret;
}

/**
 * @brief 设置电机加速时间
 * @param time 0-15
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_speed_up_time(uint16_t time)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_BLDC1, 0x008A, time);
    return ret;
}

/**
 * @brief 设置电机减速时间
 * @param time 0-15
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_slow_down_time(uint16_t time)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_BLDC1, 0x008C, time);
    return ret;
}

/**
 * @brief 控制电机开关
 * @param dir 01启动 00关闭
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_switch(uint16_t switch_ctrl)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_BLDC1, 0x006A, switch_ctrl);
    return ret;
}
