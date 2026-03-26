#ifndef __USART_COMM_H
#define __USART_COMM_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "main.h"
#include "cmsis_os.h"

typedef struct {
    uint8_t *data_ptr; // 指向数据的指针
    uint16_t len;      // 数据长度
} UartMsg_t;

// 指令类型枚举（对应cmd字段）
typedef enum {
    CMD_BRUSHLESS_MOTOR = 0,  // 直流无刷电机
    CMD_STEPPER_MOTOR,    // 步进电机
    CMD_SOLENOID_VALVE,   // 电磁阀
    CMD_PH_BOARD,         // PH板
    CMD_OD_BOARD,         // OD板
    CMD_TEMPERATURE,      // 温控
    CMD_RGB_LIGHT,        // RGB灯
    CMD_VERSION,          // 版本号查询


    CMD_UNKNOWN = 0xFF,    // 未知指令
} CmdType_t;

typedef enum{
    VALVE_FEDDING = 0,              //补料阀
    VALVE_AIR_SUPPLY,               //补气阀
    VALVE_FLOODLIGHT,               //照明灯
    VALVE_UV_LAMP,                  //紫外灯
    VALVE_LIGHT_SOURCE_SHUTTER      //光源快门
}ValveType_t;

// 电机动作
typedef enum {
    MOTOR_MODE_CW = 0,           // 正转
    MOTOR_MODE_CCW = 1,          // 反转
    MOTOR_MODE_CW_KEEP = 2,      // 持续正转
    MOTOR_MODE_CCW_KEEP = 3,     // 持续反转
    MOTOR_MODE_STOP = 4,         // 停止
} MotorMode_t;

// 直流无刷电机数据结构
typedef struct {
    uint8_t no;         // 电机序号
    uint16_t speed;     // 转速
    uint8_t mode;       // 0-正转，1-反转，2-持续正转，3-持续反转，4-停止
} BldcData_t;

// 步进电机数据结构
typedef struct {
    uint8_t no;         // 电机序号
    uint16_t speed;      // 转速
    uint32_t step;       // 步数
    uint8_t mode;        // 0-正转，1-反转，2-持续正转，3-持续反转，4-停止
} StepMotorData_t;

// 电磁阀数据结构
typedef struct {
    uint8_t no;         // 电磁阀编号
    uint8_t mode;       // 模式：0x00关，0xFF开
} ValveData_t;

typedef enum{
    VALVE_OPEN = 0XFF,              // 开
    VALVE_CLOSE = 0,          // 关
}ValveState_t;

// PH板数据结构
typedef struct {
    uint8_t ph_cmd;     // 0-set_true,1-set_false,2-get_ph,3-set_kb
    uint8_t phTime;     // 搅拌时间
    uint8_t phFactor;   // 0x00-酸因子,0x01-碱因子
    float phValue;      // PH值
    float setK;         // 校正K值
    float setB;         // 校正B值
} PhData_t;

// OD板数据结构
typedef struct {
    uint16_t odValue;    // OD值（0~65535）
} OdData_t;

// 温控数据结构
typedef struct {
    uint8_t temperature_cmd;    // 指令：0-set_true，1-set_false，2-get
    float temperatureValue;    // 温度值
} TemperatureData_t;

// RGB灯数据结构
typedef struct {
    uint8_t r;          // 红（0-255）
    uint8_t g;          // 绿（0-255）
    uint8_t b;          // 蓝（0-255）
    uint8_t mode;       // 0x00关，0xFF开
} RGBLightData_t;

// 版本信息
typedef struct {
    uint8_t version[50];
} VersionInfo_t;


// 顶层指令结构（包含所有类型）
typedef struct {
    CmdType_t cmd_type;          // 指令类型
    char error[128];           // 报警信息
    union {                    // 共用体存储不同指令的业务数据
        BldcData_t bldc_motor;
        StepMotorData_t stepp_motor;
        ValveData_t valve;
        PhData_t ph_board;
        OdData_t od_board;
        TemperatureData_t temperature_board;
        RGBLightData_t rgb_light;
        VersionInfo_t version_info;
    } data;
} SysCtrlCmd_t;

typedef enum{
    TRAIN_MOTOR = 0,        //培养电机
    FEDDING_MOTOR           //补料电机
}Bldc_type;

typedef enum{
    TEMP_CTRL_OPEN = 0,        //温控开
    TEMP_CTRL_CLOSE,           //温控关
    TEMP_CTRL_GET,             //温度获取
}TempCmd;

typedef enum{
    PH_CMD_OPEN = 0,        //PH开
    PH_CMD_CLOSE,           //PH关
    PH_CMD_GET,             //PH获取
    PH_CMD_SET_KB           //PH校准
}phCmd_t;



extern osMessageQueueId_t uartRxQueueHandle;

void uart_comm_task(void *argument);


#endif // __USART_COMM_H
