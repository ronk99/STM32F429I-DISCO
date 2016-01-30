/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include <string.h>
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_io.h"
#include "stm32f429i_discovery_ts.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f4xx_hal_gpio.h"

// FreeRTOS can be configured via ./Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOSConfig.h

/* Tracing macros have to be defined at the end of FreeRTOSConfig.h
 *
 * #define traceTASK_SWITCHED_IN() \
    { unsigned priority=pxCurrentTCB->uxPriority; *(volatile unsigned *)0x4002201a=0xf0; *(volatile unsigned *)0x40022018 = priority << 4; }

 * #define ENTER_ISR \
 * unsigned old_prio =*(volatile unsigned*)0x40022014 >> 4;   *(volatile unsigned*)0x40022018 = 0xf0;
 * #define LEAVE_ISR  *(volatile unsigned*)0x4002201a = 0xf0; *(volatile unsigned*)0x40022018 = old_prio << 4;
 * #define ENTER_RTOS *(volatile unsigned*)0x40022018 = 0xe0; *(volatile unsigned*)0x4002201a = 0x10;
 * #define ENTER_IDLE *(volatile unsigned*)0x4002201a = 0xf0;
 */

#include "FreeRTOS.h"
#include "task.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <math.h>

static void SystemClock_Config(void);

volatile int stuff;

void blink_task_low_priority( void *)
{
	BSP_LED_Init (LED3);
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while (true)
	{
		BSP_LED_Toggle(LED3); // Add breakpoint here
		vTaskDelayUntil (&xLastWakeTime, (TickType_t) 2000/3);
		BSP_LED_Toggle(LED3); // Add breakpoint here
		vTaskDelayUntil (&xLastWakeTime, (TickType_t) 1000/3);
	}
}

void blink_task_high_priority( void *)
{
	BSP_LED_Init (LED4);
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while (true)
	{
		BSP_LED_Toggle(LED4); // Add breakpoint here
		vTaskDelayUntil (&xLastWakeTime, (TickType_t) 1000/3);
		BSP_LED_Toggle(LED4); // Add breakpoint here
		vTaskDelayUntil (&xLastWakeTime, (TickType_t) 2000/3);
	}
}

void print(int line, float value, bool integer)
{
	int outputvalue;
	std::stringstream output;
	std::string outputstring;
	const char * chararray;
	output.str(std::string());
	if(integer)
	{
		outputvalue = value;
		output << std::fixed /*<< std::setprecision(4)*/ << std::right << std::setw(10) << outputvalue << " V";
	} else
	{
		output << std::fixed /*<< std::setprecision(4)*/ << std::right << std::setw(10) << value << " V";
	}
	outputstring = "";
	outputstring = output.str();
	chararray = "";
	chararray = outputstring.c_str();
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAtLine(line,(uint8_t *) chararray);
}

int main(void)
{
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();


	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(1, (uint32_t) LCD_FRAME_BUFFER);
	BSP_LCD_SetLayerVisible(1, ENABLE);

	BSP_LCD_SelectLayer(1);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayOn();

	//BSP_LCD_DisplayStringAtLine(0, (uint8_t *) "Hello FreeRTOS");

	#define LED_TASK_PRIORITY_LOW ((1 + tskIDLE_PRIORITY) | portPRIVILEGE_BIT)
	#define LED_TASK_PRIORITY_HIGH ((2 + tskIDLE_PRIORITY) | portPRIVILEGE_BIT)

	/*
	 * BSP_LED_Init (LED3);
	 *
	 */
	xTaskCreate( (pdTASK_CODE)blink_task_low_priority, 	"led3", 256, 0, LED_TASK_PRIORITY_LOW, NULL);
	xTaskCreate( (pdTASK_CODE)blink_task_high_priority, 	"led4", 256, 0, LED_TASK_PRIORITY_HIGH, NULL);

	vTaskStartScheduler ();
	return 0;
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 180000000
 *            HCLK(Hz)                       = 180000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 360
 *            PLL_P                          = 2
 *            PLL_Q                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 5
 *         The LTDC Clock is configured as follow :
 *            PLLSAIN                        = 192
 *            PLLSAIR                        = 4
 *            PLLSAIDivR                     = 8
 * @param  None
 * @retval None
 *
 * COPYRIGHT(c) 2014 STMicroelectronics
 */
static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	/* Enable Power Control clock */
	__PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is
	 clocked below the maximum system frequency, to update the voltage scaling value
	 regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/*##-1- System Clock Configuration #########################################*/
	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 360;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	HAL_PWREx_ActivateOverDrive();

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

	/*##-2- LTDC Clock Configuration ###########################################*/
	/* LCD clock configuration */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/4 = 48 Mhz */
	/* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDIVR_8 = 48/8 = 6 Mhz */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}
