#ifndef __APP_CONTROL_H
#define __APP_CONTROL_H
#include "main.h"

typedef enum{
    MODBUS_ADDR_TRAIN_BLDC      = 0x02,     // 培养电机
    MODBUS_ADDR_FEEDING_BLDC    = 0x03,     // 补料电机
    MODBUS_ADDR_OD              = 0x20,     // 自研OD模块   
    MODBUS_ADDR_PH              = 0x01,     // PH模块
    MODBUS_ADDR_TEMP            = 0x80      // 温控模块

}ModbusAddr_t;

// 定义电机硬件资源结构体
typedef struct {
    TIM_HandleTypeDef   *htim;    // 定时器实例 (如 TIM2)
    uint32_t            channel;      // PWM通道 (如 TIM_CHANNEL_1)
    GPIO_TypeDef        *dir_port;     // 方向引脚端口
    uint16_t            dir_pin;       // 方向引脚编号
} StepMotorHardware_t;

//步进电机开关
typedef enum {
    STEP_MOTOR_DISABLE = 0,     // 关闭
    STEP_MOTOR_ENABLE           // 开启
} StepMotorEnable_t;

//步进电机方向
typedef enum {
    STEP_MOTOR_REVERSES = 0,    //反转
    STEP_MOTOR_FOREWARD         //正转
} StepMotorDir_t;

// 定义泵的ID（外设选择）
typedef enum {
    STEP_MOTOR_PH = 0,          // PH泵
    STEP_MOTOR_FEED,            // 补料泵
    STEP_MOTOR_DRAIN,           // 排料泵
    STEP_MOTOR_RESERVED         // 预留泵
} StepMotorId_t;
// 命令类型
typedef enum {
    STEP_MOTOR_CMD_SWITCH,   // 开关控制
    STEP_MOTOR_CMD_SPEED,    // 转速控制
    STEP_MOTOR_CMD_DIR       // 方向控制
} StepMotorCmd_t ;

// 定义RGB结构体
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RgbColor_t;

// 定义一些常用颜色
#define RGB_RED     {10, 0, 0}
#define RGB_GREEN   {0, 39, 0}
#define RGB_BLUE    {0, 0, 10}
#define RGB_WHITE   {100, 100, 100}
#define RGB_OFF     {0, 0, 0}
#define RGB_PURPLE  {100, 0, 100}

/***************************************
 *  公共函数
 ***************************************/
//灯光控制
void run_led_on(void);
void run_led_off(void);
void led2_on(void);
void led2_off(void);

//紫外灯控制
void uv_lamp_on(void);
void uv_lamp_off(void);

//照明灯控制
void light_on(void);
void light_off(void);

//补料阀控制
void feed_valve_on(void);
void feed_valve_off(void);
//排料阀控制
void drain_valve_on(void);
void drain_valve_off(void);

//电机控制
void step_motor_init(void);
void step_motor_control(StepMotorId_t id, StepMotorCmd_t cmd, uint16_t value);

//RGB三色灯控制
void rgb_set_color(RgbColor_t color);

#endif
