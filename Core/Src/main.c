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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define START_COMMAND 'S'   // Command to start communication
#define STOP_COMMAND  'E'
#define RESPONSE_NUMBER 1   // Response number to send back
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define RX_BUFFER_SIZE 10
uint8_t rx_byte;
uint8_t uart_rx_buffer[RX_BUFFER_SIZE];  // Almacena la cadena recibida
uint8_t temp_index = 0;            // Indice del buffer

volatile uint8_t wave_type = 0; // 0 = Sinusoidal, 1 = Square, 2 = Sawtooth, 3 = Triangular

//#define QUARTER_WAVE_POINTS 50 // Points for one-quarter of the wave
//const uint16_t sine_wave[QUARTER_WAVE_POINTS] = {
//		2047, 2111, 2175, 2239, 2303, 2367, 2430, 2493, 2556, 2618, 2679, 2740, 2800, 2859, 2918, 2976, 3033, 3089, 3143, 3197, 3250, 3301, 3351, 3400, 3448, 3494, 3539, 3582, 3624, 3664, 3703, 3740, 3775, 3808, 3840, 3870, 3899, 3925, 3950, 3972, 3993, 4012, 4029, 4044, 4057, 4068, 4077, 4084, 4089, 4092
//		};

#define SINE_WAVE_POINTS 200
const uint16_t sine_wave[SINE_WAVE_POINTS] = {
		1737, 1785, 1834, 1882, 1931, 1979, 2027, 2075, 2122, 2169, 2216, 2262, 2307, 2352, 2397, 2441, 2484, 2526, 2568, 2608, 2648, 2687, 2725, 2762, 2798, 2833, 2867, 2900, 2932, 2962, 2991, 3019, 3046, 3072, 3096, 3118, 3140, 3160, 3179, 3196, 3212, 3226, 3239, 3250, 3260, 3268, 3275, 3281, 3284, 3287, 3288, 3287, 3284, 3281, 3275, 3268, 3260, 3250, 3239, 3226, 3212, 3196, 3179, 3160, 3140, 3118, 3096, 3072, 3046, 3019, 2991, 2962, 2932, 2900, 2867, 2833, 2798, 2762, 2725, 2687, 2648, 2608, 2568, 2526, 2484, 2441, 2397, 2352, 2307, 2262, 2216, 2169, 2122, 2075, 2027, 1979, 1931, 1882, 1834, 1785, 1737, 1688, 1639, 1591,
		1542, 1494, 1446, 1398, 1351, 1304, 1257, 1211, 1166, 1121, 1076, 1032, 989, 947, 905, 865, 825, 786, 748, 711, 675, 640, 606, 573, 541, 511, 482, 454, 427, 401, 377, 355, 333, 313, 294, 277, 261, 247, 234, 223, 213, 205, 198, 192, 189, 186, 186, 186, 189, 192, 198, 205, 213, 223, 234, 247, 261, 277, 294, 313, 333, 355, 377, 401, 427, 454, 482, 511, 541, 573, 606, 640, 675, 711, 748, 786, 825, 865, 905, 947, 989, 1032, 1076, 1121, 1166, 1211, 1257, 1304, 1351, 1398, 1446, 1494, 1542, 1591, 1639, 1688
		};

// TO GENERATE WITH 1.67V amplitude
//const uint16_t sine_wave[SINE_WAVE_POINTS] = {
//		2047, 2108, 2169, 2230, 2291, 2351, 2412, 2471, 2531, 2590, 2648, 2706, 2764, 2820, 2876, 2931, 2985, 3038, 3090, 3141, 3192, 3240, 3288, 3335, 3380, 3424, 3467, 3508, 3547, 3586, 3622, 3658, 3691, 3723, 3754, 3782, 3809, 3834, 3858, 3879, 3899, 3917, 3933, 3948, 3960, 3971, 3979, 3986, 3991, 3994, 3995, 3994, 3991, 3986, 3979, 3971, 3960, 3948, 3933, 3917, 3899, 3879, 3858, 3834, 3809, 3782, 3754, 3723, 3691, 3658, 3622, 3586, 3547, 3508, 3467, 3424, 3380, 3335, 3288, 3240, 3192, 3141, 3090, 3038, 2985, 2931, 2876, 2820, 2764, 2706, 2648, 2590, 2531, 2471, 2412, 2351, 2291, 2230, 2169, 2108, 2047, 1985, 1924, 1863, 1802, 1742, 1681, 1622, 1562, 1503, 1445, 1387, 1329, 1273, 1217, 1162, 1108, 1055, 1003, 952, 901, 853, 805, 758, 713, 669, 626, 585, 546,
//		507, 471, 435, 402, 370, 339, 311, 284, 259, 235, 214, 194, 176, 160, 145, 133, 122, 114, 107, 102, 99, 99, 99, 102, 107, 114, 122, 133, 145, 160, 176, 194, 214, 235, 259, 284, 311, 339, 370, 402, 435, 471, 507, 546, 585, 626, 669, 713, 758, 805, 853, 901, 952, 1003, 1055, 1108, 1162, 1217, 1273, 1329, 1387, 1445, 1503, 1562, 1622, 1681, 1742, 1802, 1863, 1924, 1985
//		};

#define SAWTOOTH_POINTS 200
const uint16_t sawtooth_wave[SAWTOOTH_POINTS] = {
		186, 201, 217, 232, 248, 263, 279, 295, 310, 326, 341, 357, 373, 388, 404, 419, 435, 450, 466, 482, 497, 513, 528, 544, 560, 575, 591, 606, 622, 638, 653, 669, 684, 700, 715, 731, 747, 762, 778, 793, 809, 825, 840, 856, 871, 887, 903, 918, 934, 949, 965, 980, 996, 1012, 1027, 1043, 1058, 1074, 1090, 1105, 1121, 1136, 1152, 1168, 1183, 1199, 1214, 1230, 1245, 1261, 1277, 1292, 1308, 1323, 1339, 1355, 1370, 1386, 1401, 1417, 1433, 1448, 1464, 1479, 1495, 1510, 1526, 1542, 1557, 1573, 1588, 1604, 1620, 1635, 1651, 1666, 1682, 1698, 1713, 1729, 1744, 1760, 1775, 1791, 1807, 1822, 1838, 1853, 1869, 1885, 1900, 1916, 1931, 1947, 1963, 1978, 1994, 2009, 2025, 2040, 2056, 2072, 2087, 2103, 2118, 2134, 2150, 2165, 2181, 2196, 2212, 2228, 2243, 2259, 2274, 2290, 2305, 2321, 2337, 2352, 2368, 2383, 2399, 2415, 2430, 2446, 2461, 2477, 2493, 2508, 2524, 2539, 2555, 2570, 2586, 2602, 2617, 2633, 2648, 2664, 2680, 2695, 2711, 2726, 2742, 2758, 2773, 2789, 2804, 2820, 2835, 2851, 2867, 2882, 2898, 2913, 2929, 2945, 2960, 2976, 2991, 3007, 3023, 3038, 3054, 3069, 3085, 3100, 3116, 3132, 3147, 3163, 3178, 3194, 3210, 3225, 3241, 3256, 3272, 3288
};

const uint16_t square_wave[2] = {0, 4095};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DAC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void MCP41100_SetValue(uint8_t value, uint8_t sensor);
uint8_t calculateValuePotentiometer(uint8_t amplitude);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void WaitForConnection(void)
{
    const char msg[] = "IPMC_READY\n";
    uint8_t received_byte = 0;

    while (received_byte != START_COMMAND)
    {
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, sizeof(msg) - 1, HAL_MAX_DELAY);
        HAL_UART_Receive(&huart2, &received_byte, 1, 1000);  // Timeout de 1 segundo
        if (received_byte != START_COMMAND) {
        	HAL_Delay(1000);
        }
    }
    uint8_t response = RESPONSE_NUMBER;
	HAL_UART_Transmit(&huart2, &response, sizeof(response), HAL_MAX_DELAY);
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
  MX_USART2_UART_Init();
  MX_DAC1_Init();
  MX_TIM2_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);

//    uint8_t buff_start = 0;
//    while(buff_start != START_COMMAND){
//    	  HAL_UART_Receive(&huart2, (uint8_t*)(&buff_start),sizeof(buff_start),1000);
//    }
//    uint8_t response = RESPONSE_NUMBER;
//    HAL_UART_Transmit(&huart2, &response, sizeof(response), HAL_MAX_DELAY);
    WaitForConnection();

  /* Start the Timer in Interrupt mode */
    HAL_TIM_Base_Start_IT(&htim2);

    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
  /* USER CODE END 2 */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_TIM2
                              |RCC_PERIPHCLK_TIM34;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Tim2ClockSelection = RCC_TIM2CLK_HCLK;
  PeriphClkInit.Tim34ClockSelection = RCC_TIM34CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */
  /* USER CODE END DAC1_Init 1 */

  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 11999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 11999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
uint16_t get_sine_value() {
    static uint16_t index = 0;

    uint16_t value = sine_wave[index];

    // Increment the index and wrap around for the next cycle
    index = (index + 1) % SINE_WAVE_POINTS;

    return value;
}


uint16_t get_sawtooth_value() {
    static uint8_t index = 0;
    uint16_t value = sawtooth_wave[index];
    index = (index + 1) % SAWTOOTH_POINTS;  // Increment index and restart at startup
    return value;
}

uint16_t get_triangular_value() {
    static uint8_t index = 0;       // Current index in the wave table
    static int8_t direction = 1;    // Direction: 1 for ascending, -1 for descending

    // Get the current value from the sawtooth wave table
    uint16_t value = sawtooth_wave[index];

    // Update the index based on the current direction
    if (direction == 1) { // Ascending
        if (index == SAWTOOTH_POINTS - 2) { // If at or near the maximum value
            direction = -1; // Switch to descending
            index -= 2;     // Ensure the next step starts descending
        } else {
            index += 2; // Move to the next value
        }
    } else { // Descending
        if (index <= 1) { // If at or near the minimum value
            direction = 1; // Switch to ascending
            index += 2;    // Ensure the next step starts ascending
        } else {
            index-=2; // Move to the previous value
        }
    }

    return value; // Return the current wave value
}

uint16_t get_square_value() {
	static uint8_t counter = 0; // Counter to track the current step

	// Determine value directly based on counter
	uint16_t value = (counter < SAWTOOTH_POINTS / 2) ? 3288 : 0;

	// Increment counter and wrap around at the end of the cycle
	counter = (counter + 1) % SAWTOOTH_POINTS;

	// Return the current value
	return value;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	uint16_t valueAct;
	switch (wave_type) {
		case 0: // Sinusoidal
			valueAct = get_sine_value();
			break;
		case 1: // Square
			valueAct = get_square_value();
			break;
		case 2: // Sawtooth
			valueAct = get_sawtooth_value();
			break;
		case 3: // Triangular
			valueAct = get_triangular_value();
			break;
		default:
			valueAct = 0;
			break;
	}
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1, DAC_ALIGN_12B_R, valueAct);
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_2, DAC_ALIGN_12B_R, valueAct+25); // Añado 25 para que salgan identicas
//	HAL_DACEx_DualSetValue(&hdac1, DAC_ALIGN_12B_R, valueAct, valueAct);
}


void parse_command(uint8_t *command) {
    // Check if it's a frequency command
	switch (command[0]) {
		case 'T':{
			wave_type = atoi((char *)(command + 2));
//			uint8_t response = 1;
//			HAL_UART_Transmit(&huart2, &response, 1, HAL_MAX_DELAY); // Send 1 (success)
			break;
		}

	    case 'F': {
	        // Handle frequency command
	        uint16_t frequency;
	        frequency = atoi((char *)(command + 2)); // Convert string to integer
	        __HAL_TIM_SET_AUTORELOAD(&htim2, 3599999UL / frequency);
	        uint8_t response = 1;
	        HAL_UART_Transmit(&huart2, &response, 1, HAL_MAX_DELAY); // Send 1 (success)
	        break;
	    }

	    case 'A': {
	        // Handle amplitude command
	        uint8_t amplitude;
	        amplitude = atoi((char *)(command + 2)); // Convert string to integer
	        //////////////////// AQUI PONER EL CODIGO NECESARIO PARA CAMBIAR LA AMPLITUD
	        uint8_t stepPot;
	        stepPot = calculateValuePotentiometer(amplitude);
	        if (command[1] == '1'){
	        	MCP41100_SetValue(stepPot, 1);
			}
			if (command[1] == '2'){
				MCP41100_SetValue(stepPot, 2);
			}
			if (command[1] == '0'){
				MCP41100_SetValue(stepPot, 0);
			}
	        uint8_t response = 1;
	        HAL_UART_Transmit(&huart2, &response, 1, HAL_MAX_DELAY); // Send 1 (success)
	        break;
	    }

	    default:
	        // Handle invalid command
	    	uint8_t response = 0;
	        HAL_UART_Transmit(&huart2, &response, 1, HAL_MAX_DELAY); // Send 0 (error)
	        break;
	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
//        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);

        if (rx_byte == STOP_COMMAND) {
        	uint8_t response = RESPONSE_NUMBER;
        	HAL_UART_Transmit(&huart2, &response, sizeof(response), HAL_MAX_DELAY);
        	NVIC_SystemReset();
        }

        if (rx_byte == '\n') {        // Detectar fin de la cadena (por ejemplo, '\n')
			uart_rx_buffer[temp_index] = '\0';  // Terminar la cadena
			parse_command(uart_rx_buffer);

			temp_index = 0;  // Reiniciar índice para el siguiente número
		} else {
			if (temp_index < RX_BUFFER_SIZE - 1) {
				uart_rx_buffer[temp_index++] = rx_byte;  // Almacenar el carácter en el buffer
			} else {
				// Buffer lleno, manejar error
				char error_msg[] = "Error: Buffer overflow\r\n";
				HAL_UART_Transmit(&huart2, (uint8_t *)error_msg, sizeof(error_msg) - 1, HAL_MAX_DELAY);
				temp_index = 0;  // Reiniciar el buffer
			}
		}
        HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_byte, 1);
    }
}

void MCP41100_SetValue(uint8_t value, uint8_t sensor) {
    // Asegúrate de que el valor esté en el rango de 0 a 255
    if (value > 255) value = 255;

    // Prepara los datos a enviar
    uint8_t data[2];
    data[0] = 0x11; // Comando para escribir al Potenciómetro 0
    data[1] = value; // Valor del potenciómetro
    if (sensor == 1 || sensor == 0){
		// Baja NSS (selecciona el dispositivo)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		// Envía los datos al MCP41100
		HAL_SPI_Transmit(&hspi2, data, 2, HAL_MAX_DELAY);
		// Sube NSS (deselecciona el dispositivo)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    }
    if (sensor == 2 || sensor == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi2, data, 2, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    }
}

uint8_t calculateValuePotentiometer(uint8_t amplitude) {
    // Define circuit parameters
    const float V_in_pp = 2.47;          // Input peak-to-peak voltage
    const float V_out_pp_max = 20.0;     // Maximum output peak-to-peak voltage
    const float A_v_max = V_out_pp_max / V_in_pp; // Maximum gain (8.1)
    // Validate that the received value is within the allowed range (0 to 20)
    if (amplitude > 20) {
        amplitude = 20; // Limit to the maximum allowed
    }

    // Calculate the desired gain from the amplitude
    float desired_gain = 1.0 + ((float)amplitude / 20.0) * (A_v_max - 1.0);

    // Calculate the potentiometer step
    uint8_t step = (uint8_t)(255.0 / (desired_gain - 1.0));

    // Return the calculated step
    return step;
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
