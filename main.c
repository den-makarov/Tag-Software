#include "stm8s.h"
#include "driverLCD.h"
#include "nordicDriver.h"


#define TIME_BASE_TIMER						TIM4

#define ENABLE_IRQ								enableInterrupts();
#define DISABLE_IRQ								disableInterrupts();

#define LCD_CLK_PORT							GPIOA
#define LCD_CLK_PIN								GPIO_PIN_3

#define NORDIC_DATA_WIDTH					(uint8_t)0x03
#define EMPTY_RECEIVER						(uint8_t)0x01
#define LCD_ENABLED								(uint8_t)0x10
#define LCD_CONTROL_MASK					(uint8_t)0xF0
#define RxFIFO_DATA_READY					(uint8_t)0x40
/* Private functions ---------------------------------------------------------*/
void SystemClockInit(void);
void decimalCorrection(uint16_t * r);
void decimalCorrection(uint16_t * r);
void updatePrice(uint8_t * src);
//void testNordicDriver(void);
void testNordicPRX(uint8_t * data, uint8_t count);


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

	uint16_t price = 0x0000;
	uint8_t lcdStatus = 0x00;
	uint8_t nordicData[NORDIC_DATA_WIDTH];
	nordicData[0] = 0x00;


	SystemClockInit();
		
	/* Initialize I/Os in Output Mode */
	GPIO_Init(LCD_CLK_PORT, (GPIO_Pin_TypeDef)LCD_CLK_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
	

	SystemClockInit();
	
	/* Initialize LCD driver */

	driverNordicInit();
	

	driverLCD_INIT();
	driverLCD_enableCMD();

	/* Initialize RF driver */
	driverNordicInit();
	
	/* Initialize LCD driver */
	driverLCD_INIT();

	
	
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
		LCD_CLK_PORT->ODR ^= LCD_CLK_PIN;
		
		testNordicPRX(nordicData, 0x03);
		
		if ((nordicData[0] & EMPTY_RECEIVER) != EMPTY_RECEIVER)
		{
			lcdStatus = nordicData[0] & LCD_CONTROL_MASK;
			if ((lcdStatus & LCD_ENABLED) == LCD_ENABLED)
			{
				driverLCD_enableCMD();
				updatePrice(nordicData + 0x01);
			}
			else
				driverLCD_disableCMD();
		}
  }

}



void testNordicPRX(uint8_t * data, uint8_t count)
{
	uint8_t i = 0x00;	
	uint8_t tempByte = 0x00;
	uint16_t timeOut;
	
	// Check Nordic RxFIFO status
	driverNordic_ReadRegisters(FIFO_STATUS, &tempByte, 0x01);
	
	if ((tempByte & RxFIFO_EMPTY) != RxFIFO_EMPTY)
		driverNordic_FlushRxFIFO();
	
	
	// Disable AutoAcknoledge for all Pipes	
	tempByte = 0x00;
	driverNordic_WriteRegisters(EN_AA_ESHBURST, &tempByte, 0x01);
	
	// Set the 105th radio-channel that is 2505 MHz
	// I hope it should be free and legal
	tempByte = 0x69;
	driverNordic_WriteRegisters(RF_CH, &tempByte, 0x01);
	
	// Set last byte of PRX address on Pipe 0
	tempByte = 0x75;
	driverNordic_WriteRegisters(RX_ADDR_P0, &tempByte, 0x01);
	
	// Set Width of Payload on Pipe 0 in case when
	// AutoPayloadWidth feature isn't enabled
	tempByte = count;
	driverNordic_WriteRegisters(RX_PW_P0, &tempByte, 0x01);
		
	// Configure nordic to listen to air
	// and mask IRQ pin on expected event
	tempByte = 0x4B;
	driverNordic_WriteRegisters(CONFIG, &tempByte, 0x01);
	
	// Delay for 1.5 ms
	// Transition from Power Down Mode to StandBy I Mode
	// Depends on external crystal inductance. For every 30 mH add 1.5 ms
	// Fifty nop cycles at 31250 Hz CPU clocking frequency
	for (i = 0; i < 50; i++);
	
	// Enable Receiver
	driver_NORDIC_CE_HIGH;
	
	// Wait for received packet or timeout at least 3 sec
	LCD_CLK_PORT->ODR |= LCD_CLK_PIN;
	for (timeOut = 0x0000; timeOut < 10000; timeOut++)
	{
		tempByte = driverNordic_GetStatus();
		
		if ((tempByte & RxFIFO_DATA_READY) == RxFIFO_DATA_READY)
		{
			driver_NORDIC_CE_LOW;
			
			driverNordic_WriteRegisters(STATUS, &tempByte, 0x01);
			driverNordic_ReadRxPayload(data, count);
			LCD_CLK_PORT->ODR &= ~LCD_CLK_PIN;
			return;
		}
	}
	LCD_CLK_PORT->ODR &= ~LCD_CLK_PIN;
	driver_NORDIC_CE_LOW;
	*data |= EMPTY_RECEIVER;
	
	return;
}


/*
void testNordicDriver()
{
	uint8_t data[5];
	uint8_t i = 0x00;
	//uint8_t j = 0x00;
	
	nordicData = (uint16_t)driverNordic_GetStatus();
	decimalCorrection(&nordicData);
	driverLCD_drawDigits(nordicData, FALSE);
	nordicData = 0x0000;
		
	Delay(2000);
		
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
		
		
		nordicData = (uint16_t)driverNordic_GetStatus();
		decimalCorrection(&nordicData);
		driverLCD_drawDigits(nordicData, FALSE);
		nordicData = 0x0000;
		
		nordicStatus = driverNordic_GetStatus();
		driverNordic_WriteRegisters(STATUS, &nordicStatus, 0x01);
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
	//CLK_PeripheralClockDisableAll();
	CLK->PCKENR1 &= 0x00;
	CLK->PCKENR2 &= 0x73;
	
	
	// Set HSI divider
	//CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV4);
	CLK->CKDIVR &= 0xE0;
	CLK->CKDIVR |= 0x17;
	
	
	// Set CPU divider
	//CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV128);
}




void updatePrice(uint8_t * src)
{
	uint16_t price = ((uint16_t)(*src << 0x08)) + ((uint16_t)*(src + 0x01));

	if (price >= 0x1999)
		price = 0x0000;

	decimalCorrection(&price);
	

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
