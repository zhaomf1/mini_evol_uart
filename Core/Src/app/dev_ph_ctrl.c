#include <string.h>
#include <stdio.h>
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
    memcpy(&result, &float_32bit, sizeof(float));
    return result;
}


#define PH_ACTIVE_ERROR_ADDR   0x12BF  // 手册寄存器 4800（1-based）→ Modbus 4799
#define PH_ACTIVE_ERROR_COUNT  8

/**
 * @brief 读取并打印 PH 计活动错误寄存器（手册 4800，8 个寄存器）
 * @note  只在测量状态非 0 时调用，用于定位具体错误
 */
static void ph_ctrl_print_active_errors(void)
{
    uint16_t err_reg[PH_ACTIVE_ERROR_COUNT] = {0};
    int ret = modbus_read_holding_registers(MODBUS_ADDR_PH,
                                            PH_ACTIVE_ERROR_ADDR,
                                            PH_ACTIVE_ERROR_COUNT,
                                            err_reg);

    if (ret != MODBUS_OK) {
        printf("[PH ERR] read active errors(4800) failed, ret=%d\r\n", ret);
        return;
    }

    printf("[PH ERR] active errors(4800): "
           "%04X %04X %04X %04X %04X %04X %04X %04X\r\n",
           err_reg[0], err_reg[1], err_reg[2], err_reg[3],
           err_reg[4], err_reg[5], err_reg[6], err_reg[7]);

    // 8 个寄存器按顺序对应 bit0~bit127：
    // err_reg[0]=bit0~15, err_reg[1]=bit16~31, ..., err_reg[7]=bit112~127
    uint8_t found = 0;

    if (err_reg[0] & (1u << 0))  { printf("  bit0  pH reading failure (summary)\r\n");           found = 1; }
    if (err_reg[0] & (1u << 1))  { printf("  bit1  calibration error / sensor failure\r\n");     found = 1; }
    if (err_reg[0] & (1u << 5))  { printf("  bit5  glass resistance too high\r\n");              found = 1; }
    if (err_reg[0] & (1u << 6))  { printf("  bit6  glass resistance too low\r\n");               found = 1; }
    if (err_reg[0] & (1u << 7))  { printf("  bit7  reference electrode resistance too high\r\n"); found = 1; }
    if (err_reg[0] & (1u << 8))  { printf("  bit8  reference electrode resistance too low\r\n");  found = 1; }
    if (err_reg[0] & (1u << 15)) { printf("  bit15 auxiliary electrode potential too high\r\n");  found = 1; }
    if (err_reg[1] & (1u << 0))  { printf("  bit16 auxiliary electrode potential too low\r\n");   found = 1; }
    if (err_reg[1] & (1u << 1))  { printf("  bit17 auxiliary electrode resistance too high\r\n"); found = 1; }
    if (err_reg[1] & (1u << 2))  { printf("  bit18 auxiliary electrode resistance too low\r\n");  found = 1; }
    if (err_reg[1] & (1u << 8))  { printf("  bit24 internal communication error\r\n");            found = 1; }
    if (err_reg[1] & (1u << 9))  { printf("  bit25 temperature sensor defective\r\n");            found = 1; }

    if (!found) {
        printf("  no known error bit matched; check raw registers above\r\n");
    }
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
    uint32_t status = ((uint32_t)reg_buf[5] << 16) | reg_buf[4];
    if (status != 0) {
        printf("\nwarning:Abnormal measurement status=0x%08x\n", status);
        ph_ctrl_print_active_errors();
        return -3;
    }

    // 核心：解析PH值（Reg3=reg_buf[2]，Reg4=reg_buf[3]）
    *od = reg_to_float(reg_buf[2], reg_buf[3]);
    printf("ph = %.4f\n", *od);


    return ret;
}
