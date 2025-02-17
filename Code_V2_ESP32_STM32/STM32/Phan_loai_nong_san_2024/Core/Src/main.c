
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include"string.h"

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
//---------LCD----------
#define Diachi_LCD 0x4E
char dataLcd[100];
char Dulieutruyen_LCD[100];
void Lcd_Ghilenh(char lenh);
void  Lcd_ghidulieu(char lenh);
void Lcd_ghichuoi(char *str);
void lcd_init(void);
void lcd_clear();
void updateDataToLcd();
//--------LCD-----------
//--------UART-----------
char txBuffer[20] ;
char rxBuffer[10];  // B? d?m d? nh?n d? li?u
int parse_key_value(char *input, char *key);
char key[5];
char value[5];
//--------UART-----------
//--------SERVO-----------
#define port GPIOA
uint16_t servo[4] = {TIM_CHANNEL_4,TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3 };
uint16_t pinH[4] = {GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_7};
void timServoInnit();
void controlServo(uint16_t servo);

//--------SERVO-----------
void phan_loai();
int loai1=0;
int loai2=0;
int loai3=0;
int loai4=0;
int main(void)
{
  
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
	HAL_UART_Transmit(&huart1,(uint8_t *)("Start Aplication \n"),sizeof("Start Aplication \n"),HAL_MAX_DELAY);
	
	timServoInnit();
  lcd_init();
	Lcd_Ghilenh(0x01);
	HAL_Delay(50);
	Lcd_Ghilenh(0x80);
	Lcd_ghichuoi("DATN");
	Lcd_Ghilenh(0xC0);
	Lcd_ghichuoi("PHANLOAI");
	HAL_Delay(3000);
	lcd_clear();
	updateDataToLcd();
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,(int)1);
  while (1)
  {
		memset(rxBuffer,'\0', sizeof(rxBuffer));
		memset(txBuffer,'\0', sizeof(txBuffer));
		memset(key,'\0', sizeof(key));
		//memset(value,'\0', sizeof(value));
	HAL_UART_Receive(&huart1,(uint8_t *)(rxBuffer),sizeof(rxBuffer),100);
	if (parse_key_value(rxBuffer,key)){
		sprintf((char *)(txBuffer),"key:%s|\n",key);
		HAL_UART_Transmit(&huart1,(uint8_t *)(txBuffer),sizeof(txBuffer), 100);
	 }
  phan_loai();
  }
  /* USER CODE END 3 */
}
void timServoInnit(){
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	
	// Dua servo ve ci tri 0 do 
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,750);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,750);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3,750);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4,750);
}
void phan_loai(){
	if(strcmp(key, "k1") == 0){
	  while(HAL_GPIO_ReadPin(port,pinH[0]) == 1);
		controlServo(servo[0]);
		loai1++;
		updateDataToLcd();
		HAL_UART_Transmit(&huart1,(uint8_t *)("ok\n"),sizeof("ok\n"),HAL_MAX_DELAY);
	}
	else if (strcmp(key, "k2") == 0){
	while(HAL_GPIO_ReadPin(port,pinH[1]) == 1);
		controlServo(servo[1]);
		loai2++;
		updateDataToLcd();
		HAL_UART_Transmit(&huart1,(uint8_t *)("ok\n"),sizeof("ok\n"),HAL_MAX_DELAY);
	}
	else if (strcmp(key, "k3") == 0){
	while(HAL_GPIO_ReadPin(port,pinH[2]) == 1);
		controlServo(servo[2]);
		loai3++;
		updateDataToLcd();
		HAL_UART_Transmit(&huart1,(uint8_t *)("ok\n"),sizeof("ok\n"),HAL_MAX_DELAY);
	}
	else if (strcmp(key, "k4") == 0){
	while(HAL_GPIO_ReadPin(port,pinH[3]) == 1);
		controlServo(servo[3]);
		loai4++;
		updateDataToLcd();
		HAL_UART_Transmit(&huart1,(uint8_t *)("ok\n"),sizeof("ok\n"),HAL_MAX_DELAY);
	}
	else if (strcmp(key, "rst") == 0){
	loai1=0; loai2=0; loai3=0; loai4=0;
	updateDataToLcd();
	}
}
void controlServo(uint16_t servo){
	HAL_Delay(5500);
	int n=5;
	for(int i = 0;i<=400;i=i+5){
 __HAL_TIM_SET_COMPARE(&htim1,servo,750-i);
		HAL_Delay(6);
	}
	HAL_Delay(2000);
__HAL_TIM_SET_COMPARE(&htim1,servo,750);
}
int parse_key_value(char *input, char *key) {
    // Tìm ký t? '=' trong chu?i input
    char *equalSign = strchr(input, '=');
    if (equalSign == NULL) {
        return 0;  // Không tìm th?y ký t? '='
    }
    // Tách key
    strncpy(key, input, equalSign - input);
    key[equalSign - input] = '\0';  // K?t thúc chu?i key b?ng ký t? null
    // Tách value
    //strcpy(value, equalSign + 1);  // Sao chép ph?n value sau d?u '='
    return 1;  // Tách thành công
}
void updateDataToLcd(){
	//loai 1, 
	Lcd_Ghilenh(0x80);
	sprintf((char *)dataLcd,"L1:%d",loai1);
	Lcd_ghichuoi(dataLcd);
	//loai 2
	Lcd_Ghilenh(0x88);
	sprintf((char *)dataLcd,"L2:%d",loai2);
	Lcd_ghichuoi(dataLcd);
	//loai 3
	Lcd_Ghilenh(0xc0);
	sprintf((char *)dataLcd,"L3:%d",loai3);
	Lcd_ghichuoi(dataLcd);
	//loai4
	Lcd_Ghilenh(0xc8);
	sprintf((char *)dataLcd,"L4:%d",loai4);
	Lcd_ghichuoi(dataLcd);
}
void lcd_clear(){
	Lcd_Ghilenh(0x80);
	Lcd_ghichuoi("               ");
	Lcd_Ghilenh(0xC0);
	Lcd_ghichuoi("                ");
	HAL_Delay(1000);
}
void Lcd_Ghilenh(char lenh)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (lenh&0xf0);
	data_l = ((lenh <<4)&0xf0);
	data_t[0]=data_u|0x0c;
	data_t[1]=data_u|0x08;
	data_t[2]=data_l|0x0c;
	data_t[3]=data_l|0x08;
HAL_I2C_Master_Transmit(&hi2c1, Diachi_LCD,(uint8_t *) data_t, 4, 100);
}
void  Lcd_ghidulieu(char lenh)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (lenh&0xf0);
	data_l = ((lenh <<4)&0xf0);
	data_t[0]=data_u|0x0D;
	data_t[1]=data_u|0x09;
	data_t[2]=data_l|0x0D;
	data_t[3]=data_l|0x09;
	HAL_I2C_Master_Transmit(&hi2c1, Diachi_LCD,(uint8_t *) data_t, 4, 100);
}
	
void lcd_init(void)
{
	Lcd_Ghilenh(0x03);
	HAL_Delay(50);
	Lcd_Ghilenh(0x02);
	HAL_Delay(50);
	Lcd_Ghilenh(0x06);
	HAL_Delay(50);
	Lcd_Ghilenh(0x0c);
	HAL_Delay(50);
	Lcd_Ghilenh(0x28);
	HAL_Delay(50);
	Lcd_Ghilenh(0x80);
	HAL_Delay(50);
}
void Lcd_ghichuoi(char *str)
{
	while(*str) Lcd_ghidulieu(*str++);
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 31;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 9999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  if (HAL_UART_Init(&huart1) != HAL_OK)
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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
