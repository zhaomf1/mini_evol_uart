#ifndef __APP_CONTROL_H
#define __APP_CONTROL_H
#include "main.h"
#include "stdbool.h"
#include "FreeRTOS.h"

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

//步进电机步数计时器
typedef struct {
    bool active;
    TickType_t start_tick;
    TickType_t timeout_ticks;
    uint8_t func_id;      // 仅用于标识，此处与索引一致
} TimerItem_t;

//PH控制结构体
typedef struct {
    bool active;
    uint8_t ph_factor;      // 0：酸因子 1：碱因子
    float ph_value;         // PH值
    uint8_t ph_time;        // PH搅拌时间
} PhCtrl_t;

//PH K/B设定值
typedef struct{
    float set_k;
    float set_b;
}PhSetKb_t;

typedef enum {
    PH_ACID = 0,    //酸因子
    PH_ALKALI       //碱因子
} PhFactor_t;

typedef enum {
    INIT_SYS_READY,
    INIT_BLDC_PENDING,
    INIT_TEMP_PENDING,
    INIT_ALL_DONE
} InitState_t;

// 定义一些常用颜色
#define RGB_RED     {255, 0, 0}
#define RGB_GREEN   {0, 255, 0}
#define RGB_BLUE    {0, 0, 255}
#define RGB_WHITE   {100, 100, 100}
#define RGB_OFF     {0, 0, 0}
#define RGB_PURPLE  {100, 0, 100}


/***************************************
 *  变量声明
 ***************************************/

/***************************************
 *  公共函数
 ***************************************/
//灯光控制
void run_led_on(void);
void run_led_off(void);
void run_led2_on(void);
void run_led2_off(void);

//紫外灯控制
void uv_lamp_on(void);
void uv_lamp_off(void);

//照明灯控制
void light_on(void);
void light_off(void);

//补料阀控制
void feed_valve_on(void);
void feed_valve_off(void);
//补气阀控制
void air_supply_valve_on(void);
void air_supply_valve_off(void);

//电机控制
void step_motor_init(void);
void step_motor_control(StepMotorId_t id, StepMotorCmd_t cmd, uint16_t value);
void set_step_motor_step_number(StepMotorId_t id, uint32_t steps);
void TimerTask(void *pvParameters); // 定时器任务
void appInitTask(void *pvParameters); //初始化任务
void StartTimer(uint8_t func_id, uint32_t timeout_ms);
void StopTimer(uint8_t func_id);
void set_ph_kb_value(float ph_k, float ph_b);
void get_ph_kb_value(float *ph_k, float *ph_b);

//PH流程控制
void set_ph_ctrl_start(PhCtrl_t ph_data);
void set_ph_ctrl_stop(void);
void phControlTask(void *pvParameters);//ph流程任务

//RGB三色灯控制
void rgb_set_color(RgbColor_t color);
void rgb_switch_ctrl(uint8_t state);

#endif
