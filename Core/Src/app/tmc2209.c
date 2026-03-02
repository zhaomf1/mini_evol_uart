/*
 * TMC2209.c
 *
 *  Created on: 2025
 *      Author: yjj

 */

#include "usart.h"
#include "tmc2209.h"
#include "crc.h"

uint8_t rxBuffer[8] = {0};
uint8_t txBuffer[8] = {0};
// uint8_t UART_Address = 3;                //对于TMC2209 设置UART的器件地址，地址范围0...3。

uint32_t volatile register_value = 0; // 临时变量
__TMC2209 TMC2209;

#define MOTOR_UART_HANDLE &huart2

#define ENABLE_CONTROL1() HAL_GPIO_WritePin(ENABLE_1_GPIO_Port, ENABLE_1_Pin, GPIO_PIN_RESET)  // ENABLE_1_Pin引脚置低电平使能电机
#define DISENABLE_CONTROL1() HAL_GPIO_WritePin(ENABLE_1_GPIO_Port, ENABLE_1_Pin, GPIO_PIN_SET) // ENABLE_1_Pin引脚置高电平不使能电机
#define ENABLE_CONTROL2() HAL_GPIO_WritePin(ENABLE_2_GPIO_Port, ENABLE_2_Pin, GPIO_PIN_RESET)  // ENABLE_2_Pin引脚置低电平使能电机
#define DISENABLE_CONTROL2() HAL_GPIO_WritePin(ENABLE_2_GPIO_Port, ENABLE_2_Pin, GPIO_PIN_SET) // ENABLE_2_Pin引脚置高电平不使能电机
#define ENABLE_CONTROL3() HAL_GPIO_WritePin(ENABLE_4_GPIO_Port, ENABLE_4_Pin, GPIO_PIN_RESET)  // ENABLE_2_Pin引脚置低电平使能电机
#define DISENABLE_CONTROL3() HAL_GPIO_WritePin(ENABLE_4_GPIO_Port, ENABLE_4_Pin, GPIO_PIN_SET) // ENABLE_2_Pin引脚置高电平不使能电机
#define ENABLE_CONTROL4() HAL_GPIO_WritePin(ENABLE_4_GPIO_Port, ENABLE_4_Pin, GPIO_PIN_RESET)  // ENABLE_2_Pin引脚置低电平使能电机
#define DISENABLE_CONTROL4() HAL_GPIO_WritePin(ENABLE_4_GPIO_Port, ENABLE_4_Pin, GPIO_PIN_SET) // ENABLE_2_Pin引脚置高电平不使能电机

/**
 * @brief  往寄存器值写入参数
 * @param  slaveAddress：UART通信地址。地址值范围0...3；由MS1、MS2两引脚的电平设置。两引脚有内置下拉电阻
 * @param  address：     寄存器地址
 * @param    value：     写入的值
 * @attention   有的寄存器仅读访问，也有寄存器可读可写，具体查TMC2209规格书确认。
 *              TMC2209的UART的波特率自适应。本例程用的MCU是STM32F103系列，实际波特率115200
 *
 */
void TMC2209_writeInt(uint8_t slaveAddress, uint8_t address, int32_t value)
{
    txBuffer[0] = 0x05;
    txBuffer[1] = slaveAddress;
    txBuffer[2] = address | TMC2209_WRITE_BIT;
    txBuffer[3] = (value >> 24) & 0xFF;
    txBuffer[4] = (value >> 16) & 0xFF;
    txBuffer[5] = (value >> 8) & 0xFF;
    txBuffer[6] = (value) & 0xFF;
    txBuffer[7] = tmc_crc(txBuffer, 7);

    HAL_UART_Transmit_DMA(MOTOR_UART_HANDLE, txBuffer, 8);
    HAL_Delay(1);
}

/**
 * @brief  往寄存器值写入参数
 * @param  slaveAddress：UART通信地址。地址值范围0...3；由MS1、MS2两引脚的电平设置。
 * @param  address：     寄存器地址
 * @attention   有的寄存器仅读访问，也有寄存器可读可写，具体查TMC2209规格书确认。
 *
 */
int32_t TMC2209_readInt(uint8_t slaveAddress, uint8_t address)
{
    uint8_t data[12] = {0};

    address = TMC_ADDRESS(address);

    txBuffer[0] = 0x05;
    txBuffer[1] = slaveAddress;
    txBuffer[2] = address;
    txBuffer[3] = tmc_crc(txBuffer, 3);

    HAL_UART_Receive_DMA(MOTOR_UART_HANDLE, data, 12);
    HAL_UART_Transmit_DMA(MOTOR_UART_HANDLE, txBuffer, 4);

    HAL_Delay(1);

    for (int i = 0; i < 8; i++)
    {
        rxBuffer[i] = data[i + 4];
    }

    // Byte 0: Sync nibble correct?
    if (rxBuffer[0] != 0x05)
        return 0;

    // Byte 1: Master address correct?
    if (rxBuffer[1] != 0xFF)
        return 0;

    // Byte 2: Address correct?
    if (rxBuffer[2] != address)
        return 0;

    // Byte 7: CRC correct?
    if (rxBuffer[7] != tmc_crc(rxBuffer, 7))
        return 0;

    return (rxBuffer[3] << 24) | (rxBuffer[4] << 16) | (rxBuffer[5] << 8) | rxBuffer[6];
}

// 对TMC2209初始化，设定其电流、细分、斩波模式等参数
void TMC2209_init(uint8_t slaveAddress, uint8_t slaveIRUN, uint8_t slaveMRES)
{
    DISENABLE_CONTROL1(); // 先不使能电机，不锁轴
    DISENABLE_CONTROL2(); // 先不使能电机，不锁轴
    DISENABLE_CONTROL3();
    DISENABLE_CONTROL4();
    volatile uint8_t UART_Address;
    volatile uint8_t MRES;

    volatile uint8_t version;
    UART_Address = slaveAddress;

    //    register_value = TMC2209_readInt(UART_Address, TMC2209_IOIN);  只写寄存器不再读寄存器
    //		register_value = TMC2209_readInt(UART_Address, TMC2209_IOIN);
    //	version = TMC2209_readInt(UART_Address, TMC2209_IOIN)>>24;
    //	if(version!=0x21)//获取版本号。
    //	{
    ////		while(1);//版本号获取失败，请检查通讯，vccio供电
    //
    //	}
    // 获取版本号

    // 配置GCONF寄存器。配置斩波模式等参数。
    TMC2209.GCONF.en_SpreadCycle = Stealthchop; // 可选 Stealthchop：通常翻译成静音模式，选择这斩波模式，电机中低速转动时噪声通常很低，不过有转速上限，在300RPM左右。
    //  TMC2209.GCONF.en_SpreadCycle        =   Spreadcycle;                //或者选 Spreadcycle：通常翻译成高速模式，电机可以低速跑，也可以高转速跑，例如能跑到3000RPM。电机转动时通常有种电流声。
    TMC2209.GCONF.I_scale_analog = RESET; // 0：只用0x10寄存器设置电机相电流。
                                          // 1：0x10寄存器，还有VERF引脚(例如输入电压)，一起控制电机相电流。
                                          //      TMC2209.GCONF.internal_Rsense       =   SET;                       // 1：使用内部采样电阻

    TMC2209.GCONF.pdn_disable = SET; // 1：通过UART配置的0x10寄存器控制电机待机电流（当使用串口时设置这位）

    TMC2209.GCONF.mstep_reg_select = SET; // 1：通过UART配置0x6C寄存器的MRES参数设置细分

    TMC2209.GCONF.multistep_filt = SET; // 1：当STEP引脚的输入频率 > 750Hz时。芯片内部软件算法对STEP输入信号滤波处理。

    // 配置电流
    TMC2209.IHOLD_IRUN.IRUN = slaveIRUN;      // 调整电机运行电流，数值范围0...31。调整这里可改变电机转动时的扭矩。
    TMC2209.IHOLD_IRUN.IHOLD = slaveIRUN / 2; // 调整电机待机电流，数值范围0...31。调整这里可改变电机待机时的扭矩，可设置为IRUN的一半。
    TMC2209.IHOLD_IRUN.IHOLDDELAY = 8;        // 调整运行电流到待机电流的延时，数值范围0...15。如果设为0，是瞬间断电。平滑过渡可避免断电时电机抖动。

    // 配置细分
    MRES = slaveMRES; // 0;                                                      //设置细分。数值范围0...8。调整这里可改变电机微步步距角
                      // MRES = 0，是256细分。例如1.8度步距角的电机转一圈则需要51200个脉冲数。
                      // MRES = 1，是128细分。例如1.8度步距角的电机转一圈则需要25600个脉冲数。
                      // MRES = 2，是 64细分。例如1.8度步距角的电机转一圈则需要12800个脉冲数。
                      // MRES = 3，是 32细分；			//MRES = 4，是 16细分；			//MRES = 5，是  8细分。
                      // MRES = 6，是  4细分；			//MRES = 7，是  2细分；			//MRES = 8，是  0细分，电机整步运行。

    TMC2209_writeInt(UART_Address, TMC2209_GCONF, TMC2209.GCONF.Value);
    TMC2209_writeInt(UART_Address, TMC2209_IHOLD_IRUN, 0x70000 | TMC2209.IHOLD_IRUN.Value);
    TMC2209_writeInt(UART_Address, TMC2209_TCOOLTHRS, 0x0);
    TMC2209_writeInt(UART_Address, TMC2209_CHOPCONF, 0x10010053 | (MRES << TMC2209_MRES_SHIFT));
    TMC2209_writeInt(UART_Address, TMC2209_PWMCONF, 0xC10D0024);

#if 0

    TMC2209_writeInt(UART_Address, TMC2209_TPWMTHRS,  54);            //设置这寄存器，可让5160芯片根据速度，在运动过程中自动切换斩波模式。要先把“stealthchop”赋值给“TMC5160_Registers.GCONF.en_pwm_mode”。
                                                        //不过运动过程中切换斩波模式有副作用，不管是自动切换还是手动切换，切换那个点电机会抖动，会丢步。
                                                        //所以不建议运动过程中自动切换，不管是手动切换还是自动切换。
                                                        //寄存器配置，具体对应多少RPM，可看表格“斜坡计算公式”。
#else

    TMC2209_writeInt(UART_Address, TMC2209_TPWMTHRS, 0);
#endif

    ENABLE_CONTROL1(); // 使能电机，锁轴
    ENABLE_CONTROL2(); // 使能电机，锁轴
    ENABLE_CONTROL3();
    ENABLE_CONTROL4();
    HAL_Delay(100);    // 初始化完，务必延时100毫秒，让内部完成初始化
}
