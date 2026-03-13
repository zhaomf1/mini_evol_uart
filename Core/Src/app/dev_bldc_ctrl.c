#include "dev_bldc_ctrl.h"
#include "modbus_rtu.h"
#include "app_control.h"
#include "stm32f4xx_hal.h"
#include "freeRTOS.h"


/**
 * @brief 使能电机 第一步调用
 * @param none
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_enable(void)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_TRAIN_BLDC, 0x00B6, 1);
    // ret |= modbus_write_single_register(MODBUS_ADDR_FEEDING_BLDC, 0x00B6, 1);
    printf("ctrl enable ret = %02x\n",ret);
    return ret;
}


/**
 * @brief 设置是否回传协议
 * @param trans 0回传 1不回传
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_protocol_trans(uint16_t trans)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_TRAIN_BLDC, 0x0040, trans);
    // ret |= modbus_write_single_register(MODBUS_ADDR_FEEDING_BLDC, 0x0040, trans);
    printf("protocol ret = %02x\n",ret);
    return ret;
}

/**
 * @brief 设置电机正反转模式
 * @param dir 00正转 01反转
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_dir(uint16_t dir)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_TRAIN_BLDC, 0x006D, dir);
    // ret |= modbus_write_single_register(MODBUS_ADDR_FEEDING_BLDC, 0x006D, dir);
    printf("dir ret = %02x\n",ret);
    return ret;
}

/**
 * @brief 设置电机加速时间
 * @param time 0-15
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_speed_up_time(uint16_t time)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_TRAIN_BLDC, 0x008A, time);
    // ret |= modbus_write_single_register(MODBUS_ADDR_FEEDING_BLDC, 0x008A, time);
    printf("speed up ret = %02x\n",ret);
    return ret;
}

/**
 * @brief 设置电机减速时间
 * @param time 0-15
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_slow_down_time(uint16_t time)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_TRAIN_BLDC, 0x008C, time);
    // ret |= modbus_write_single_register(MODBUS_ADDR_FEEDING_BLDC, 0x008C, time);
    printf("slow time ret = %02x\n",ret);
    return ret;
}

/**
 * @brief 控制电机刹车状态 设置成1
 * @param dir 01启动 00关闭
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_switch(uint16_t switch_ctrl)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_TRAIN_BLDC, 0x006A, switch_ctrl);
    // ret |= modbus_write_single_register(MODBUS_ADDR_FEEDING_BLDC, 0x006A, switch_ctrl);
    printf("switch ret = %02x\n",ret);
    return ret;
}

/**
 * @brief 设置电机转速  转速为0代表停止运行
 * @param speed 电机转速 0-60000
 * @return MODBUS_OK 成功，其他错误码
 */
int bldc_ctrl_set_speed(ModbusAddr_t bldc_addr,uint16_t speed)
{
    int ret = modbus_write_single_register(bldc_addr, 0x0056, speed);
    printf("speed ret = %02x\n",ret);
    return ret;
}


/**
 * @brief 直流无刷电机初始化，上电初始化一次
 * @return 
 */
void bldc_init(void)
{
    bldc_ctrl_enable();
    bldc_ctrl_set_protocol_trans(0);
    bldc_ctrl_set_dir(0);
    bldc_ctrl_set_speed_up_time(8);
    bldc_ctrl_set_slow_down_time(8);
    bldc_ctrl_switch(1);
}