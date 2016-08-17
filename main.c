#include "stm8s.h"
#include "driverLCD.h"

#define TIME_BASE_TIMER						TIM4

#define ENABLE_IRQ								enableInterrupts();
#define DISABLE_IRQ								disableInterrupts();

#define LCD_CLK_PORT							GPIOA
#define LCD_CLK_PIN								GPIO_PIN_3

/* Private functions ---------------------------------------------------------*/
#define DELAY							(1)
#ifdef DELAY
void Delay (uint16_t nCount);
#endif
/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	/* Initialize I/Os in Output Mode */
	GPIO_Init(LCD_CLK_PORT, (GPIO_Pin_TypeDef)LCD_CLK_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
	
	/* Initialize LCD driver */
	driverLCD_setup()
	
	
	
	/* Initialization Time Base unit for synchronized delays */
	// Base frequency is fHSI/(TIMx->PSCR * TIMx->ARR)
	// Timer clock prescaler equals to value powered by TIMx->PSCR
	//TIME_BASE_TIMER->PSCR = (uint8_t)0x07; // 2^7 = 128
	//TIME_BASE_TIMER->ARR = (uint8_t)125;	// Timer Overflow event frequency is 125 Hz
	//TIME_BASE_TIMER->IER = 0x01;
	//TIME_BASE_TIMER->CR1 = 0x01;

	//ENABLE_IRQ;

  while (1)
  {
		Delay(1000);
		LCD_CLK_PORT->ODR ^= LCD_CLK_PIN;
  }

}


/************************************************************************
*		Initialize Clock Control Module
*		---- default values after power on reset:
*				-- HSI selected as master clock source
*				-- fHSI = (fHSI RC output)/8 = 2 MHz
*				-- SWIM CLK frequency = (fHSI RC output)/2 = 8 MHz
*	
*		---- set new clocking configuration:
*				-- HSI divider = /4
*				-- fHSI/4 = 4 MHz - Master frequency (Peripheral CLK frequency)
*				-- CPU divider = Master frequency / 128 = 31.250 kHz
*************************************************************************/
void SystemClockInit(void)
{
	// Disable all peripheries clocking
	CLK_PeripheralClockDisableAll();
	
	// Set HSI divider
  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV4);
	
	// Set CPU divider
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV128);
}

#ifdef DELAY //Software Delay function
/**
  * @brief Delay
  * @param nCount
  * @retval None
  */
void Delay(uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

#endif

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
