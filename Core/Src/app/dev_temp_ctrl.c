#include <stdio.h>
#include "dev_temp_ctrl.h"
#include "modbus_rtu.h"
#include "app_control.h"

uint8_t temp_error_info[50]; // 温度报警信息


/**
 * @brief 控制温控开关
 * @param state     非0-开启，0x0000-关闭
 * @return MODBUS_OK 成功，其他错误码
 */
int temp_ctrl_switch_ctrl_temperature(uint16_t state)
{
    int ret = modbus_write_single_coil(MODBUS_ADDR_TEMP, 0x0000, state);

    return ret;
}

/**
 * @brief 设定目标温度
 * @param temperature 设定温度值,温度系数0.01
 * @return MODBUS_OK 成功，其他错误码
 */
int temp_ctrl_set_temperature(uint16_t temperature)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_TEMP, 0x0000, temperature);

    return ret;
}

/**
 * @brief 读取当前温度
 * @param temperature 输出温度值 (单位 0.01°C，例如 2530 表示 25.30°C)
 * @return MODBUS_OK 成功，其他错误码
 */
int temp_ctrl_read_temperature(uint16_t *temperature)
{
    uint16_t reg;
    int ret = modbus_read_input_registers(MODBUS_ADDR_TEMP, 0x0000, 1, &reg);
    if (ret == MODBUS_OK) {
        *temperature = reg;
    }
    return ret;
}

/**
 * @brief 读取报警信息
 * @param alarm_info 输出报警信息
 * @return MODBUS_OK 成功，其他错误码
 */
int temp_ctrl_read_alarm(uint16_t *alarm_info)
{
    uint16_t reg = 0;
    int ret = modbus_read_input_registers(MODBUS_ADDR_TEMP, 0x0003, 1, &reg);
    if (ret == MODBUS_OK) {
        *alarm_info = reg;
    }

    return ret;
}

/**
 * @brief 设置超时时间
 * @param time 超时时间 (60 - 3600 单位 秒)
 * @return MODBUS_OK 成功，其他错误码
 */
int temp_ctrl_set_timeout(uint16_t second)
{
    int ret = modbus_write_single_register(MODBUS_ADDR_TEMP, 0x0009, second);

    return ret;
}


/**
 * @brief 初始化温控设备,上电初始化一次
 * @return  0 成功 -1 失败
 */
/**
 * @brief 初始化设备温度控制模块
 * 
 * @return int 成功返回0，失败返回-1
 */
int dev_temp_init(void)
{
    // 设置温度控制超时时间为3600秒
    if(temp_ctrl_set_timeout(3600) != 0)
    {
        // 设置超时失败，返回错误码
        return -1;
    }
    // 初始化成功
    return 0;
}


void set_temp_error_info(uint16_t error_code)
{
    int bit0 = (error_code & 0x01);
    int bit1 = (error_code & 0x02) >> 1;
    int bit2 = (error_code & 0x04) >> 2;
    int bit3 = (error_code & 0x08) >> 3;

    memset(temp_error_info, 0, sizeof(temp_error_info));

    if (bit0 == 0 && bit1 == 0 && bit2 == 0 && bit3 == 0) {
        strcpy((char*)temp_error_info, "OK");
        return;
    }

    if (bit0 == 1) {
        strcat((char*)temp_error_info, "temp alarm");
    }
    if (bit1 == 1) {
        if (strlen((char*)temp_error_info) > 0) {
            strcat((char*)temp_error_info, ",");
        }
        strcat((char*)temp_error_info, "temp not ready for a long time");
    }
    if (bit2 == 1) {
        if (strlen((char*)temp_error_info) > 0) {
            strcat((char*)temp_error_info, ",");
        }
        strcat((char*)temp_error_info, "temp probe short circuit");
    }
    if (bit3 == 1) {
        if (strlen((char*)temp_error_info) > 0) {
            strcat((char*)temp_error_info, ",");
        }
        strcat((char*)temp_error_info, "temp probe open circuit");
    }
}

void get_temp_error_info(uint8_t *error_info)
{
    strcpy((char*)error_info, (char*)temp_error_info);
}

/**
 * @brief 读取温控报警并更新错误信息
 * @return MODBUS_OK 成功，其他错误码
 */
int temp_ctrl_check_and_update_alarm(void)
{
    uint16_t alarm_info = 0;
    int ret = temp_ctrl_read_alarm(&alarm_info);
    if (ret == MODBUS_OK) {
        set_temp_error_info(alarm_info);
    }
    return ret;
}