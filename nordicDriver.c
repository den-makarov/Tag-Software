/* Includes ------------------------------------------------------------------*/
#include "nordicDriver.h"




/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
void driverNordic_SPIConfig(void)
{
	SPI_CONFIG_TypeDef	SPI_InitStructure;
	
	//SPI_DeInit();
	SPI->CR1    = SPI_CR1_RESET_VALUE;
  SPI->CR2    = SPI_CR2_RESET_VALUE;
  SPI->ICR    = SPI_ICR_RESET_VALUE;
  SPI->SR     = SPI_SR_RESET_VALUE;
  SPI->CRCPR  = SPI_CRCPR_RESET_VALUE;

	// SPI Config 
	SPI_InitStructure.FirstBit = SPI_FIRSTBIT_MSB;
	SPI_InitStructure.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	SPI_InitStructure.Mode = SPI_MODE_MASTER;
	SPI_InitStructure.ClockPolarity = SPI_CLOCKPOLARITY_LOW;
	SPI_InitStructure.ClockPhase = SPI_CLOCKPHASE_1EDGE;
	SPI_InitStructure.Data_Direction = SPI_DATADIRECTION_2LINES_FULLDUPLEX;
	SPI_InitStructure.Slave_Management = SPI_NSS_HARD;
	
	SPI_Init_Simplified(&SPI_InitStructure);

	// SPI enable
	//SPI_Cmd(ENABLE);
	SPI->CR1 |= SPI_CR1_SPE;
}




/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
void driverNordic_GPIOConfig(void)
{
	//	NORDIC_SPI_SCK
	GPIO_Init(driver_NORDIC_SPI_PORT, (GPIO_Pin_TypeDef)driver_NORDIC_SCK_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	
	//	NORDIC_SPI_MISO - Set only direction
	GPIO_Init(driver_NORDIC_SPI_PORT, (GPIO_Pin_TypeDef)driver_NORDIC_MISO_PIN, GPIO_MODE_IN_FL_NO_IT);
	
	//	NORDIC_SPI_MOSI
	GPIO_Init(driver_NORDIC_SPI_PORT, (GPIO_Pin_TypeDef)driver_NORDIC_MOSI_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	
	//	NORDIC_IRQ	-	Set only direction
	GPIO_Init(driver_NORDIC_IRQ_PORT, (GPIO_Pin_TypeDef)driver_NORDIC_IRQ_PIN, GPIO_MODE_IN_FL_NO_IT);
	
	//	NORDIC_CSN	-	Set only direction
	GPIO_Init(driver_NORDIC_CSN_PORT, (GPIO_Pin_TypeDef)driver_NORDIC_CSN_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
	driver_NORDIC_CSN_HIGH;
	
	//	NORDIC_CE	-	Set only direction
	GPIO_Init(driver_NORDIC_CE_PORT, (GPIO_Pin_TypeDef)driver_NORDIC_CE_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
	driver_NORDIC_CE_LOW;
}




/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
void driverNordicInit(void)
{
	driverNordic_GPIOConfig();
	
	//CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef)CLK_PERIPHERAL_SPI, ENABLE);
	CLK->PCKENR1 |= 0x02;
	
	driverNordic_SPIConfig();
}




/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
uint8_t driverNordic_GetStatus(void)
{
	uint8_t status;

	driver_NORDIC_CSN_LOW;
	SPI->DR = NOP_CMD;
	
	while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
	
	driver_NORDIC_CSN_HIGH;	
	status = (uint8_t)SPI->DR;
	
	return status;
}




/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
void driverNordic_ReadRegisters(uint8_t registerAddress, uint8_t * data, uint8_t count)
{
	driver_NORDIC_CSN_LOW;
	
	SPI->DR = (READ_REGISTER_CMD | registerAddress);
	
	while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
	
	*data = (uint8_t)SPI->DR;
	
	while (count-- > 0)
	{
		SPI->DR = NOP_CMD;
	
		while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
		
		*data++ = (uint8_t)SPI->DR;
	}
	driver_NORDIC_CSN_HIGH;
}




/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
void driverNordic_FlushRxFIFO(void)
{
	uint8_t i;
	
	driver_NORDIC_CSN_LOW;
	
	SPI->DR = FLUSH_RX;
	
	while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
	
	i = (uint8_t)SPI->DR;
	
	driver_NORDIC_CSN_HIGH;
}



/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
void driverNordic_ReadRxPayload(uint8_t * data, uint8_t count)
{
	driver_NORDIC_CSN_LOW;
	SPI->DR = R_RX_PYLD;
	
	while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
	
	*data = (uint8_t)SPI->DR;
	
	while (count-- > 0)
	{
		SPI->DR = NOP_CMD;
	
		while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
		
		*data++ = (uint8_t)SPI->DR;
	}
	driver_NORDIC_CSN_HIGH;
}




/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
void driverNordic_WriteTx(uint8_t * data, uint8_t count)
{
	uint8_t i;
	
	driver_NORDIC_CSN_LOW;
	SPI->DR = W_TX_PYLD;
	
	while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
	
	i = (uint8_t)SPI->DR;
	
	while (count-- > 0)
	{
		SPI->DR = *data++;
	
		while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
		
		i = (uint8_t)SPI->DR;
		i++;
	}
	driver_NORDIC_CSN_HIGH;
}




/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
void driverNordic_WriteRegisters(uint8_t registerAddress, uint8_t * data, uint8_t count)
{
	uint8_t i;
	
	driver_NORDIC_CSN_LOW;
	SPI->DR = (WRITE_REGISTER_CMD | registerAddress);
	
	while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
	
	i = (uint8_t)SPI->DR;
	
	while (count-- > 0)
	{
		SPI->DR = *data++;
	
		while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);
		
		i = (uint8_t)SPI->DR;
		i++;
	}
	driver_NORDIC_CSN_HIGH;
}




/*---------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------*/
/*void driverNordic_TestTransmition(void)
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
		driver_NORDIC_CE_HIGH;
		
		// Delay for 130 us - Tx settling time
		// Four nop cycles at 31250 Hz CPU clocking frequency
		for (i = 0; i < 4; i++);
	
		driverNordic_WriteTx(data, 0x05);
	
		driver_NORDIC_CE_LOW;
	
		data[0] = driverNordic_GetStatus();
		driverNordic_WriteRegisters(STATUS, &data[0], 0x01);
	//}
}*/
