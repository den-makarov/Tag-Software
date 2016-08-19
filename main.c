#include "stm8s.h"
#include "driverLCD.h"
#include "nordicDriver.h"


#define TIME_BASE_TIMER						TIM4

#define ENABLE_IRQ								enableInterrupts();
#define DISABLE_IRQ								disableInterrupts();

#define LCD_CLK_PORT							GPIOA
#define LCD_CLK_PIN								GPIO_PIN_3

/* Private functions ---------------------------------------------------------*/
void SystemClockInit(void);
//void decimalCorrection(uint16_t * r);
//void updatePrice(void);
//void testNordicDriver(void);

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
	uint16_t nordicData = 0x00;
	uint8_t nordicStatus = 0x00;
	
	SystemClockInit();
		
	/* Initialize I/Os in Output Mode */
	GPIO_Init(LCD_CLK_PORT, (GPIO_Pin_TypeDef)LCD_CLK_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
	
	/* Initialize RF driver */
	//driverNordicInit();
	
	/* Initialize LCD driver */
	//driverLCD_INIT();
	//driverLCD_enableCMD();
	
	
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
		Delay(2000);
		LCD_CLK_PORT->ODR ^= LCD_CLK_PIN;
		
		/*nordicData = (uint16_t)driverNordic_GetStatus();
		decimalCorrection(&nordicData);
		driverLCD_drawDigits(nordicData, FALSE);
		nordicData = 0x0000;
		
		Delay(2000);
		LCD_CLK_PORT->ODR ^= LCD_CLK_PIN;
		
		testNordicDriver();
		nordicData = (uint16_t)driverNordic_GetStatus();
		decimalCorrection(&nordicData);
		driverLCD_drawDigits(nordicData, FALSE);
		nordicData = 0x0000;
		
		nordicStatus = (uint16_t)driverNordic_GetStatus();
		driverNordic_WriteRegisters(STATUS, &nordicStatus, 0x01);
		//updatePrice();*/
  }

}

/*
void testNordicDriver()
{
	uint8_t data[5];
	uint8_t i = 0x00;
	//uint8_t j = 0x00;
	
	data[0] = (uint8_t)0x00;
	data[1] = (uint8_t)0x3A;
	data[2] = (uint8_t)0x01;
	data[3] = (uint8_t)105;
	data[4] = (uint8_t)0x75;
	
	driverNordic_WriteRegisters(SETUP_RETR, &data[0], 0x01);
	driverNordic_WriteRegisters(EN_AA_ESHBURST, &data[0], 0x01);
	driverNordic_WriteRegisters(TX_ADDR, &data[4], 0x01);
	driverNordic_WriteRegisters(RF_CH, &data[3], 0x01);
	driverNordic_WriteRegisters(CONFIG, &data[1], 0x01);
	
	// Delay for 1.5 ms
	// Transition from Power Down Mode to StandBy I Mode
	// Depends on external crystal inductance. For every 30 mH add 1.5 ms
	// Fifty nop cycles at 31250 Hz CPU clocking frequency
	for (i = 0; i < 50; i++);
	
	//for (j = 0; j < 4; j++)
	//{
		driverNordic_WriteTx(data, 0x05);
		
		driver_NORDIC_CE_HIGH;
		
		// Delay for 130 us - Tx settling time
		// Four nop cycles at 31250 Hz CPU clocking frequency
		//for (i = 0; i < 4; i++);
	
		
	
		driver_NORDIC_CE_LOW;
	
		
		//data[0] = driverNordic_GetStatus();
		//driverNordic_WriteRegisters(0x07, &data[0], 0x01);
}*/

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


/*
void updatePrice()
{
	static uint16_t price = 0x0000;

	if (price++ >= 0x1999)
		price = 0x0000;

	decimalCorrection(&price);
	
	if (price & 0x0010)
		driverLCD_drawDigits(price, TRUE);
	else
		driverLCD_drawDigits(price, FALSE);
}



void decimalCorrection(uint16_t * r)
{
	uint8_t tempByte = 0x00;
	uint8_t i;

	for (i = 0x00; i < 0x04; i++)
	{
		tempByte = (uint8_t)((*r >> (i * 4)) & 0x0F);
		if (tempByte > 0x09)
			*r += ((uint16_t)0x0006 << (i * 4));
	}
}
*/


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
