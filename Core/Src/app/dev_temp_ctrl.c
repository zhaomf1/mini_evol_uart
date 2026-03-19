#include <stdio.h>
#include "dev_temp_ctrl.h"
#include "modbus_rtu.h"
#include "app_control.h"


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
 * @param 
 * @return MODBUS_OK 成功，其他错误码
 */
int temp_ctrl_read_alarm(void)
{
    uint16_t reg = 0;
    int ret = modbus_read_input_registers(MODBUS_ADDR_TEMP, 0x0003, 1, &reg);

    int bit0 = (reg & 0x01);      
    int bit1 = (reg & 0x02) >> 1; 
    int bit2 = (reg & 0x04) >> 2; 
    int bit3 = (reg & 0x08) >> 3;

#if 1
    printf("alarm info: reg = %02X\n",reg);

    if(bit0 == 1){
        printf("温度报警");
    } else if(bit1 == 1){
        printf("长时间不就绪");
    } else if(bit2 == 1){
        printf("探头短路");
    } else if(bit3 == 1){
        printf("探头没接");
    }
#endif

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
 * @return  
 */
void dev_temp_init(void)
{
    temp_ctrl_set_timeout(3600);
}
