/*
 * TMC2209_Constants.h
 *
 *  Created on: 16.01.2019
 *      Author: LK
 */

#ifndef TMC_IC_TMC2209_TMC2209_CONSTANTS_H_
#define TMC_IC_TMC2209_TMC2209_CONSTANTS_H_

#define TMC2209_MOTORS           1
#define TMC2209_REGISTER_COUNT   TMC_REGISTER_COUNT
#define TMC2209_WRITE_BIT        0x80
#define TMC2209_ADDRESS_MASK     0x7F
#define TMC2209_MAX_VELOCITY     s32_MAX
#define TMC2209_MAX_ACCELERATION u24_MAX


enum 
{
	Stealthchop = 0,			//通常翻译成静音模式
	Spreadcycle,				//通常翻译成高速模式，也可以翻译成非静音模式
};  //GCONF(0x00)寄存器的en_pwm_mode位(bit2)


#endif /* TMC_IC_TMC2209_TMC2209_CONSTANTS_H_ */
