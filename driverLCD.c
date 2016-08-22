#include "driverLCD.h"





/*----------------------------------------------------------------------------------------
		Initializes the display driver.
----------------------------------------------------------------------------------------*/
void driverLCD_INIT(void)
{
	//driverLCD_ClockDomainConfig();
	CLK->PCKENR1 |= 0x01;
	
	//driverLCD_GPIO_I2C_Config();
	LCD_I2C_PORT->DDR |= (LCD_SCL_PIN | LCD_SDA_PIN);
	
	
	driverLCD_I2C_Config();
}




/*----------------------------------------------------------------------------------------
		Initializes the I2C display driver clocking.
----------------------------------------------------------------------------------------*/
/*void driverLCD_ClockDomainConfig(void)
{
	// Enable LCD_I2C_PORT clock
	CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef)CLK_PERIPHERAL_I2C, ENABLE);
}*/




/*----------------------------------------------------------------------------------------
		Initializes the GPIO pins used by the I2C display driver.
----------------------------------------------------------------------------------------*/
/*void driverLCD_GPIO_I2C_Config(void)
{
	// Initialize I/Os in Output Open-Drain HIZ Slow Mode
	//GPIO_Init(LCD_I2C_PORT, (GPIO_Pin_TypeDef)LCD_SCL_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW);
	//GPIO_Init(LCD_I2C_PORT, (GPIO_Pin_TypeDef)LCD_SDA_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW);
	
	// PINS I2C_SCL and I2C_SDA implemented without P_BUFFER and only in SLOW_SLOPE
	// So, there is only one configuration option - Direction
	// Set output direction
	LCD_I2C_PORT->DDR |= (LCD_SCL_PIN | LCD_SDA_PIN);
}*/


/*----------------------------------------------------------------------------------------
		Configure the I2C Peripheral used to communicate with LCD driver
----------------------------------------------------------------------------------------*/
void driverLCD_I2C_Config(void)
{
	I2C_CONFIG_TypeDef I2C_Configuration;
	
	/* IOE_I2C configuration */
	I2C_Configuration.OutputClockFrequencyHz = 222222;		// I2C speed
	I2C_Configuration.OwnAddress = 0x00;					// Used only Master transmit mode
	I2C_Configuration.I2C_DutyCycle = I2C_DUTYCYCLE_2;		// Understandable
	//I2C_Configuration.Ack = I2C_ACK_NONE;					// Used only Master transmit mode
	I2C_Configuration.AddMode = I2C_ADDMODE_7BIT;			// 7 bit Address of the device
	I2C_Configuration.InputClockFrequencyMHz = 4;			//	System frequency configured in main()
  
	I2C->CR1 &= (uint8_t)(~I2C_CR1_PE);
	I2C->CR2 |= I2C_CR2_SWRST;
	I2C->CR2 &= (uint8_t)(~I2C_CR2_SWRST);
	
	I2C_Init_Simplified(LCD_I2C, &I2C_Configuration);
}




/*----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------*/
void driverLCD_enableCMD(void)
{
	//I2C_Cmd(DISABLE);
  
  
	/* Reset all I2C1 registers */
	//I2C_SoftwareResetCmd(ENABLE);
	//I2C_SoftwareResetCmd(DISABLE);
  
	/* Configure the I2C1 peripheral */
	driverLCD_I2C_Config();

	//I2C_GenerateSTART(ENABLE);
	I2C->CR2 |= I2C_CR2_START;
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

	//I2C_Send7bitAddress(driverLCD_ADDRESS, I2C_DIRECTION_TX);
	I2C->DR = driverLCD_ADDRESS;
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	//I2C_SendData(driverLCD_MODE_CMD| driverLCD_DEV_ENABLE);
	I2C->DR = (driverLCD_MODE_CMD | driverLCD_DEV_ENABLE);
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	//I2C_GenerateSTOP(ENABLE);
	I2C->CR2 |= I2C_CR2_STOP;
}




/*----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------*/
void driverLCD_disableCMD(void)
{
	//I2C_Cmd(DISABLE);
    
	/* Reset all I2C1 registers */
	//I2C_SoftwareResetCmd(ENABLE);
	//I2C_SoftwareResetCmd(DISABLE);
	
	/* Configure the I2C1 peripheral */
	driverLCD_I2C_Config();
	
	//I2C_GenerateSTART(ENABLE);
	I2C->CR2 |= I2C_CR2_START;
	
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

	//I2C_Send7bitAddress(driverLCD_ADDRESS, I2C_DIRECTION_TX);
	I2C->DR = driverLCD_ADDRESS;
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C->DR = (driverLCD_MODE_CMD | driverLCD_DEV_DISABLE);
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	//I2C_GenerateSTOP(ENABLE);
	I2C->CR2 |= I2C_CR2_STOP;
}




/*----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------*/
void driverLCD_WriteRAM(uint8_t * bufferPtr, uint8_t originRAM, uint8_t bytes)
{
	//I2C_Cmd(DISABLE);	
	
	/* Reset all I2C1 registers */
	//I2C_SoftwareResetCmd(ENABLE);
	//I2C_SoftwareResetCmd(DISABLE);

  
	/* Configure the I2C1 peripheral */
	driverLCD_I2C_Config();

	//I2C_GenerateSTART(ENABLE);
	I2C->CR2 |= I2C_CR2_START;
	
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	//I2C_Send7bitAddress(driverLCD_ADDRESS, I2C_DIRECTION_TX);
	I2C->DR = driverLCD_ADDRESS;
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C->DR = (driverLCD_NEXT_CMD | driverLCD_SUBADDRESS_CMD | driverLCD_SUBADDR_0);
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C->DR = originRAM;
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	while(bytes-- > 0x00)
	{
		I2C->DR = *bufferPtr++;
		while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	//I2C_GenerateSTOP(ENABLE);
	I2C->CR2 |= I2C_CR2_STOP;
}




/*----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------*/
void driverLCD_drawDigits(uint16_t value, bool fraction)
{
	uint8_t i, bufferLCD[4];
	
	for (i = 0x00; i < 0x04; i++)
		bufferLCD[i] = 0x00;
	
	if (((value & 0xF000) > 0x1000) || ((value & 0x0F00) > 0x0900) || ((value & 0x00F0) > 0x0090) || ((value & 0x000F) > 0x0009))
	{
		bufferLCD[0] = 0x80;
		bufferLCD[1] = 0x40;
		bufferLCD[2] = 0x42;
		driverLCD_WriteRAM(bufferLCD, 0x03, 0x04);
		return;
	}
	
	if (fraction)
		bufferLCD[1] |= 0x10;
	
	if ((value & 0xF000) == 0x1000)
		bufferLCD[3] |= 0x20;
	
	switch(value & 0x0F00)
	{
		case 0x0000: bufferLCD[2] |= 0x1D; bufferLCD[3] |= 0x50; break;
		case 0x0100: bufferLCD[2] |= 0x14; break;
		case 0x0200: bufferLCD[2] |= 0x0F; bufferLCD[3] |= 0x10; break;
		case 0x0300: bufferLCD[2] |= 0x1F; break;
		case 0x0400: bufferLCD[2] |= 0x16; bufferLCD[3] |= 0x40; break;
		case 0x0500: bufferLCD[2] |= 0x1B; bufferLCD[3] |= 0x40; break;
		case 0x0600: bufferLCD[2] |= 0x1B; bufferLCD[3] |= 0x50; break;
		case 0x0700: bufferLCD[2] |= 0x1C; break;
		case 0x0800: bufferLCD[2] |= 0x1F; bufferLCD[3] |= 0x50; break;
		case 0x0900: bufferLCD[2] |= 0x1F; bufferLCD[3] |= 0x40; break;
		default: 
			bufferLCD[0] = 0x80;
			bufferLCD[1] = 0x40;
			bufferLCD[2] = 0x42;
			driverLCD_WriteRAM(bufferLCD, 0x03, 0x04);
			return;
	}
	
	for (i = 0x00; i < 0x02; i++)
	{
		switch((value >> i * 4) & 0x0F)
		{
			case 0x00: bufferLCD[0 + i] |= 0x0F; bufferLCD[1 + i] |= 0xA0; break;
			case 0x01: bufferLCD[0 + i] |= 0x06; break;
			case 0x02: bufferLCD[0 + i] |= 0x0D; bufferLCD[1 + i] |= 0x60; break;
			case 0x03: bufferLCD[0 + i] |= 0x0F; bufferLCD[1 + i] |= 0x40; break;
			case 0x04: bufferLCD[0 + i] |= 0x06; bufferLCD[1 + i] |= 0xC0; break;
			case 0x05: bufferLCD[0 + i] |= 0x0B; bufferLCD[1 + i] |= 0xC0; break;
			case 0x06: bufferLCD[0 + i] |= 0x0B; bufferLCD[1 + i] |= 0xE0; break;
			case 0x07: bufferLCD[0 + i] |= 0x0E; break;
			case 0x08: bufferLCD[0 + i] |= 0x0F; bufferLCD[1 + i] |= 0xE0; break;
			case 0x09: bufferLCD[0 + i] |= 0x0F; bufferLCD[1 + i] |= 0xC0; break;
			default:		bufferLCD[0] = 0x80; bufferLCD[1] = 0x40;
									bufferLCD[2] = 0x42; driverLCD_WriteRAM(bufferLCD, 0x03, 0x04);
									return;
		}
	}
	
	driverLCD_WriteRAM(bufferLCD, 0x03, 0x04);
}
