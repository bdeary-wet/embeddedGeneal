/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "crc.h"
#include "tim.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <alloca.h>
#include "taskService.h"
#include "swbgtimers.h"
#include "gencmdef.h"
#include "generalQueue.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
#define NUM_OF_TASKS 10
extern __IO uint32_t uwTick;
__IO uint32_t *someCounter = &uwTick;

uint32_t tasksAllowed;
taskHandle_t bgTimerTask;
taskHandle_t task1Task;
taskHandle_t task2Task;
taskHandle_t task3Task;
taskHandle_t task4Task;

void HAL_SYSTICK_Callback(void)
{
    TS_SignalTask(bgTimerTask);
}

uint32_t Clock3;
uint32_t Clock6;
uint32_t Clock7;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim3) ++Clock3;
    if (htim == &htim6) ++Clock6;
    if (htim == &htim7) ++Clock7;
}

static void subTask(int cnt)
{
    volatile int i;
    for (i = 0; i < cnt; i++)
    {
        if ( 0 == i%1000 )
        {
            TS_Yield();
        }
    }
}


void task1(void)
{
    subTask(3000);
    TS_SignalTask(task4Task);
    TS_SignalTask(task2Task);
}

void task2(void)
{
    subTask(30000);
    TS_SignalTask(task3Task);    
}

void task3(void)
{
    subTask(300000);
    TS_SignalTask(task1Task);
}

void task4(void)
{
    subTask(3000000);
    TS_SignalTask(task2Task);
}


// Interface to the fast software timer code using the 4 output compare
// registers of TIM3
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim != &htim3) return;
    switch(htim->Channel)
    {
        case HAL_TIM_ACTIVE_CHANNEL_1:
        SWT_ChanIsr(0);
        break;
        case HAL_TIM_ACTIVE_CHANNEL_2:
        SWT_ChanIsr(1);        
        break;
        case HAL_TIM_ACTIVE_CHANNEL_3:
        SWT_ChanIsr(2);        
        break;
        case HAL_TIM_ACTIVE_CHANNEL_4:    
        SWT_ChanIsr(3);          
        break;
        case HAL_TIM_ACTIVE_CHANNEL_CLEARED:
        break;
    }
}

future_t* FutureCallbackIsr(uint32_t uSec, uintptr_t context, objFunc_f callback);

uint32_t futureCalled;

void futureTest(void *context)
{
    FutureCallbackIsr((uint32_t)context, (uintptr_t)context, futureTest);
    futureCalled++;
}


/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_CRC_Init();
  MX_TIM3_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();

  /* USER CODE BEGIN 2 */
  
  HAL_ResumeTick();
  
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);
  
  SWT_FastInit();
  
  // allocate space for the tasks  
  uint32_t *space = alloca(TS_InitGetSize(NUM_OF_TASKS));
  tasksAllowed = TS_Init(space,TS_InitGetSize(NUM_OF_TASKS));
  if(tasksAllowed != NUM_OF_TASKS)
    { while (1) {} }  // Oh Crap
  // make the background timer a task itself
  bgTimerTask =  TS_AddTask(SWT_Background);
  task1Task =  TS_AddTask(task1);
  task2Task =  TS_AddTask(task2);
  task3Task =  TS_AddTask(task3);
  task4Task =  TS_AddTask(task4);    

  TS_SignalTask(task3Task);
    
  // launch future test
  futureTest((void*)100);
  futureTest((void*)15000);  
  futureTest((void*)1023);
    
    
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // The task runner  
    TS_Background();
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */


  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

   if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);

  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_16);

  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);

  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(64000000);

  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

  LL_SetSystemCoreClock(64000000);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
}

/** NVIC Configuration
*/
static void MX_NVIC_Init(void)
{
  /* TIM6_DAC1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM6_DAC1_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(TIM6_DAC1_IRQn);
  /* TIM7_DAC2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM7_DAC2_IRQn, 1, 2);
  HAL_NVIC_EnableIRQ(TIM7_DAC2_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
