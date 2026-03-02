/*
 * TMC2209.h
 *
 *  Created on: 2025
 *      Author: yjj
 */

#ifndef TMC_IC_TMC2209_H_
#define TMC_IC_TMC2209_H_


#include "tmc2209_register.h"
#include "tmc2209_constants.h"
#include "tmc2209_fields.h"
#include "stdint.h"

/*
电机IO口
*/
#define DIR1_Pin 			GPIO_PIN_4
#define DIR1_GPIO_Port 		GPIOC
#define INDEX1_Pin 			GPIO_PIN_5
#define INDEX1_GPIO_Port 	GPIOC
#define INDEX1_EXTI_IRQn 	EXTI2_IRQn
#define DIAG1_Pin 			GPIO_PIN_0
#define DIAG1_GPIO_Port 	GPIOB
#define DIAG1_EXTI_IRQn 	EXTI3_IRQn
#define STEP1_Pin 			GPIO_PIN_9
#define STEP1_GPIO_Port 	GPIOE
#define ENABLE_1_Pin 		GPIO_PIN_1
#define ENABLE_1_GPIO_Port 	GPIOB

#define DIR2_Pin 			GPIO_PIN_7
#define DIR2_GPIO_Port 		GPIOE
#define STEP2_Pin 			GPIO_PIN_5
#define STEP2_GPIO_Port 	GPIOA
#define INDEX2_Pin 			GPIO_PIN_8
#define INDEX2_GPIO_Port 	GPIOE
#define INDEX2_EXTI_IRQn 	EXTI9_5_IRQn
#define DIAG2_Pin 			GPIO_PIN_10
#define DIAG2_GPIO_Port 	GPIOE
#define DIAG2_EXTI_IRQn 	EXTI4_IRQn
#define ENABLE_2_Pin 		GPIO_PIN_11
#define ENABLE_2_GPIO_Port 	GPIOE

#define DIR3_Pin 			GPIO_PIN_12
#define DIR3_GPIO_Port 		GPIOE
#define INDEX3_Pin 			GPIO_PIN_13
#define INDEX3_GPIO_Port 	GPIOE
#define INDEX3_EXTI_IRQn 	EXTI2_IRQn
#define DIAG3_Pin 			GPIO_PIN_13
#define DIAG3_GPIO_Port 	GPIOE
#define DIAG3_EXTI_IRQn 	EXTI3_IRQn
#define STEP3_Pin 			GPIO_PIN_6
#define STEP3_GPIO_Port 	GPIOA
#define ENABLE_3_Pin 		GPIO_PIN_15
#define ENABLE_3_GPIO_Port 	GPIOE

#define DIR4_Pin 			GPIO_PIN_3
#define DIR4_GPIO_Port 		GPIOD
#define INDEX4_Pin 			GPIO_PIN_4
#define INDEX4_GPIO_Port 	GPIOD
#define INDEX4_EXTI_IRQn 	EXTI2_IRQn
#define DIAG4_Pin 			GPIO_PIN_5
#define DIAG4_GPIO_Port 	GPIOD
#define DIAG4_EXTI_IRQn 	EXTI3_IRQn
#define STEP4_Pin 			GPIO_PIN_12
#define STEP4_GPIO_Port 	GPIOD
#define ENABLE_4_Pin 		GPIO_PIN_6
#define ENABLE_4_GPIO_Port 	GPIOD

// Generic mask/shift macros
#define FIELD_GET(data, mask, shift) \
	(((data) & (mask)) >> (shift))
#define FIELD_SET(data, mask, shift, value) \
	(((data) & (~(mask))) | (((value) << (shift)) & (mask)))
    

// 获取某个寄存器的某一个参数
#define TMC2209_FIELD_READ(address, mask, shift) \
	FIELD_GET(TMC2209_readInt(address), mask, shift)
// 修改某个寄存器的某一个参数，其他位保持不变
#define TMC2209_FIELD_UPDATE(address, mask, shift, value) \
	(TMC2209_writeInt(address, FIELD_SET(TMC2209_readInt(address), mask, shift, value)))


// 确保寄存器地址的bit8不是1
#define TMC_ADDRESS(x) ((x) & (TMC2209_ADDRESS_MASK))


// Default Register values
#define R00 0x00000040  // GCONF

#define R10 0x00071703  // IHOLD_IRUN
#define R11 0x00000014  // TPOWERDOWN
#define R6C 0x10000053  // CHOPCONF
#define R70 0xC10D0024  // PWMCONF


extern uint32_t volatile register_value;
extern uint8_t UART_Address;


void TMC2209_writeInt(uint8_t slaveAddress, uint8_t address, int32_t value);
int32_t TMC2209_readInt(uint8_t slaveAddress, uint8_t address);

//void TMC2209_init();
void TMC2209_init(uint8_t slaveAddress,uint8_t slaveIRUN,uint8_t slaveMRES);
#endif /* TMC_IC_TMC2209_H_ */
