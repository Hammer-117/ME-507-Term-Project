/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "motorDriver.h"
#include <stdio.h>
#include "encoder.h"
#include "APDS.h"
#include "BNO055.h"
#include "TCRT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//States
//#define S1_DEADBAT  1;
//#define S2_DEADMAN  2;
//#define S3_MANUAL   3;
//#define S4_GO 		4;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
uint8_t S1_DEADBAT 	= 1;
uint8_t S2_DEADMAN 	= 2;
uint8_t S3_MANUAL 	=  3;
uint8_t S4_GO 		=	4;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI1_Init(void);
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
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */


  // General GPIO Manipulation

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); //Enable Motor Relay

  // Initialize Motors
  motorType_t my_motor1 = {&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, 800}; // Configure and Enable Drive Motor 1
  enableMotor(my_motor1);
  motorType_t my_motor2 = {&htim1, TIM_CHANNEL_3, TIM_CHANNEL_4, 800}; // Configure and Enable Drive Motor 2
  enableMotor(my_motor2);
  motorType_t my_motor3 = {&htim3, TIM_CHANNEL_1, TIM_CHANNEL_2, 800}; // Configure and Enable Collector Motor
  enableMotor(my_motor3);

  // Initialize Duty Cycles for Debugger
  int8_t DTY1 = 0x00;
  int8_t DTY2 = 0x00;
  int8_t DTY3 = 0x00;

  // Initialize Motor 1's Encoder
  encoder_t myEnc1 = encoder_Init(&htim4, 0xFFFF, 360, 34);
  //encoder_t myEnc5 = encoder_Init(&htim5, 0xFFFFFFFF, 360, 34);

  // Initialize APDS Sensor and its variables
  APDS_t myAPDS = APDS9960_Init(&hi2c2, 0xDB, x4);
  uint32_t color = 0;
  uint8_t prox = 0;
  uint8_t desired_color = RED;

  // Limit Switch Variable
  uint8_t isFull = 0;

  // Initialize IMU
  BNO055_t myBNO = BNO_Init(&hi2c2);
  myBNO = BNO_OprMode(myBNO, NDOF);

  // Initialize Line Followers
  TCRT_t myTCRTR = TCRT_Init(&hadc1, 2, GPIOB, GPIO_PIN_12);
  TCRT_t myTCRTL = TCRT_Init(&hadc1, 3, GPIOB, GPIO_PIN_13);

  // Initialize Battery as TCRT for ADC Reading Function
  uint32_t BatLvl = 0;

  //Set State Variables
  int8_t state = S2_DEADMAN;

  // Arduino I2C addr and data holder (0x69<<1)
  uint8_t Ard_addr = 0xD2;
  //uint8_t Ard_addr = 0x69;
  uint8_t Deadman = 0;
  uint8_t ManCont = 0;
  uint8_t BallTake = 0;

  // Variables for P Control
  int32_t Kpa = 0.25;
  int32_t Kpd = 0.25;
  int32_t angle;
  int32_t distance;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //Before entering states, do basic checking to see if and how we should operate
	  uint8_t Ard_msg[4] = {0};
	  HAL_I2C_Master_Receive(&hi2c2, Ard_addr, Ard_msg, 4, HAL_MAX_DELAY);
	  Deadman = Ard_msg[0];
	  ManCont = Ard_msg[1];
	  int8_t ContY = Ard_msg[2]-128;
	  int8_t ContX = Ard_msg[3]-128;

	  // Check Battery Level
	  //If BatLvl is under 3025 (9.9V), turn off motors (3113 10.2, 3177 10.4, 3208, 10.5)
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	  BatLvl = HAL_ADC_GetValue(&hadc1);

	  //Check if Ball Carriage is full
	  isFull = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);

	  /*
	  if (BatLvl < 3113) {
		  state = S1_DEADBAT;
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); //BAT LED On
		  setDuty(my_motor1, 0x00);
		  setDuty(my_motor2, 0x00);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); //Disable Motor Relay
	  	  }
	  else if (ManCont)  {
		  state = S3_MANUAL;
	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); //Enable Motor Relay
	  	  }
	  else if (Deadman) {
		  state = S4_GO;
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); //Enable Motor Relay
		  }
	  else {
		  state = S2_DEADMAN;
		  setDuty(my_motor1, 0x00);
		  setDuty(my_motor2, 0x00);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); //Disable Motor Relay
		  }
		  */


	  if (state == S1_DEADBAT) {
		  if (BatLvl > 3113) {
			  state = S2_DEADMAN;
			  }
	  }
	  else if (state == S2_DEADMAN) {
		  if (BatLvl < 3113) {
			  state = S1_DEADBAT;
			  setDuty(my_motor1, 0x00);
			  setDuty(my_motor2, 0x00);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); //BAT LED On
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); //Disable Motor Relay
			  }
		  else if (ManCont)  {
			  state = S3_MANUAL;
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); //Enable Motor Relay

			  }
		  else if (Deadman) {
			  state = S4_GO;
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); //Enable Motor Relay

			  }
	  }
	  else if (state == S3_MANUAL) {
		  DTY1 = ContX/2 + ContY/2;
		  DTY2 = -ContX/2 + ContY/2;
		  setDuty(my_motor1, DTY1);
		  setDuty(my_motor2, DTY2);
		  if (BatLvl < 3113) {
			  state = S1_DEADBAT;
			  setDuty(my_motor1, 0x00);
			  setDuty(my_motor2, 0x00);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); //BAT LED On
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); //Disable Motor Relay
			  }
		  else if (!ManCont) {
			  state = S2_DEADMAN;
			  setDuty(my_motor1, 0x00);
			  setDuty(my_motor2, 0x00);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); //Disable Motor Relay
		  	  }
	  }
	  else if (state == S4_GO) {
		  setDuty(my_motor1, 0x40);
		  setDuty(my_motor2, -0x40);
		  if (!Deadman) {
			  state = S2_DEADMAN;
			  setDuty(my_motor1, 0x00);
			  setDuty(my_motor2, 0x00);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); //Disable Motor Relay
		  }
	  }
	  //else if (Deadman && isFull) { state = S4_AUTOHOME;}
	  //else if (Deadman) { state = S5_AUTOBALL;}

	  /*
	  myBNO = BNO_GetEul(myBNO);

	  myTCRTR = TCRT_ReadDigital(myTCRTR);
	  myTCRTL = TCRT_ReadDigital(myTCRTL);

	  setDuty(my_motor1, DTY1);
	  setDuty(my_motor2, DTY2);
	  setDuty(my_motor3, DTY3);


	  myEnc1 = encoder_Update(myEnc1);
	  //myEnc5 = encoder_Update(myEnc5);
	  */

/*
	  if (state <= 4 && state != 0) {
		  switch(state) {

		  	 //Dead Battery
			 case 1:
				 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); //Disable Motor Relay
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); //Low Bat LED On

			 //Dead Man
			 case 2:
				 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); //Disable Motor Relay
				 setDuty(my_motor1, 0x00);
				 setDuty(my_motor2, 0x00);

			 // Manual Control
			 case 3:
				 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); //Enable Motor Relay
				 DTY1 = ContX/2 + ContY/2;
				 DTY2 = -ContX/2 + ContY/2;
				 setDuty(my_motor1, DTY1);
				 setDuty(my_motor2, DTY2);
				 /*
				 BallTake = Ard_msg[6];
				 if (BallTake) { setDuty(my_motor3, 0x60);}
				 else {setDuty(my_motor3, -0x60);}


			 // Autocontrol go home
			 case 4:
				 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); //Enable Motor Relay
				 setDuty(my_motor1, 0x40);
				 setDuty(my_motor2, -0x40);


				 /*angle = (Ard_msg[7]<<8) | Ard_msg[8];
				distance = (Ard_msg[9]<<8) | Ard_msg[10];
				if (angle > 10) {
					DTY1 = Kpa*angle;
					DTY2 = Kpa*angle;
					setDuty(my_motor1, DTY1);
					setDuty(my_motor2, DTY2);
				}
				else {
					DTY1 = Kpd*distance;
					DTY2 = -Kpd*distance;
				}
				if (BallTake) {setDuty(my_motor3, -0x60);}
				else {setDuty(my_motor3, 0x00);}

			 // Autocontrol go ball
			 case 5:
				 angle = (Ard_msg[0]<<8) | Ard_msg[1];
				 distance = (Ard_msg[2]<<8) | Ard_msg[3];
				 if (angle > 10) {
			 		 DTY1 = Kpa*angle;
					 DTY2 = Kpa*angle;
					 setDuty(my_motor1, DTY1);
					 setDuty(my_motor2, DTY2);
				 }
				 else {
					 DTY1 = Kpd*distance;
					 DTY2 = -Kpd*distance;
					 setDuty(my_motor1, DTY1);
					 setDuty(my_motor2, DTY2);
				 }

				 prox = APDS9960_Prox(myAPDS);
			     color = APDS9960_Color(myAPDS);
				 if ((prox > 128) && (color == desired_color)) {
					 setDuty(my_motor3, 0x60);
				 }
				 else if ((prox > 128) && (color != desired_color)) {
					 setDuty(my_motor3, -0x60);
				 }
				 else {
					 setDuty(my_motor3, 0x60);
				 }


			 default :
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); //Disable Motor Relay
		  }
		  HAL_Delay(50);

	  }*/

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 799;
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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 799;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB12 PB13 PB14
                           PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
