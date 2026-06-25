/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "dma.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>  		// 包含fabs()函数的声明
#include "stdio.h"
#include <string.h>
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000); 		//串口打印
  return ch;
}
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

/* USER CODE BEGIN PV */

uint8_t rx_buffer[UART_BUFFER_SIZE],rx_data_backup[UART_BUFFER_SIZE];			//PC通讯变量
uint8_t PH_buffer[UART_BUFFER_SIZE],PH_data_backup[UART_BUFFER_SIZE];			//PH通讯变量

uint8_t ph_error_code = 0;
uint8_t pc_host_connected = 0xFF;	// === 新增：PC连接状态标志（0=已连接，0xFF=未连接） ===

uint8_t Rx_Mark,rx_length,LED_Mark,PH_Mark,PH_Control,Pump_Control,Chip_1,Chip_2,Monitor,Mismark,high_order,low_order;
uint32_t LED_Count;
 
uint16_t Velocity_1,Velocity_2;
uint16_t Frequency_1,Frequency_2;
float Parameter_1=2,Parameter_2=2,Inq_PH,Target_PH;
uint8_t StateFlag_1,StateFlag_2,Type_PH,Stir=10;			
volatile uint8_t Motor_1,Motor_2;
volatile uint32_t Counter_1=0,Counter_2=0;

uint8_t Inquire_PH[8]={0x01,0x03,0x00,0xC8,0x00,0x01,0x05,0xF4},Inquire;				//PH
float ph_k = 1;
float ph_b = 0;	//y=kx+b(x为实际测量PH值)，PH标定参数

static uint32_t start_ph_control_time = 0;			//PH开始调控时间
static uint32_t current_ph_control_time = 0;		//当前PH调控的时间

//如果当前时间-上次通信时间超时，则视为通讯异常，PH控制流程自动停止
static uint32_t last_communication_time = 0;		//上次通信时间
static uint32_t current_system_time = 0;			//当前系统时间

// === 新增：计时激活标志（1=正在计时，0=暂停/停止）===
uint8_t ph_timing_active = 0;

// === 新增：pH连续异常检测相关变量 ===
static float ph_base_value = 0;          /* pH基准值（目标值） */
static uint8_t ph_abnormal_count = 0;    /* 连续异常计数 */
static uint8_t ph_off_limit_count = 0;    /* 连续超出上下限计数 */

uint8_t base_is_set = 0;  // 基准是否已初始化


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  TIM3_Init(39);				//电机2  PWM配置 
  MX_USART1_UART_Init();
//  MX_IWDG_Init();
  MX_TIM5_Init();
  TIM2_Init(39);				//电机1  PWM配置 
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);  												/* 开启空闲中断 */
  HAL_UART_Receive_DMA(&huart1, rx_buffer, UART_BUFFER_SIZE);	  				/* 开启DMA串口接收 */

  __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);  												/* 开启空闲中断 */
  HAL_UART_Receive_DMA(&huart3, PH_buffer, UART_BUFFER_SIZE);	  				/* 开启DMA串口接收 */
	
  HAL_TIM_Base_Start_IT(&htim5);					//启动定时器5
	// printf("****** PH调控 开机 ******\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {		
		uint8_t is_abnormal = 0;	//是否异常
		static float base_inq_ph = 0;

		if(PH_Control==1)				//控制PH
		{
			//检查是否通讯异常
			current_system_time = HAL_GetTick();	//获取当前系统时间
			if(current_system_time - last_communication_time > PH_COMMUNICATION_TIMEOUT)	//如果当前时间-上次通信时间超时，则视为通讯异常
			{
			    PH_Control = 0;	//停止PH控制流程
				ph_timing_active = 0;
				ph_abnormal_count = 0;
				base_is_set = 0;

				continue;	
			}
			// 查询PH
			HAL_UART_Transmit_DMA(&huart3,Inquire_PH,8);			//查询PH
			HAL_Delay(1000);
			if(PH_Mark==1)	//与PH通讯成功
			{
				Monitor=0;
				PH_Mark=0;
				Inq_PH=(float)((PH_data_backup[3]<<8)+PH_data_backup[4])/1000;		//当前实际PH值

				//防错，当PH值在0-14范围内时才进行调控
				if(Inq_PH <= 0 || Inq_PH >= 14)
				{
					ph_off_limit_count++;
					if(ph_off_limit_count >= 3)	//连续三次超出上下限
					{
						ph_timing_active = 0;      // 异常时清零计时
						ph_error_code = 0x01;		//PH值异常
						// === 新增：重置异常计数 ===
						ph_abnormal_count = 0;
						base_is_set = 0;	//等待再次进入PH控制流程后，重新设定基准值
						PH_Control = 0;
						ph_off_limit_count = 0;

						goto skip_ctrl;
					}
				}
				else
				{
					ph_off_limit_count = 0;
				}

				Inq_PH = ph_k*Inq_PH + ph_b;										//PH标定后的PH值	

				int16_t ph_value = (int16_t)(Inq_PH * 1000 + 0.5f);
				// 经过PH标定后数据上报给上位机
				high_order = (ph_value >> 8) & 0xFF;  // 高8位
				low_order = ph_value & 0xFF;          // 低8位

				// ====== 新增：pH连续异常检测逻辑 ======
				{

					
					// 首次运行/上位机每次开启PH控制，初始化基准值
					if (!base_is_set)
					{
						base_inq_ph = Inq_PH;
						base_is_set = 1;
						ph_abnormal_count = 0;
					}
					
					float ph_deviation = Inq_PH - base_inq_ph;  // 当前值与基准值的偏差
					
					if (Type_PH == 1)  // 补酸模式：期望pH下降
					{
						// pH相对于基准值上升超过1为异常（Inq_PH - base >= 1.0）
						if (ph_deviation >= 1.0f)
						{
							is_abnormal = 1;
						}
					}
					else if (Type_PH == 2)  // 补碱模式：期望pH上升
					{
						// pH相对于基准值下降超过1为异常（Inq_PH - base <= -1.0）
						if (ph_deviation <= -1.0f)
						{
							is_abnormal = 1;
						}
					}
					
					if (is_abnormal)
					{
						ph_abnormal_count++;
						
						if (ph_abnormal_count >= 3)	//连续三次出现异常
						{
							ph_error_code = 0x03;  // 新增错误码：pH连续异常波动
							
							// 停止电机和计时
							ph_timing_active = 0;
							HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);

							//重置错误，基准值归位
							base_is_set = 0;	//等待再次进入PH控制流程后，重新设定基准值
							PH_Control = 0;		//关闭PH控制，等待上位机下发指令重新开启
							ph_abnormal_count = 0;
						}

						goto skip_ctrl;		//跳过本轮调控，继续监测

					}
					else
					{
						// 偏差在正常范围内，更新基准值为当前值，重置计数
						base_inq_ph = Inq_PH;
						ph_abnormal_count = 0;
					}
				}
				// ====== pH连续异常检测逻辑结束 ======
				
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);				//方向控制引脚置1表示正向

				// ====== 判断当前是否已经达到目标（达标则停止计时并暂停电机） ======
				uint8_t target_reached = 0;
				if(Type_PH==1)   // 加酸，目标：Inq_PH <= Target_PH
				{
					if(Inq_PH <= Target_PH)
						target_reached = 1;
				}
				else if(Type_PH==2) // 加碱，目标：Inq_PH >= Target_PH
				{
					if(Inq_PH >= Target_PH)
						target_reached = 1;
				}

				if(target_reached)
				{
					// 已经达标，停止本轮计时，并确保电机停止
					ph_timing_active = 0;
					HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);				
				}
				// ====== 达标判断结束 ======
				
				if(Type_PH==1)						//01表示投加因子是酸  			Target_PH 目标值
				{
					if( ( Inq_PH<=Target_PH+0.05f )&&( Inq_PH>Target_PH ) )									// 
					{
						// === 新增：电机启动前，启动或检查计时 ===
						if(ph_timing_active == 0) {
							start_ph_control_time = get_sys_time();
							ph_timing_active = 1;
						} else {
							if(get_sys_time() - start_ph_control_time > PH_CONTROL_TIMEOUT) 
							{
								ph_timing_active = 0;
								ph_error_code= 0x02;base_is_set = 0;PH_Control = 0;ph_abnormal_count = 0;
							}
						}
						HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						HAL_Delay(500);
						HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
					}
					else if( ( Inq_PH<=Target_PH+0.1f )&&( Inq_PH>Target_PH+0.05f ) ) 			// 
					{
						if(ph_timing_active == 0) {
							start_ph_control_time = get_sys_time();
							ph_timing_active = 1;
						} else {
							if(get_sys_time() - start_ph_control_time > PH_CONTROL_TIMEOUT) 
							{
								ph_timing_active = 0;
								ph_error_code= 0x02;base_is_set = 0;PH_Control = 0;ph_abnormal_count = 0;
							}
						}
						HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						HAL_Delay(1000);
						HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
					}
					else if( ( Inq_PH<=Target_PH+0.2f )&&( Inq_PH>Target_PH+0.1f ) ) 				// 
					{
						if(ph_timing_active == 0) {
							start_ph_control_time = get_sys_time();
							ph_timing_active = 1;
						} else {
							if(get_sys_time() - start_ph_control_time > PH_CONTROL_TIMEOUT) 
							{
								ph_timing_active = 0;
								ph_error_code= 0x02;base_is_set = 0;PH_Control = 0;ph_abnormal_count = 0;
							}
						}
						HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						HAL_Delay(2000);
						HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
					}
					else if( Inq_PH>Target_PH+0.2f ) 												// 
					{
						if(ph_timing_active == 0) {
							start_ph_control_time = get_sys_time();
							ph_timing_active = 1;
						} else {
							if(get_sys_time() - start_ph_control_time > PH_CONTROL_TIMEOUT) 
							{
								ph_timing_active = 0;
								ph_error_code= 0x02;base_is_set = 0;PH_Control = 0;ph_abnormal_count = 0;
							}
						}
						HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						HAL_Delay(3000);
						HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
						HAL_Delay(5000);
					}				
					HAL_Delay(Stir*1000);				//延时10秒，混匀
				}
				else if(Type_PH==2)				//02表示投加因子是碱
				{
					if( ( Inq_PH>=Target_PH-0.05f )&&( Inq_PH<Target_PH ) )									// 
					{
						if(ph_timing_active == 0) {
							start_ph_control_time = get_sys_time();
							ph_timing_active = 1;
						} else {
							if(get_sys_time() - start_ph_control_time > PH_CONTROL_TIMEOUT) 
							{
								ph_timing_active = 0;
								ph_error_code= 0x02;base_is_set = 0;PH_Control = 0;ph_abnormal_count = 0;
							}
						}
						HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						HAL_Delay(500);
						HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
					}
					else if( ( Inq_PH>=Target_PH-0.1f )&&( Inq_PH<Target_PH-0.05f ) ) 			// 
					{
						if(ph_timing_active == 0) {
							start_ph_control_time = get_sys_time();
							ph_timing_active = 1;
						} else {
							if(get_sys_time() - start_ph_control_time > PH_CONTROL_TIMEOUT) 
							{
								ph_timing_active = 0;
								ph_error_code= 0x02;base_is_set = 0;PH_Control = 0;ph_abnormal_count = 0;
							}
						}
						HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						HAL_Delay(1000);
						HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
					}
					else if( ( Inq_PH>=Target_PH-0.2f )&&( Inq_PH<Target_PH-0.1f ) ) 				// 
					{
						if(ph_timing_active == 0) {
							start_ph_control_time = get_sys_time();
							ph_timing_active = 1;
						} else {
							if(get_sys_time() - start_ph_control_time > PH_CONTROL_TIMEOUT) 
							{
								ph_timing_active = 0;
								ph_error_code= 0x02;base_is_set = 0;PH_Control = 0;ph_abnormal_count = 0;
							}
						}
						HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						HAL_Delay(2000);
						HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
					}
					else if( Inq_PH<Target_PH-0.2f ) 												// 
					{
						if(ph_timing_active == 0) {
							start_ph_control_time = get_sys_time();
							ph_timing_active = 1;
						} else {
							if(get_sys_time() - start_ph_control_time > PH_CONTROL_TIMEOUT) 
							{
								ph_timing_active = 0;
								ph_error_code= 0x02;base_is_set = 0;PH_Control = 0;ph_abnormal_count = 0;
							}
						}
						HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						HAL_Delay(3000);
						HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
						HAL_Delay(5000);
					}				
					HAL_Delay(Stir*1000);				//延时10秒，混匀
				}
				
			}
			else
			{
				Monitor++;
				if(Monitor>=5)
				{
					Monitor=0;
//					printf("******通讯异常****** \r\n");
					Mismark=1;						//真空泵工作异常，停止置换
				}				
			}
		}

skip_ctrl:
		if(Pump_Control==1)					//控制碱液至瓶口	
		{
			Pump_Control=0;
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);				//方向控制引脚置1表示正向
			HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
			HAL_Delay(20000);					//运行20秒
			HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);	
		}		

		if(Inquire==1)	//不开启控制时 查询PH
		{
			Inquire=0;
			
			HAL_UART_Transmit_DMA(&huart3,Inquire_PH,8);			//查询PH
			HAL_Delay(1000);

			Inq_PH=(float)((PH_data_backup[3]<<8)+PH_data_backup[4])/1000;		//当前实际PH值
			rx_buffer[12] = pc_host_connected;

			Inq_PH = ph_k*Inq_PH + ph_b;										//PH标定后的PH值	

			int16_t ph_value = (int16_t)(Inq_PH * 1000 + 0.5f);

			if(PH_Mark==1)
			{
				Monitor=0;
				PH_Mark=0;
				rx_buffer[5] = (ph_value >> 8) & 0xFF;  // 高8位
				rx_buffer[6] = ph_value & 0xFF;          // 低8位

			}
			else
			{
				Monitor++;
				if(Monitor>=5)
				{
					Monitor=0;
//					printf("******通讯异常****** \r\n");
					Mismark=1;						//真空泵工作异常，停止置换
				}				
			}	
			if(Mismark==1)
			{
				rx_buffer[11]=4;
				Mismark=0;
			}
			rx_buffer[10]=Chip_1+(Chip_2<<1);		//电机异常上报				
			HAL_UART_Transmit_DMA(&huart1, rx_buffer, UART_TRUE_SIZE);					//数据反馈	
		}
		
		/************ 驱动芯片状态检测程序 开始 ************/				
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==1)									//高电平为出错
			Chip_1=1;
		else
			Chip_1=0;
		
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)==1)									//高电平为出错						
			Chip_2=1;
		else
			Chip_2=0;


		/************ 归零后避免撞击程序 结束（可以增加一字节，发送归零信号）************/				
		LED_Count++;
		if(LED_Count%1555990==0)
		{
		/********* LED灯闪烁 开始 **********/		
			LED_Count=0;
			if(LED_Mark==0)
			{
				LED_Mark=1;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
				
			}
			else
			{
				LED_Mark=0;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
				
			}			
		/********* LED灯闪烁 结束 **********/		
		}
		
		/* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/************************* 定时器中断回调函数  开始 *************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	
{
	if(htim==&htim5)
	{
		if(Motor_1==1)							//电机1 启停控制程序
		{
			if(Counter_1>=1)
			{
				HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);				//电机1 PWM输出启动函数
				Counter_1--;																		//步数递减计数
			}
			else
			{
				HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
				Motor_1=0;
				Counter_1=0;
			}
		}
		
		if(Motor_2==1)							//电机2 启停控制程序
		{
			if(Counter_2>=1)
			{
				HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);				//电机2 PWM输出启动函数
				Counter_2--;																		//步数递减计数
			}
			else
			{
				HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);					//电机2 停止输出PWM
				Motor_2=0;
				Counter_2=0;
			}
		}
	}	
}
/************************* 定时器中断回调函数  结束 *************************/

//功能接口函数
//设置PH标定值
void set_ph_calibration_value(float k,float b)
{
	ph_k=k;
	ph_b=b;
	// printf("PH标定值设置成功,k:%.3f,b:%.3f\r\n",ph_k,ph_b);
}

/**
 * @brief  解析3字节有符号定点数为浮点数
 * @param  data: 指向3字节数据的指针，格式为[高字节, 中字节, 低字节]
 * @param  scale: 缩放因子，发送时乘以的倍数（如1000表示精度0.001）
 *         1: 高字节在前 (MSB first)
 *         0: 低字节在前 (LSB first)
 * @return 解析后的浮点数值
 * 
 */
float convert_3byte_signed_to_float(const uint8_t* data, int scale) 
{
    int32_t raw_value;
    
    // 将3字节组合成24位有符号数
	raw_value = (int32_t)((data[0] << 16) | (data[1] << 8) | data[2]);
    
    // 检查24位数的最高位（第23位）是否为1（负数）
    // 0x00800000 = 0000 0000 1000 0000 0000 0000 0000 0000
    if (raw_value & 0x00800000) {
        // 如果是负数，进行符号扩展到32位
        // 方法：将24位数的最高位扩展到32位的最高8位
        raw_value |= 0xFF000000;  // 将高8位全置为1
    } else {
        // 如果是正数，确保高8位为0（清除可能的高位垃圾数据）
        raw_value &= 0x00FFFFFF;  // 只保留低24位
    }
    
    // 转换为浮点数并除以缩放因子
    return (float)raw_value / (float)scale;
}



/**
 * @brief  获取系统运行时间
 * @param  None
 * @return 系统运行时间（毫秒）
 */
uint32_t get_sys_time(void)
{
	return HAL_GetTick();
}

/**
 * @brief  设置PH调控开始时间
 * @return None
 */
void set_ph_control_start_time(void)
{
	start_ph_control_time = get_sys_time();		//记录PH调控开始时间
}

/**
 * @brief 记录上次与上位机通信时间
 * @return None
 */
void record_last_communication_time(void)
{
	last_communication_time = get_sys_time();		//记录上次通信时间
}

/**
 * @brief 更新PH基准值状态
 * @return None
 */
void reset_ph_base_value(void)
{
	base_is_set = 0;	//重置基准值状态，等待下一次进入PH控制流程时重新设定
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */