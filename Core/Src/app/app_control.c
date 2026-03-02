#include "gpio.h"
#include "app_control.h"
#include "tmc2209.h"
#include "tim.h"

extern TIM_HandleTypeDef htim1,htim2,htim3,htim4;

const StepMotorHardware_t motor_hw_config[] = {
    // PH泵
    { 
        .htim = &htim1, 
        .channel = TIM_CHANNEL_1, 
        .dir_port = DIR1_GPIO_Port, 
        .dir_pin = DIR1_Pin 
    },
    // 补料泵
    { 
        .htim = &htim2, 
        .channel = TIM_CHANNEL_1, 
        .dir_port = DIR2_GPIO_Port, 
        .dir_pin = DIR2_Pin 
    },
    // 排料泵
    { 
        .htim = &htim3, 
        .channel = TIM_CHANNEL_1, 
        .dir_port = DIR3_GPIO_Port, 
        .dir_pin = DIR3_Pin 
    },
    // 预留泵
    { 
        .htim = &htim4, 
        .channel = TIM_CHANNEL_1, 
        .dir_port = DIR4_GPIO_Port, 
        .dir_pin = DIR4_Pin 
    }
};

//运行状态指示灯
void run_led_on(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}
void run_led_off(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
}
void run_led2_on(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
}

void run_led2_off(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
}

//紫外灯控制
void uv_lamp_on(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
}
void uv_lamp_off(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
}

//照明灯控制
void light_on(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
}
void light_off(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
}

//补料阀控制
void feed_valve_on(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
}
void feed_valve_off(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
}

//排料阀控制
void drain_valve_on(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
}
void drain_valve_off(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
}

//电机初始化
void step_motor_init(void)
{ 
    //定时器初始化
    TIM1_Init(39);
    TIM2_Init(39);
    TIM3_Init(39);
    TIM4_Init(39);
    // 电机1初始化
    TMC2209_init(0,10,4); 
    // 电机2初始化
    TMC2209_init(1,10,4); 
    // 电机3初始化
    TMC2209_init(2,10,4); 
    // 电机4初始化
    TMC2209_init(3,10,4); 
}

/**
 * @brief  步进电机控制函数
 * @note   用于控制指定ID的步进电机的开关、转速和方向
 * @param  id: 电机ID，选择要控制的泵电机 (@ref StepMotorId_t)
 * @param  cmd: 控制命令类型 (@ref StepMotorCmd_t)
 * @param  value: 命令对应的参数值
 * @retval 无
 */
void step_motor_control(StepMotorId_t id, StepMotorCmd_t cmd, uint16_t value)
{
    if (id >= sizeof(motor_hw_config) / sizeof(motor_hw_config[0])) {
        return; 
    }

    const StepMotorHardware_t *hw = &motor_hw_config[id];

    switch(cmd)
    {
        case STEP_MOTOR_CMD_SWITCH:
        {
            // 1=开, 0=关
            if(value) {
                HAL_TIM_PWM_Start(hw->htim, hw->channel); // 注意：这里需要传入TIM句柄，见下文说明
            } else {
                HAL_TIM_PWM_Stop(hw->htim, hw->channel);
            }
            break;
        }

        case STEP_MOTOR_CMD_SPEED:
        {
            // 设置PWM占空比 (CCR寄存器)
            // __HAL_TIM_SET_COMPARE(&htim2, hw->Channel, value); // 同样需要句柄
            break;
        }

        case STEP_MOTOR_CMD_DIR:
        {
            HAL_GPIO_WritePin(hw->dir_port, hw->dir_pin, (GPIO_PinState)value);
            break;
        }
        
        default:
            break;
    }
}
