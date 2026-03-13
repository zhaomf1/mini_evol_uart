/*
 * TMC2209_Register.h
 *
 *  Created on: 2025
 *      Author: yjj
 */

#ifndef TMC2209_REGISTER_H
#define TMC2209_REGISTER_H

# include "stdint.h"

// ===== TMC2209 & 2202 & TMC2209 & 2220 & 2225 "Donkey Kong" family register set =====

#define TMC2209_GCONF         0x00

#define TMC2209_GSTAT         0x01
#define TMC2209_IFCNT         0x02
#define TMC2209_SLAVECONF     0x03
#define TMC2209_OTP_PROG      0x04
#define TMC2209_OTP_READ      0x05
#define TMC2209_IOIN          0x06
#define TMC2209_FACTORY_CONF  0x07

#define TMC2209_IHOLD_IRUN    0x10
#define TMC2209_TPOWERDOWN    0x11
#define TMC2209_TSTEP         0x12
#define TMC2209_TPWMTHRS      0x13
#define TMC2209_TCOOLTHRS     0x14

#define TMC2209_VACTUAL       0x22

#define TMC2209_SGTHRS        0x40
#define TMC2209_SG_RESULT     0x41
#define TMC2209_COOLCONF      0x42

#define TMC2209_MSCNT         0x6A
#define TMC2209_MSCURACT      0x6B
#define TMC2209_CHOPCONF      0x6C

#define TMC2209_DRVSTATUS     0x6F

#define TMC2209_PWMCONF       0x70
#define TMC2209_PWMSCALE      0x71
#define TMC2209_PWM_AUTO      0x72



#pragma anon_unions
typedef union // 用来描述 GCONF(0x00)的数据字
{
	uint32_t Value;
	struct //
	{
		// 低位在前，高位在后
		uint8_t I_scale_analog              : 1;    // 0：只用0x10寄存器设置电机相电流。
                                                    // 1：0x10寄存器，还有VERF引脚(例如输入电压)，一起控制电机相电流。
        
		uint8_t internal_Rsense             : 1;    // 0：正常使用外部采样电阻（配置这种更多）。
                                                    // 1：内部检测电阻。 使用提供给 AIN 的电流作为内部检测电阻的参考。
        
		uint8_t en_SpreadCycle              : 1;    // 0：使用Stealthchop斩波模式，通常翻译成静音斩波模式
                                                    // 1：使用Spreadcycle斩波模式，通常翻译高速斩波模式，或者非静音斩波模式。
        
		uint8_t shaft                       : 1;    // 1：电机反方向转动。如果电机两相的线交换，电机正方向转动的方向会改变，这时候可以设置这位切换电机方向。电机正常转动需切换方向时不是设置这位。
		uint8_t index_otpw 	                : 1;    // 
		uint8_t index_step 	                : 1;    // 
		uint8_t pdn_disable    		        : 1;    // 0：通过PDN_UART引脚的高低电平控制电机待机电流
                                                    // 1：通过UART配置的0x10寄存器控制电机待机电流（当使用串口时设置这位）
        
		uint8_t mstep_reg_select    		: 1;    // 0：通过MS1、MS2两引脚设置电机细分。改变MS1、MS2两引脚电平，也会改变串口器件地址
                                                    // 1：通过UART配置0x6C寄存器的MRES参数设置细分。
                                                    
        uint8_t multistep_filt              : 1;    // 0：不对STEP引脚的输入滤波
                                                    // 1：当STEP引脚的输入频率 > 750Hz时。芯片内部软件算法对STEP输入信号滤波处理。        
        
        
        uint8_t test_mode                   : 1;    // 0：正常模式。这位通常置0。
	};
}__GCONF;



typedef union // 用来描述 IHOLD_IRUN(0x10) 的数据字
{
	uint32_t Value;
	struct
	{
		// 低位在前，高位在后
		uint8_t IHOLD               : 5;    // 调整电机运行电流，数值范围0...31。调整这里可改变电机转动时的扭矩。
        uint8_t reserved_1          : 3;    // 保留位
		uint8_t IRUN                : 5;    // 调整电机待机电流，数值范围0...31。调整这里可改变电机待机时的扭矩，可设置为IRUN的一半。
        uint8_t reserved_2          : 3;    // 保留位
		uint8_t IHOLDDELAY          : 4;    // 调整运行电流到待机电流的延时，数值范围0...15。如果设为0，是瞬间断电。平滑过渡可避免断电时电机抖动。
        uint8_t reserved_3          : 4;    // 保留位
	};
}__IHOLD_IRUN;

typedef struct
{
    __GCONF         GCONF;
    __IHOLD_IRUN    IHOLD_IRUN;
//    __CHOPCONF      CHOPCONF;
}__TMC2209;

extern __TMC2209 TMC2209;

#endif /* TMC2209_Register */
