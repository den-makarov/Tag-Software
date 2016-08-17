/* Includes ------------------------------------------------------------------*/
#include "nordicDriver.h"

#define readRegisterCmd				0x00
#define writeRegisterCmd			0x20
#define R_RX_PYLD							0x61
#define W_TX_PYLD							0xA0
#define FLUSH_TX							0xE1
#define FLUSH_RX							0xE2
#define REUSE_TX_PYLD					0xE3
#define R_RX_PYLD_WID					0x60
#define W_ACK_PYLD						0xA8
#define W_TX_PYLD_NOACK				0xB0
#define nopCmd								0xFF

void delaySPI(uint32_t);




void driverRF_SPIConfig(void)
{
	SPI_TypeDef    SPI_InitStructure;
  GPIO_TypeDef   GPIO_InitStructure;

	delaySPI(1000);
	
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(driver_NORDIC_GPIO_CLK | driver_NORDIC_IRQ_CLK | driver_NORDIC_CSN_CLK | driver_NORDIC_CE_CLK, ENABLE);
  //GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);

  /* Enable NORDIC SPI clock  */
  RCC_APB2PeriphClockCmd(driver_NORDIC_SPI_CLK, ENABLE);
  
  /* Configure NORDIC SPI pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = driver_NORDIC_SCK_PIN | driver_NORDIC_MISO_PIN | driver_NORDIC_MOSI_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(driver_NORDIC_GPIO_PORT, &GPIO_InitStructure);
  
  /* Configure NORDIC pins: IRQ */
  GPIO_InitStructure.GPIO_Pin = driver_NORDIC_IRQ_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(driver_NORDIC_IRQ_PORT, &GPIO_InitStructure);
		
	/* Configure NORDIC pins: CSN */
  GPIO_InitStructure.GPIO_Pin = driver_NORDIC_CSN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(driver_NORDIC_CSN_PORT, &GPIO_InitStructure);
	driver_NORDIC_CSN_HIGH;
	
	/* Configure NORDIC pins: CE */
  GPIO_InitStructure.GPIO_Pin = driver_NORDIC_CE_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(driver_NORDIC_CE_PORT, &GPIO_InitStructure);
	driver_NORDIC_CE_LOW;
	
	SPI_I2S_DeInit(driver_NORDIC_SPI);
  	
  /* SPI Config */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init(driver_NORDIC_SPI, &SPI_InitStructure);

  /* SPI enable */
  SPI_Cmd(driver_NORDIC_SPI, ENABLE);
}




void driverRF_INIT(void)
{
	driverRF_SPIConfig();
	//driverRF_SYNCHRONIZE();
}




uint8_t driverRF_Get_Status(void)
{
	uint8_t status;

	//SPI_I2S_SendData(driver_NORDIC_SPI, (uint16_t)(readRegisterCmd | STATUS));
	driver_NORDIC_CSN_LOW;
	SPI_I2S_SendData(driver_NORDIC_SPI, nopCmd);
	
	while(SPI_I2S_GetFlagStatus(driver_NORDIC_SPI, SPI_I2S_FLAG_BSY) != RESET);
	
	driver_NORDIC_CSN_HIGH;	
	status = (uint8_t)SPI_I2S_ReceiveData(driver_NORDIC_SPI);
	
	return status;
}




void driverRF_readRegisters(uint16_t registerAddress, uint8_t * data, uint8_t count)
{
	driver_NORDIC_CSN_LOW;
	SPI_I2S_SendData(driver_NORDIC_SPI, ((uint16_t)readRegisterCmd) | registerAddress);
	
	while(SPI_I2S_GetFlagStatus(driver_NORDIC_SPI, SPI_I2S_FLAG_BSY) != RESET);
	
	*data = (uint8_t)SPI_I2S_ReceiveData(driver_NORDIC_SPI);
	
	while (count-- > 0)
	{
		SPI_I2S_SendData(driver_NORDIC_SPI, (uint16_t)nopCmd);
	
		while(SPI_I2S_GetFlagStatus(driver_NORDIC_SPI, SPI_I2S_FLAG_BSY) != RESET);
		
		*data++ = (uint8_t)SPI_I2S_ReceiveData(driver_NORDIC_SPI);
	}
	driver_NORDIC_CSN_HIGH;
}




void driverRF_writeTx(uint8_t * data, uint8_t count)
{
	uint16_t i;
	
	driver_NORDIC_CSN_LOW;
	SPI_I2S_SendData(driver_NORDIC_SPI, (uint16_t)W_TX_PYLD);
	
	while(SPI_I2S_GetFlagStatus(driver_NORDIC_SPI, SPI_I2S_FLAG_BSY) != RESET);
	
	i = SPI_I2S_ReceiveData(driver_NORDIC_SPI);
	
	while (count-- > 0)
	{
		SPI_I2S_SendData(driver_NORDIC_SPI, (uint8_t)*data++);
	
		while(SPI_I2S_GetFlagStatus(driver_NORDIC_SPI, SPI_I2S_FLAG_BSY) != RESET);
		
		i = SPI_I2S_ReceiveData(driver_NORDIC_SPI);
		i++;
	}
	driver_NORDIC_CSN_HIGH;
}



void driverRF_triggerCEPin(void)
{
	driver_NORDIC_CE_HIGH;
	delaySPI(100);
	//driver_NORDIC_CE_LOW;
}



void driverRF_writeRegisters(uint16_t registerAddress, uint8_t * data, uint8_t count)
{
	uint16_t i;
	
	driver_NORDIC_CSN_LOW;
	SPI_I2S_SendData(driver_NORDIC_SPI, ((uint16_t)writeRegisterCmd) | registerAddress);
	
	while(SPI_I2S_GetFlagStatus(driver_NORDIC_SPI, SPI_I2S_FLAG_BSY) != RESET);
	
	i = SPI_I2S_ReceiveData(driver_NORDIC_SPI);
	
	while (count-- > 0)
	{
		SPI_I2S_SendData(driver_NORDIC_SPI, (uint8_t)*data++);
	
		while(SPI_I2S_GetFlagStatus(driver_NORDIC_SPI, SPI_I2S_FLAG_BSY) != RESET);
		
		i = SPI_I2S_ReceiveData(driver_NORDIC_SPI);
		i++;
	}
	driver_NORDIC_CSN_HIGH;
}




void driverRF_SYNCHRONIZE(void)
{
	//SPI_I2S_SendData(driver_SPI, ((uint16_t)readRegisterCmd) | STATUS);
	driver_NORDIC_CSN_LOW;
	SPI_I2S_SendData(driver_NORDIC_SPI, nopCmd);
	
	while(SPI_I2S_GetFlagStatus(driver_NORDIC_SPI, SPI_I2S_FLAG_BSY) != RESET);
	
	if (((SPI_I2S_ReceiveData(driver_NORDIC_SPI)) & 0x00FF) != (uint8_t)STATUS_DEFAULT)
	{
		forceSPI_SCK();
		driverRF_SYNCHRONIZE();
	}
	driver_NORDIC_CSN_HIGH;
}




void setupRFDriverGPIO(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//Enable Clock for GPIO port used in SPI driver
	RCC_APB2PeriphClockCmd(driver_NORDIC_IRQ_CLK, ENABLE);
	
	/* Configure NORDIC pins: IRQ */
  GPIO_InitStructure.GPIO_Pin = driver_NORDIC_IRQ_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(driver_NORDIC_IRQ_PORT, &GPIO_InitStructure);
}




void forceSPI_SCK(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_Cmd(driver_NORDIC_SPI, DISABLE);
	  
  /* Configure the GPIO_SPI_SCK and GPIO_SPI_MOSI pins */
  GPIO_InitStructure.GPIO_Pin = driver_NORDIC_SCK_PIN | driver_NORDIC_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(driver_NORDIC_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_ResetBits(driver_NORDIC_GPIO_PORT, driver_NORDIC_MOSI_PIN);
	GPIO_SetBits(driver_NORDIC_GPIO_PORT, driver_NORDIC_SCK_PIN);
	delaySPI(1000);
	GPIO_ResetBits(driver_NORDIC_GPIO_PORT, driver_NORDIC_SCK_PIN);
	delaySPI(1000);
	
	GPIO_InitStructure.GPIO_Pin = driver_NORDIC_SCK_PIN | driver_NORDIC_MOSI_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(driver_NORDIC_GPIO_PORT, &GPIO_InitStructure);
	
	SPI_Cmd(driver_NORDIC_SPI, ENABLE);
}





void driverRF_testTransmition(void)
{
	uint8_t data[5];
	uint32_t i = 0x00;
	uint8_t j = 0x00;
	
	data[0] = (uint8_t)0x00;
	data[1] = (uint8_t)0x3A;
	data[2] = (uint8_t)0x01;
	data[3] = (uint8_t)105;
	data[4] = (uint8_t)0x75;
	
	driverRF_writeRegisters(0x04, &data[0]);
	driverRF_writeRegisters(0x01, &data[0]);
	driverRF_writeRegisters(0x10, &data[4]);
	driverRF_writeRegisters(0x05, &data[3]);
	driverRF_writeRegisters(0x00, &data[1]);
	
	for (i = 0; i < 400000; i++);
	
	for (j = 0; j < 4; j++)
	{
		driver_NORDIC_CE_HIGH;
	
		for (i = 0; i < 4000; i++);
	
		driverRF_writeTx(data, 0x05);
	
		for (i = 0; i < 400000; i++);
		
		driver_NORDIC_CE_LOW;
	
		data[0] = driverRF_Get_Status();
		driverRF_writeRegisters(0x07, &data[0]);
	}
}





void delaySPI(uint32_t nCount)
{
  uint32_t index = 0; 
  for(index = (100 * nCount); index != 0; index--);

}
