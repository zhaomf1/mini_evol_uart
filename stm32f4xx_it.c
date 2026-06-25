/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>				//使用拷贝函数需要此指令
#include "tim.h"
#include "stdio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim5;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */
extern uint8_t Rx_Mark,rx_buffer[UART_BUFFER_SIZE],rx_data_backup[UART_BUFFER_SIZE],PH_buffer[UART_BUFFER_SIZE],PH_data_backup[UART_BUFFER_SIZE],PH_Mark;
extern uint8_t Chip_1,Chip_2,Mismark,PH_Control,Pump_Control,StateFlag_1,StateFlag_2,Motor_1,Motor_2,Type_PH,Stir,Inquire,high_order,low_order;
extern uint16_t Velocity_1,Velocity_2;
extern uint16_t Frequency_1,Frequency_2;
extern float Parameter_1,Parameter_2,Inq_PH,Target_PH;
extern uint32_t Counter_1,Counter_2;
extern uint8_t ph_error_code;
extern uint8_t pc_host_connected;


/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  */
void DMA1_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */

  /* USER CODE END DMA1_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */

  /* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream3 global interrupt.
  */
void DMA1_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_tx);
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream5 global interrupt.
  */
void DMA1_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

  /* USER CODE END DMA1_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

  /* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream6 global interrupt.
  */
void DMA1_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream6_IRQn 0 */

  /* USER CODE END DMA1_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Stream6_IRQn 1 */

  /* USER CODE END DMA1_Stream6_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	
  // 检查是否是空闲中断
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
	{
			// 清除空闲中断标志   若不清除 IDLE 标志，中断会持续触发，导致 CPU 陷入死循环，无法处理其他任务
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);
			
			// 停止DMA接收
			HAL_UART_DMAStop(&huart1);
			
			// 计算接收到的数据长度
			uint8_t COUNTER = UART_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

			if( (COUNTER==UART_TRUE_SIZE)&&(rx_buffer[0]==0x55)&&(rx_buffer[1]==0xAA)&&(rx_buffer[2]==0)&&(rx_buffer[3]==1)&&(rx_buffer[18]==0x5A)&&(rx_buffer[19]==0xA5) )				
			{
				//协议校验通过，更新通信时间
				record_last_communication_time();	//更新上次通信时间

				memcpy(rx_data_backup, rx_buffer, UART_BUFFER_SIZE);	
				rx_data_backup[10]=Chip_1+(Chip_2<<1);			//电机异常上报			
				if(Mismark==1)
				{
					rx_data_backup[11]=4;
					Mismark=0;
				}
				
				if(rx_buffer[4]==0xFF)								//查询版本号
					rx_data_backup[5]=13;							//版本号13表示V1.1.3
				
				if(rx_buffer[4]==15)									//开启控制后 查询PH
				{
					rx_data_backup[5]=high_order;						//高8位
					rx_data_backup[6]=low_order;							//低8位

					rx_data_backup[11] = ph_error_code;						//错误码上报
					if(ph_error_code != 0)
					{
						ph_error_code = 0;	//清除错误码，等待下一次上报
					}

				}
				if(rx_buffer[4]==17)									//不开启控制时 查询PH
				{
					Inquire=1;
				}		
				if(rx_buffer[4]==18)									//设置K,B值
				{
					for(int i=5;i<10;i++)
					{
						rx_data_backup[i]=0;
					}
				}		
				if(rx_buffer[4] == 19)									//上位机首次启用通知
				{
					pc_host_connected = 0;  // 设置PC连接状态为已连接
				}

				rx_data_backup[12] = pc_host_connected;	//=== 新增：上位机连接状态上报 ===

				if(rx_buffer[4]!=17)
					HAL_UART_Transmit_DMA(&huart1, rx_data_backup, UART_TRUE_SIZE);					//数据反馈			
				
				switch(rx_buffer[4])											//电机选择、运动方向控制、行走步数控制
				{
					case 1:					//PH控制  使用电机1
						if(rx_buffer[5]==0xFF)
						{
							PH_Control=1;
							Target_PH=rx_buffer[6]/10.0f;						//PH目标值
							Type_PH=rx_buffer[7];                		//01表示投加因子是酸；02表示投加因子是碱

							extern uint8_t ph_timing_active;
							ph_timing_active = 0;    // <--- 新增，确保新一轮调控计时从零开始
							reset_ph_base_value();	// <--- 新增，确保新一轮调控计时从零开始
							
//							printf("开启PH控制，设定值为：   %.1f    \r\n",Target_PH);
						}
						else
							PH_Control=0;
						
							break;
					
					case 2:					//新耗材提取碱液至瓶口  使用电机1
							Pump_Control=1;
					
							break;				

					case 3:					//电磁阀一  
						if(rx_buffer[5]==0xFF)
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
						else
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);	
							break;		
					case 4:					//电磁阀二  
						if(rx_buffer[5]==0xFF)
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
						else
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);	
							break;	
					case 5:					//电磁阀三  
						if(rx_buffer[5]==0xFF)
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
						else
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);	
							break;		
					case 6:					//电磁阀四  
						if(rx_buffer[5]==0xFF)
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
						else
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);	
							break;
					case 7:					//电磁阀五  
						if(rx_buffer[5]==0xFF)
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
						else
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);	
							break;		
					case 8:					//电磁阀六 
						if(rx_buffer[5]==0xFF)
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
						else
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);	
							break;

					case 9:					//光源挡板一  
						if(rx_buffer[5]==0xFF)		//升起
						{
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);	
	//						HAL_Delay(1000);
	//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);					
						}
						else											//降下
						{
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);	
	//						HAL_Delay(1000);
	//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
						}
					
							break;		
					case 10:					//光源挡板二  
						if(rx_buffer[5]==0xFF)		//升起
						{
							HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);	
	//						HAL_Delay(1000);
	//						HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
	//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);					
						}
						else											//降下
						{
							HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);	
	//						HAL_Delay(1000);
	//						HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
	//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
						}				
					
							break;	
					case 11:					//光源挡板三  
						if(rx_buffer[5]==0xFF)		//升起
						{
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);	
	//						HAL_Delay(1000);
	//						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
	//						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);					
						}
						else											//降下
						{
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);	
	//						HAL_Delay(1000);
	//						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
	//						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
						}					
					
							break;		

					
					/********* 调试指令 *********/
					case 12:					//控制电机1运动 
						Velocity_1=(uint16_t)(rx_buffer[15]<<8)+rx_buffer[16];
						if(Velocity_1>3000)
							Velocity_1=3000;
						Frequency_1=40000/Velocity_1;  					//rx_buffer[8]是频率HZ 		Load_1是重装载值		 PC输入范围是1-10000
						TIM2_Init(Frequency_1-1);								//设置转速 设置1不会转，即Frequency_1最小是4
						Parameter_1=(float)3000/Velocity_1;				//计算步数倍数

						if( (rx_buffer[5]==0xFF) || (rx_buffer[5]==0x00) )			//步数模式运动
						{
							if(rx_buffer[5]==0xFF) 							//方向选择
							{
								HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);			//方向控制引脚置1表示正向
								StateFlag_1=0xFF;
							}
							else if(rx_buffer[5]==0x00)
							{
								HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);			//方向控制引脚置0表示反向 
								StateFlag_1=0x00;
							}
							Counter_1=( ((uint32_t)rx_buffer[6]<<24) + ((uint32_t)rx_buffer[7]<<16) + ((uint32_t)rx_buffer[8]<<8) + rx_buffer[9] )*Parameter_1;		//取步数
							Motor_1=1;									//启动电机1
						}
						else if(rx_buffer[5]==0xA1)			//启停模式运动
						{
							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);				//方向控制引脚置1表示正向
							StateFlag_1=0xFF;
							Motor_1=0;									//不再使用步数计数						
							HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						}
						else if(rx_buffer[5]==0xA2)			//启停模式运动
						{
							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);				//方向控制引脚置1表示反向
							StateFlag_1=0x00;
							Motor_1=0;									//不再使用步数计数						
							HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
						}					
						else if(rx_buffer[5]==0xB1)
						{
							HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
							Motor_1=0;
							Counter_1=0;
						}
						
												
							break;
						
					case 13:											//控制电机2运动
						Velocity_2=(uint16_t)(rx_buffer[15]<<8)+rx_buffer[16];
						if(Velocity_2>3000)
							Velocity_2=3000;
						Frequency_2=40000/Velocity_2;  					//rx_buffer[8]是频率HZ 		Load_1是重装载值		 PC输入范围是1-10000
						TIM3_Init(Frequency_2-1);								//设置转速 设置1不会转，即Frequency_1最小是4
						Parameter_2=(float)3000/Velocity_2;				//计算步数倍数

						if( (rx_buffer[5]==0xFF) || (rx_buffer[5]==0x00) )	//步数模式	
						{
							if(rx_buffer[5]==0xFF)						//方向选择
							{
								HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);			//方向控制引脚置1表示正向
								StateFlag_2=0xFF;
							}
							else if(rx_buffer[5]==0x00)
							{
								HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);			//方向控制引脚置0表示反向						
								StateFlag_2=0x00;
							}
							Counter_2=( ((uint32_t)rx_buffer[6]<<24) + ((uint32_t)rx_buffer[7]<<16) + ((uint32_t)rx_buffer[8]<<8) + rx_buffer[9] )*Parameter_2;	//取步数
							Motor_2=1;											//启动电机2	
						}
						/************ 运动模式选择 结束 **********/								
						else if(rx_buffer[5]==0xA1)		    	//启停模式
						{
							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);		//方向控制引脚置1表示正向
							StateFlag_2=0xFF;
							Motor_2=0;											//不再使用步数计数
							HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);	//电机2 开始输出PWM
						}
						else if(rx_buffer[5]==0xA2)		    	//启停模式
						{
							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);		//方向控制引脚置1表示反向
							StateFlag_2=0x00;
							Motor_2=0;											//不再使用步数计数
							HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);	//电机2 开始输出PWM
						}					
						else if(rx_buffer[5]==0xB1)
						{
							HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);					//电机2 停止输出PWM
							Motor_2=0;
							Counter_2=0;
						}					

							break;

						
					case 14:										//关闭所有电机阀门
					
						HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);					//电机1 停止输出PWM
						Motor_1=0;
						Counter_1=0;

						HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);					//电机2 停止输出PWM
						Motor_2=0;
						Counter_2=0;
					
						/******** 阀门关闭 ********/
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);	
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);	
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);	
						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);	
						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);					
							break;	
					
					case 16:										//设置搅拌时间
					
						Stir=rx_buffer[5];
							break;			
							
					case 18:										//设置ph标定值
						float k = convert_3byte_signed_to_float(&rx_buffer[5],1000);
						float b = convert_3byte_signed_to_float(&rx_buffer[8],1000);
						set_ph_calibration_value(k,b);
						break;

					default: 
						break;
				}
				
				
			}
			else if( (rx_buffer[2]==0)&&(rx_buffer[3]==1) )							//否则返回不正确的数据,并增加错误标记
			{
				rx_buffer[COUNTER]=1;
				rx_buffer[COUNTER+1]=2;
				rx_buffer[COUNTER+2]=3;
				HAL_UART_Transmit_DMA(&huart1, rx_buffer, COUNTER+3);
			}			
			
			// 重新启动DMA接收 
			HAL_UART_Receive_DMA(&huart1, rx_buffer, UART_BUFFER_SIZE);
	}

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
	
  // 检查是否是空闲中断
	if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET)
	{
			// 清除空闲中断标志   若不清除 IDLE 标志，中断会持续触发，导致 CPU 陷入死循环，无法处理其他任务
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);
			
			// 停止DMA接收
			HAL_UART_DMAStop(&huart3);
			
			// 计算接收到的数据长度
			uint8_t COUNTER = UART_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);

			if( (COUNTER==PH_TRUE_SIZE)&&(PH_buffer[0]==0x01)&&(PH_buffer[1]==0x03)&& (PH_buffer[2]==0x02) )				
			{
				PH_Mark=1;			// 标记接收完成
				memcpy(PH_data_backup, PH_buffer, PH_TRUE_SIZE);									//数据转存到rx_buffer
			}
//			else if( (PH_buffer[0]==0x01)&&(PH_buffer[1]==0x03) )							//否则返回不正确的数据,并增加错误标记
//			{
//				PH_buffer[COUNTER]=1;
//				PH_buffer[COUNTER+1]=2;
//				PH_buffer[COUNTER+2]=3;
//				HAL_UART_Transmit_DMA(&huart3, PH_buffer, COUNTER+3);
//			}			
			
			// 重新启动DMA接收 
			HAL_UART_Receive_DMA(&huart3, PH_buffer, UART_BUFFER_SIZE);
	}
	
  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void TIM5_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */

  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&htim5);
  /* USER CODE BEGIN TIM5_IRQn 1 */

  /* USER CODE END TIM5_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void DMA2_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */

  /* USER CODE END DMA2_Stream7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */

  /* USER CODE END DMA2_Stream7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
