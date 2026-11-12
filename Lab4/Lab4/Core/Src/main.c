/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stm32l4s5i_iot01.h"
#include "hts221.h"
#include "lis3mdl.h"
#include "lps22hb.h"
#include "lsm6dsl.h"
#include <string.h>
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
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart1;

osThreadId defaultTaskHandle;
osThreadId transmit_to_UARHandle;
osThreadId read_sensorHandle;
/* USER CODE BEGIN PV */
osThreadId sensorTaskHandle;
osThreadId uartTaskHandle;
osMutexId  msgMutexHandle;
osSemaphoreId buttonSemHandle;
osSemaphoreId dataReadySemHandle;

static char msg_buf[64];   // text shared between sensor task and UART task
static int  msg_len = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void const * argument);
void StartUartTask(void const * argument);
void StartSensorTask(void const * argument);

/* USER CODE BEGIN PFP */
void StartSensorTask(void const * argument);
void StartUartTask(void const * argument);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef enum {MODE_HTS221, MODE_LIS3MDL, MODE_LSM6DSL, MODE_LPS22HB} SensorMode;
volatile SensorMode sensor_mode = MODE_HTS221; 

float ReadHumidity(void)
{
  return HTS221_H_ReadHumidity(HTS221_I2C_ADDRESS);
}

void ReadMagnetometer(int16_t m[3])
{
  LIS3MDL_MagReadXYZ(m);
}

void ReadGyroscope(float g[3])
{
  LSM6DSL_GyroReadXYZAngRate(g);
}

float ReadPressure(void)
{
  return LPS22HB_P_ReadPressure(LPS22HB_I2C_ADDRESS);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

  if (GPIO_Pin == GPIO_PIN_13)
  {
    static uint32_t lastPress = 0; 
    uint32_t currentPress = HAL_GetTick(); // get the current time in milliseconds
    if (currentPress - lastPress > 50 && buttonSemHandle != NULL) { // debounce and check if semaphore is valid
      osSemaphoreRelease(buttonSemHandle); //wake button task
    }
    lastPress = currentPress; // update the last press time
  }
}

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
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  SENSOR_IO_Init();

  HTS221_H_Init(HTS221_I2C_ADDRESS);
  HTS221_T_Init(HTS221_I2C_ADDRESS, NULL);
  LPS22HB_P_Init(LPS22HB_I2C_ADDRESS);
  LPS22HB_T_Init(LPS22HB_I2C_ADDRESS, NULL);

  LSM6DSL_AccInit(
      ((uint16_t)LSM6DSL_BDU_BLOCK_UPDATE | LSM6DSL_ACC_GYRO_IF_INC_ENABLED << 8) |
      LSM6DSL_ODR_104Hz |
      LSM6DSL_ACC_FULLSCALE_2G);
  LSM6DSL_GyroInit(
      ((uint16_t)LSM6DSL_BDU_BLOCK_UPDATE | LSM6DSL_ACC_GYRO_IF_INC_ENABLED << 8) |
      LSM6DSL_ODR_104Hz |
      LSM6DSL_GYRO_FS_245);

  MAGNETO_InitTypeDef magnetometer_init = {
      .Register1 = LIS3MDL_MAG_TEMPSENSOR_ENABLE |
                   LIS3MDL_MAG_OM_XY_HIGH |
                   LIS3MDL_MAG_ODR_80_HZ,
      .Register2 = LIS3MDL_MAG_FS_4_GA,
      .Register3 = LIS3MDL_MAG_CONTINUOUS_MODE,
      .Register4 = LIS3MDL_MAG_OM_Z_HIGH,
      .Register5 = LIS3MDL_MAG_BDU_MSBLSB};
  LIS3MDL_MagInit(magnetometer_init);  
  
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  osMutexDef(msgMutex);
  msgMutexHandle = osMutexCreate(osMutex(msgMutex));
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  osSemaphoreDef(buttonSem);
  buttonSemHandle = osSemaphoreCreate(osSemaphore(buttonSem), 1);
  osSemaphoreWait(buttonSemHandle, 0); // start empty

  osSemaphoreDef(dataReadySem);
  dataReadySemHandle = osSemaphoreCreate(osSemaphore(dataReadySem), 1);
  osSemaphoreWait(dataReadySemHandle, 0); // start empty
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of transmit_to_UAR */
  osThreadDef(transmit_to_UAR, StartUartTask, osPriorityNormal, 0, 512);
  transmit_to_UARHandle = osThreadCreate(osThread(transmit_to_UAR), NULL);

  /* definition and creation of read_sensor */
  osThreadDef(read_sensor, StartSensorTask, osPriorityNormal, 0, 512);
  read_sensorHandle = osThreadCreate(osThread(read_sensor), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  // osThreadDef(sensorTask, StartSensorTask, osPriorityNormal, 0, 512);
  // sensorTaskHandle = osThreadCreate(osThread(sensorTask), NULL);

  // osThreadDef(uartTask, StartUartTask, osPriorityNormal, 0, 512);
  // uartTaskHandle = osThreadCreate(osThread(uartTask), NULL);
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00100D14;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
    if(osSemaphoreWait(buttonSemHandle, osWaitForever) == osOK) {
      sensor_mode = (sensor_mode + 1) % 4;
    }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartUartTask */
/**
* @brief Function implementing the transmit_to_UAR thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartTask */
void StartUartTask(void const * argument)
{
  /* USER CODE BEGIN StartUartTask */
  char buf[64];
  int len;

  for(;;) {
    osDelay(1);

    //wait for data to be ready from the sensor task, when osOK -> data is ready, got the semaphore
    if(osSemaphoreWait(dataReadySemHandle, osWaitForever) == osOK) {
      osMutexWait(msgMutexHandle, osWaitForever);
      len = msg_len;
      memcpy(buf, msg_buf, len);
      osMutexRelease(msgMutexHandle);
      HAL_UART_Transmit(&huart1, (uint8_t *)buf, len, HAL_MAX_DELAY);
    }

  }
  /* USER CODE END StartUartTask */
}

/* USER CODE BEGIN Header_StartSensorTask */
/**
* @brief Function implementing the read_sensor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSensorTask */
void StartSensorTask(void const * argument)
{
  /* USER CODE BEGIN StartSensorTask */
  char buf[64];
  int len;

  for(;;) {
    osDelay(100);

    float humidity = ReadHumidity();

    int16_t m[3];
    ReadMagnetometer(m);

    float g[3];
    ReadGyroscope(g);
    
    float pressure = ReadPressure();

    len = 0;
    switch (sensor_mode) {
      case MODE_HTS221: {
        len = snprintf(buf, sizeof buf, "Humidity: %d\r\n", (int)(humidity * 100));
        break;
      }
      case MODE_LIS3MDL: {
        len = snprintf(buf, sizeof buf, "Magnitude: %d, %d, %d\r\n", m[0], m[1], m[2]);
        break;
      }
      case MODE_LSM6DSL: {
        len = snprintf(buf, sizeof buf, "Gyroscope: %d, %d, %d\r\n", (int)g[0], (int)g[1], (int)g[2]);
        break;
      }
      case MODE_LPS22HB: {
        len = snprintf(buf, sizeof buf, "Pressure: %d\r\n", (int)(pressure * 100));
        break;
      }
      default:
        break;
    }

    if (len > 0) {
      if (len >= (int)sizeof buf) {
        len = sizeof buf - 1;
      } 
      osMutexWait(msgMutexHandle, osWaitForever);
      memcpy(msg_buf, buf, len);
      msg_len = len;
      osMutexRelease(msgMutexHandle);
      osSemaphoreRelease(dataReadySemHandle);
    }
  }
  /* USER CODE END StartSensorTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
