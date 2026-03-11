#include "dev_ph_ctrl.h"
#include "modbus_rtu.h"
#include "app_control.h"


/**
 * @brief 将2个16位寄存器数据转换为IEEE 754单精度浮点数
 * @param high 高16位寄存器值
 * @param low 低16位寄存器值
 * @return 解析后的浮点数
 */
float reg_to_float(uint16_t high, uint16_t low) {
    uint32_t float_32bit = ((uint32_t)low << 16) | high;
    float result;
    memcpy(&result, &float_32bit, sizeof(float)); // 内存拷贝避免精度丢失
    return result;
}


/**
 * @brief 读取当前ph值
 * @param od ph值
 * @return MODBUS_OK 成功，其他错误码
 */
int ph_ctrl_read_value(float *od)
{
    uint16_t reg_buf[10] = {0};
    int ret = modbus_read_holding_registers(MODBUS_ADDR_PH, 0x0829, 0x0A, reg_buf);

    if (reg_buf[0] != 0x1000) {
        printf("\nwarning: not ph reg=0x%04x\n", reg_buf[0]);
        return -2;
    }

    // 验证：测量状态是否正常（Reg5+Reg6=0x0000）
    uint32_t status = ((uint32_t)reg_buf[4] << 16) | reg_buf[5];
    if (status != 0) {
        printf("\nwarning:Abnormal measurement status=0x%08x\n", status);
        return -3;
    }

    // 核心：解析PH值（Reg3=reg_buf[2]，Reg4=reg_buf[3]）
    *od = reg_to_float(reg_buf[2], reg_buf[3]);
    printf("ph = %.4f\n", *od);


    return ret;
}
