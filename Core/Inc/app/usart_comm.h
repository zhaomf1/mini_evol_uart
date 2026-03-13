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
    CMD_RGB_LIGHT,         // RGB灯

    CMD_UNKNOWN = 0xFF,    // 未知指令
} CmdType;

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
    uint16_t step;       // 步数
    uint8_t mode;        // 0-正转，1-反转，2-持续正转，3-持续反转，4-停止
} StepMotorData_t;

// 电磁阀数据结构
typedef struct {
    uint8_t no;         // 电磁阀编号
    uint8_t mode;       // 模式：0x00关，0xFF开
} ValveData_t;

// PH板数据结构
typedef struct {
    uint8_t ph_cmd;     // 0-set_true,1-set_false,2-get_ph,3-set_kb
    uint8_t phTime;     // 时间
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


// 顶层指令结构（包含所有类型）
typedef struct {
    CmdType cmd_type;          // 指令类型
    char error[128];           // 报警信息
    union {                    // 共用体存储不同指令的业务数据
        BldcData_t bldc_motor;
        StepMotorData_t stepp_motor;
        ValveData_t valve;
        PhData_t ph_board;
        OdData_t od_board;
        TemperatureData_t temperature_board;
        RGBLightData_t rgb_light;
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



extern osMessageQueueId_t uartRxQueueHandle;

void uart_comm_task(void *argument);

/**
 * @brief 封装直流无刷电机JSON指令（error在最后一行，用宏定义键名）
 * @param no 电机序号（0-培养电机，1-补料电机）
 * @param speed 电机速度
 * @param mode 电机方向（0-正转，1-反转，2-持续正转，3-持续反转，4-停止）
 * @param error 报警信息（无错误传""）
 * @return 生成的JSON字符串（需用free()释放），失败返回NULL
 */
char* send_bldc_data(uint8_t no, uint16_t speed, uint8_t mode, const char *error);
char* send_steo_motor_data(uint8_t no, uint16_t speed, uint16_t step, uint8_t mode, const char *error);
char* send_valve_data(uint8_t no, uint8_t mode, const char *error);
char* send_ph_data(uint8_t ph_cmd, uint8_t phTime, uint8_t phFactor, float phValue, float setK, float setB, const char *error);


#endif // __USART_COMM_H
