#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "app_control.h"
#include "tmc2209.h"
#include "tim.h"
#include "task.h"
#include "cmsis_os2.h"
#include "dev_ph_ctrl.h"
#include "dev_temp_ctrl.h"
#include "dev_bldc_ctrl.h"

TimerItem_t timers[4]; //定时器列表
PhCtrl_t ph_ctrl;
PhSetKb_t ph_set_kb;
static uint8_t g_pwm_is_running = 0;    //PWM是否正在运行

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

// 二维数组，每行有两个元素：[PH差值, 电机时间(ms)]
const float ph_time_table[][2] = {
    {0.0f,   0},   // diff < 0         
    {0.05f,  500},   // 0 ≤ diff < 0.05  
    {0.1f,  1000},   // 0.05 ≤ diff < 0.1
    {0.2f,  2000},   // 0.1 ≤ diff < 0.2 
    {16.0f, 3000}    // diff ≥ 0.2       
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

//补气阀控制
void air_supply_valve_on(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
}
void air_supply_valve_off(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
}

//电机初始化，初始化定时器和TMC2209
void step_motor_init(void)
{ 
    //定时器初始化
    TIM1_Init(39);
    TIM2_Init(39);
    TIM3_Init(39);
    TIM4_Init(255);
    // 电机1初始化
    TMC2209_init(0,10,4); 
    // 电机2初始化
    TMC2209_init(1,10,4); 
    // 电机3初始化
    TMC2209_init(2,10,4); 
    // 电机4初始化
    TMC2209_init(3,10,4); 
}

//根据电机ID选择定时器
TIM_HandleTypeDef *get_motor_tim(StepMotorId_t id)
{
    if(id == STEP_MOTOR_PH)
    {
        return &htim1;
    }
    else if(id == STEP_MOTOR_FEED)
    {
        return &htim2;
    }
    else if(id == STEP_MOTOR_DRAIN)
    {
        return &htim3;
    }
	
	return NULL;
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
    uint16_t Period = 0;
    const StepMotorHardware_t *hw = &motor_hw_config[id];

    switch(cmd)
    {
        case STEP_MOTOR_CMD_SWITCH:
        {
            // 1=开, 0=关
            if(value) {
                HAL_TIM_PWM_Start(hw->htim, hw->channel); 
            } else {
                HAL_TIM_PWM_Stop(hw->htim, hw->channel);
            }
            break;
        }

        case STEP_MOTOR_CMD_SPEED:
        {
            //获取定时器句柄
            TIM_HandleTypeDef *htim = get_motor_tim(id);
            //计算自动重装载值
            if(id == STEP_MOTOR_RESERVED)
            {
                Period = 256100 / value;    // 256100/频率 = 自动重装载值
            }else{
                Period = 40000 / value;     // 40000/频率 = 自动重装载值
            }
            // 设置自动重装载值
            __HAL_TIM_SET_AUTORELOAD(htim, Period);
            //设置占空比
            __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, Period/2);
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

//三色灯控制
void rgb_set_color(RgbColor_t color)
{
    if(g_pwm_is_running == 0)
    {
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

        g_pwm_is_running = 1; // 更新状态
    }
    // 设置红色 (TIM4_CH2 -> PD13)
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 255-color.r);
    // 设置绿色 (TIM4_CH3 -> PD14)
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 255-color.g);
    // 设置蓝色 (TIM4_CH4 -> PD15)
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 255-color.b);
}

//三色灯开关控制 0-开 0xff-关
void rgb_switch_ctrl(uint8_t state)
{
    if(state == 0x00)
    {
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
    }
    else if(state == 0xff)
    {
        HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);

        g_pwm_is_running = 0; // 更新状态
    }
}

//设置步进电机步数,运行时间 = 步数/频率 单位ms
void set_step_motor_step_number(StepMotorId_t id, uint16_t steps)
{
    StartTimer(id, steps);
}

/**
 * 启动定时器函数
 * @param func_id 定时器功能ID，范围0-3
 * @param timeout_ms 定时器超时时间，单位毫秒
 */
void StartTimer(uint8_t func_id, uint32_t timeout_ms)
{
    if (func_id >= 4) return;
    TickType_t now = xTaskGetTickCount();
    TickType_t ticks = pdMS_TO_TICKS(timeout_ms);

    taskENTER_CRITICAL();
    timers[func_id].active = true;
    timers[func_id].start_tick = now;
    timers[func_id].timeout_ticks = ticks;
    timers[func_id].func_id = func_id;  
    taskEXIT_CRITICAL();
}

void StopTimer(uint8_t func_id)
{
    if (func_id >= 4) return;
    taskENTER_CRITICAL();
    timers[func_id].active = false;
    taskEXIT_CRITICAL();
}


//定时任务函数
void TimerTask(void *pvParameters)
{
    TickType_t now;
    for (;;)
    {
        now = xTaskGetTickCount();

        for (int i = 0; i < 4; i++)
        {
            if (timers[i].active)
            {
                if ((now - timers[i].start_tick) >= timers[i].timeout_ticks)
                {
                    // 超时，先清除标志（临界区保护）
                    taskENTER_CRITICAL();
                    timers[i].active = false;
                    taskEXIT_CRITICAL();

                    // 处理超时事件（根据 i 区分功能）
                    switch (i) {
                        case STEP_MOTOR_PH: 
                            step_motor_control(STEP_MOTOR_PH, STEP_MOTOR_CMD_SWITCH, STEP_MOTOR_DISABLE);
                            StopTimer(STEP_MOTOR_PH);

                        break;
                        case STEP_MOTOR_FEED:
                            step_motor_control(STEP_MOTOR_FEED, STEP_MOTOR_CMD_SWITCH, STEP_MOTOR_DISABLE);
                            StopTimer(STEP_MOTOR_FEED);

                        break;
                        case STEP_MOTOR_DRAIN:
                            step_motor_control(STEP_MOTOR_DRAIN, STEP_MOTOR_CMD_SWITCH, STEP_MOTOR_DISABLE);
                            StopTimer(STEP_MOTOR_DRAIN);

                        break;
                        case STEP_MOTOR_RESERVED:
                            step_motor_control(STEP_MOTOR_RESERVED, STEP_MOTOR_CMD_SWITCH, STEP_MOTOR_DISABLE);
                            StopTimer(STEP_MOTOR_RESERVED);

                        break;
                    }
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms 检查一次
    }
}

//启动PH流程控制
void set_ph_ctrl_start(PhCtrl_t ph_data)
{
    memcpy(&ph_ctrl, &ph_data, sizeof(PhCtrl_t));
}

//结束PH流程控制
void set_ph_ctrl_stop(void)
{
    ph_ctrl.active = false;
}

bool get_ph_ctrl_state(void)
{
    return ph_ctrl.active;
}

/*
根据当前PH值获取电机运行时间，返回ms
*/
uint16_t get_motor_time(float ph_diff)
{
    uint8_t table_rows = sizeof(ph_time_table) / sizeof(ph_time_table[0]);
    for (int i = 0; i < table_rows; i++) {
        if (ph_diff < ph_time_table[i][0]) {   
            return (uint16_t)ph_time_table[i][1];
        }
    }
	
	return 0;
}

//设定PH标定值
void set_ph_kb_value(float ph_k, float ph_b)
{
    ph_set_kb.set_k = ph_k;
    ph_set_kb.set_b = ph_b;
    printf("set_ph_kb_value: %f, %f",ph_set_kb.set_k,ph_set_kb.set_b);
}

void get_ph_kb_value(float *ph_k, float *ph_b)
{
    *ph_k = ph_set_kb.set_k;
    *ph_b = ph_set_kb.set_b;
}

//PH流程控制任务函数
void phControlTask(void *pvParameters)
{
    float curr_ph = 0;      //实时获取PH值
    float ph_diff = 0;      //PH值差值
    uint16_t run_time = 0;  //电机运行时间

    for (;;)
    {
        if(get_ph_ctrl_state()) //PH控制流程启动
        {
            // 1.获取当前PH值
            ph_ctrl_read_value(&curr_ph);
            curr_ph = curr_ph * ph_set_kb.set_k + ph_set_kb.set_b;  //标定后的PH值
            // 2.根据当前PH值，执行电机运行逻辑
            if(ph_ctrl.ph_factor == PH_ACID)    //酸
            {
                ph_diff = curr_ph - ph_ctrl.ph_value;
            }
            else if(ph_ctrl.ph_factor == PH_ALKALI)     //碱
            {
                ph_diff = ph_ctrl.ph_value - curr_ph;
            }
            run_time = get_motor_time(ph_diff);
            // 3.运行电机
            step_motor_control(STEP_MOTOR_PH,STEP_MOTOR_CMD_SWITCH,STEP_MOTOR_ENABLE);
            step_motor_control(STEP_MOTOR_PH,STEP_MOTOR_CMD_DIR,STEP_MOTOR_FOREWARD);
            set_step_motor_step_number(STEP_MOTOR_PH,run_time);
            printf("PH control: current _ph = %f,ph_diff = %f, run_time = %dms\n",curr_ph,ph_diff,run_time);
            // 4.等待一段时间，PH值稳定下来后，再次测量PH值，重复以上流程
            vTaskDelay(pdMS_TO_TICKS(ph_ctrl.ph_time*1000));


        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

//APP初始化任务，电机
void appInitTask(void *pvParameters)
{
    osDelay(100); //等待系统初始化完成

    dev_bldc_init();
    dev_temp_init();

    extern osThreadId_t appInitTaskHandle;
    osThreadTerminate(appInitTaskHandle); 
}
