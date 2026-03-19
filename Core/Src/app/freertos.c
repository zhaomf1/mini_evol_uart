/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "modbus_rtu.h"
#include "app_control.h"
#include "usart_comm.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
//任务句柄
osThreadId_t defaultTaskHandle;
osThreadId_t uartCommTaskHandle;        //与上位机通讯任务句柄
osThreadId_t timerTaskHandle;           //定时器任务
osThreadId_t phCtrlTaskHandle;          //PH流程控制任务
osThreadId_t appInitTaskHandle;         //应用初始化任务

//消息队列句柄
osMessageQueueId_t uartRxQueueHandle;   //串口接收消息队列

const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    // .stack_size = 128 * 4,
    .stack_size = 256 * 4,      //printf占用栈空间，先设置大点便于调试
    .priority = (osPriority_t)osPriorityNormal,
};

const osThreadAttr_t uartCommTask_attributes = {
    .name = "uartCommTask",
    // .stack_size = 128 * 4,
    .stack_size = 256 * 4,      //printf占用栈空间，先设置大点便于调试
    .priority = (osPriority_t)osPriorityNormal,
};

const osThreadAttr_t timerTask_attributes = {
    .name = "timerTask",
    .stack_size =256,
    .priority = (osPriority_t)osPriorityNormal,
};

const osThreadAttr_t phCtrlTask_attributes = {
    .name = "phCtrlTask",
    .stack_size =256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

const osThreadAttr_t appInitTask_attributes = {
    .name = "appInitTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    modbus_create_rx_semaphore();     //初始化串口3接收信号量
    modbus_create_mutex();   //modbus初始化，初始化互斥锁

    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
     uartRxQueueHandle = osMessageQueueNew(10, sizeof(UartMsg_t), NULL);                    //创建串口接收消息队列
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);       //创建默认任务
    uartCommTaskHandle = osThreadNew(uart_comm_task, NULL, &uartCommTask_attributes);       //创建与上位机通讯任务
    timerTaskHandle = osThreadNew(TimerTask, NULL, &timerTask_attributes);                  //定时器任务
    phCtrlTaskHandle = osThreadNew(phControlTask, NULL, &phCtrlTask_attributes);            //ph流程任务
    appInitTaskHandle = osThreadNew(appInitTask, NULL, &appInitTask_attributes);            //应用初始化任务
    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    uint8_t first_enter = 1;
    uint16_t step_cnt = 0;
    uint16_t color_cnt = 0;


    /* Infinite loop */
    for (;;)
    {
        // 喂狗
        extern IWDG_HandleTypeDef hiwdg;
        HAL_IWDG_Refresh(&hiwdg);

        // OD测试程序
        {
            #if 0
                extern int od_ctrl_read_value(uint16_t *od);
                uint16_t od = 0;
                int ret = od_ctrl_read_value(&od);
                printf("OD value = %d\n",od);
            
                osDelay(100);
            #endif
        }

        //直流无刷电机测试程序
        {
            #if 0
            extern int bldc_ctrl_enable(void);
            extern int bldc_ctrl_set_speed(ModbusAddr_t bldc_addr,uint16_t speed);
            extern int bldc_ctrl_set_dir(uint16_t dir);
            extern int bldc_ctrl_switch(uint16_t switch_ctrl);
            extern int bldc_ctrl_set_speed_up_time(uint16_t time);
            extern int bldc_ctrl_set_slow_down_time(uint16_t time);
            extern int bldc_ctrl_set_protocol_trans(uint16_t trans);

            if(first_enter)
            {
                bldc_ctrl_enable();
                bldc_ctrl_set_protocol_trans(0);
                bldc_ctrl_set_speed(MODBUS_ADDR_TRAIN_BLDC,5000);
                bldc_ctrl_set_dir(0);
                bldc_ctrl_set_speed_up_time(8);
                bldc_ctrl_set_slow_down_time(8);
                bldc_ctrl_switch(1);

            }
            
            static uint16_t cnt = 0;

            bldc_ctrl_set_speed(MODBUS_ADDR_TRAIN_BLDC,cnt);

            cnt += 50;
            if(cnt >= 2500)
                cnt = 0;

            osDelay(100);
            #endif
        }
        

        //温控测试程序
        {
            #if 0
            extern int temp_ctrl_set_temperature(uint16_t temperature);
            extern int temp_ctrl_read_temperature(uint16_t *temperature);
            extern int temp_ctrl_read_alarm(void);
            extern int temp_ctrl_set_timeout(uint16_t second);

            if(first_enter)
            {
                temp_ctrl_set_temperature(1500);    //设定10摄氏度
                temp_ctrl_set_timeout(3600);   
            }

            uint16_t temperature = 0;
            temp_ctrl_read_temperature(&temperature);
            double f_temp = (double )temperature / 100.0;
            printf("tempearture = %.2f\n",f_temp);
            temp_ctrl_read_alarm();

            #endif
        }

        //PH测试程序
        {
            #if 0
            extern int ph_ctrl_read_value(float *od);
            float ph = 0;
            int ret = ph_ctrl_read_value(&ph);
            // printf("PH value = %.2f\n",ph);
            #endif
        }

        //RGB测试程序
        {
            #if 0
            if(color_cnt%3 == 0)
            {
                rgb_set_color((RgbColor_t)RGB_RED);
            }
            else if(color_cnt%3 == 1)
            {
                rgb_set_color((RgbColor_t)RGB_GREEN);
            }
            else if(color_cnt%3 == 2)
            {
                rgb_set_color((RgbColor_t)RGB_BLUE);
            }

            color_cnt++;
            if(color_cnt >= 1000)
                color_cnt = 0;
            #endif
        }


        first_enter = 0;
        
        osDelay(500);
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
