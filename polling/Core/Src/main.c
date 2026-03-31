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
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "fonts.h"
#include "step_detector.h"
#include "adxl345.h"


#include <stdio.h>
#include <string.h>
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int16_t accelX, accelY, accelZ;//zmienne do odczytów

StepDetector_t myTracker;//tworzymy obiekt step detectora od tego structa tak jakby

volatile uint8_t isTracking = 1;//flaga do prerwania z przycisku USER
/* USER CODE END PTD */

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
char buffer[200];
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
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(100);//czekamy zeby wszystko sie zainicjalizowalo

   if(ssd1306_Init(&hi2c2) != 0)//wywolanei na wypadek bledu komunikacja OLEDa
   {
       Error_Handler();
   }
   ADXL345_Init(&hspi1);
   StepDetector_Init(&myTracker);//inicjalizacja detectora ze wskaznikiem na obiekt ze structu gdzzie zapisywane sa zmienne

   // Ekran powitalny
   ssd1306_Fill(Black);
   ssd1306_SetCursor(20, 20);
   ssd1306_WriteString("WELCOME", Font_11x18, White);
   ssd1306_UpdateScreen(&hi2c2);
   HAL_Delay(2000);
   //sprawdzenie dla pewnosci stanu adxl
   UART_Print("\r\n=== Stars tracking ===\r\n");
   uint8_t devId = ADXL345_ReadRegister(ADXL345_REG_DEVID);
   sprintf(buffer, "Device ID: 0x%02X (oczekiwany 0xE5)\r\n", devId);
   UART_Print(buffer);

   if(devId != 0xE5)//zabezpieczienei na wypadek błędu, wpada wtedy w petle bez konca
   {
       UART_Print("ERROR: Wrong device id");
       while(1);
   }

   //UART_Print("ADXL345 initialized (polling mode)\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   char bufferOled[32];
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(isTracking)
	  {
	  ADXL345_ReadAccel(&accelX, &accelY, &accelZ);//pobieramy dane
	  	  float xg = accelX * 0.0039f;//przeliczamy
	  	  float yg = accelY * 0.0039f;
	  	  float zg = accelZ * 0.0039f;

	  	  StepDetector_Process(&myTracker, xg, yg, zg);//wywołujemy funkcje detekcji
	  	  static uint32_t lastDisplay = 0;//ustawiamy na poczatku na na 0
	  	  	  if(HAL_GetTick() - lastDisplay > 200)// sprawdzamy aktualny stan zegara z lastdisplayem jezeli jest tam co te 200ms to wysweitlamy
	  	  	  {

	  	  		  lastDisplay = HAL_GetTick();//zapisujemy ten czas do zmiennej zeby znoiwu miec do porownania w if
			  sprintf(buffer,
					  "RAW X=%6d Y=%6d Z=%6d | g X=%7.3f Y=%7.3f Z=%7.3f | UP=%2d DOWN=%2d\r\n",
					  accelX, accelY, accelZ,
					  xg, yg, zg,
					  myTracker.stepsUp, myTracker.stepsDown);

			 UART_Print(buffer);
			 UART_Print("\r\n");


	  				  ssd1306_Fill(Black);

	  				  ssd1306_SetCursor(0, 0);
	  				  ssd1306_WriteString("Tracking", Font_11x18, White);


	  				  ssd1306_SetCursor(0, 25);
	  				  sprintf(bufferOled, "Up: %2d", myTracker.stepsUp);
	  				  ssd1306_WriteString(bufferOled, Font_7x10, White);

	  				  ssd1306_SetCursor(0, 35);
	  				  sprintf(bufferOled, "Down: %2d", myTracker.stepsDown);
	  				  ssd1306_WriteString(bufferOled, Font_7x10, White);

	  				  ssd1306_SetCursor(0, 45);
	  				  sprintf(bufferOled, "Total: %2d", myTracker.stepsTotal);
	  				  ssd1306_WriteString(bufferOled, Font_7x10, White);


	  				  ssd1306_UpdateScreen(&hi2c2);


	  			  }
	  }
	  else
	  {
		  ssd1306_Fill(Black);

		  ssd1306_SetCursor(0, 0);
		  ssd1306_WriteString("Stop", Font_11x18, White);

		  ssd1306_UpdateScreen(&hi2c2);
	  }

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	if(GPIO_Pin == USER_BUTTON_PIN_Pin)
	{
	        static uint32_t last_press_tick = 0;
	        uint32_t current_tick = HAL_GetTick();

	        // ignoruje kliknięcia częstsze niż 200ms na wypadek debouncu
	        if(current_tick - last_press_tick > 200)
	        {
	            isTracking = !isTracking;
	            last_press_tick = current_tick;
	        }
	}
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
