#include "driverLCD.h"

/**
  * @brief  Initializes the display driver.
  * @param  None
  * @retval None
  */
void driverLCD_setup(void)
{
	driverLCD_I2C_init();
	
	/* Uncomment in case of LCD driver external clocking */
	//driverLCD_Clock_init();
}

/**
  * @brief  Initializes the I2C display driver.
  * @param  None
  * @retval None
  */
void driverLCD_I2C_init(void)
{
	driverLCD_ClockDomainConfig();
	driverLCD_GPIO_I2C_Config();
	driverLCD_I2C_Config();
}


/**
  * @brief  Initializes the I2C display driver clocking.
  * @param  None
  * @retval None
  */
void driverLCD_ClockDomainConfig(void)
{
	 /* Enable driverLCD_I2C clock */
  CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef)CLK_PERIPHERAL_I2C, ENABLE);
}

/**
  * @brief  Initializes the GPIO pins used by the I2C display driver.
  * @param  None
  * @retval None
  */
void driverLCD_GPIO_I2C_Config(void)
{
  /* Initialize I/Os in Input Floating Mode */
	GPIO_Init(LCD_I2C_PORT, (GPIO_Pin_TypeDef)LCD_SCL_PIN, GPIO_MODE_OUT_OD_HIZ_FAST);
	GPIO_Init(LCD_I2C_PORT, (GPIO_Pin_TypeDef)LCD_SDA_PIN, GPIO_MODE_OUT_OD_HIZ_FAST);
}

/**
  * @brief  Configure the I2C Peripheral used to communicate with LCD driver.
  * @param  None
  * @retval None
  */
void driverLCD_I2C_Config(void)
{
	struct I2C_CONFIG_TypeDef I2C_Configuration;
	
	/*
	uint32_t OutputClockFrequencyHz;
	uint16_t OwnAddress;
	I2C_DutyCycle_TypeDef I2C_DutyCycle;
	I2C_Ack_TypeDef Ack;
	I2C_AddMode_TypeDef AddMode;
	uint8_t InputClockFrequencyMHz;
	*/
	
	I2C_Configuration.OutputClockFrequencyHz = 400000;
	I2C_Configuration.OwnAddress = 0x00;
	I2C_Configuration.I2C_DutyCycle = I2C_DUTYCYCLE_2;
	I2C_Configuration.Ack = 400000; //I2C_Ack_Enable
	I2C_Configuration.AddMode = 400000; //I2C_Mode_I2C
	I2C_Configuration.InputClockFrequencyMHz = 400000;
  
  /* IOE_I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = driverLCD_I2C_SPEED;
  
  I2C_Init(driverLCD_I2C, &I2C_InitStructure);
	I2C_Cmd(driverLCD_I2C, ENABLE);
}

/**
  * @brief  Initializes the Clock signal for display driver.
  * @param  None
  * @retval None
  */
void driverLCD_Clock_init(void)
{
	driverLCD_GPIO_Clock_Config();
	driverLCD_Clock_Config();
}

/**
  * @brief  Initializes the GPIO pin used by the Clock Timer for display driver.
  * @param  None
  * @retval None
  */
void driverLCD_GPIO_Clock_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable driverLCD_Timer_PORT clock */
	RCC_APB2PeriphClockCmd(RCC_APB_driverLCD_Clock_PORT, ENABLE);
	
	/* driverLCD_Clock pin configuration */
	GPIO_InitStructure.GPIO_Pin = driverLCD_Clock;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(driverLCD_Clock_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Initializes the Timer module used to clock display driver.
  * @param  None
  * @retval None
  */
void driverLCD_Clock_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t CCR1_Val = 6000000 / 2000;
	uint16_t PrescalerValue = 0;
	
	TIM_TypeDef Timer_InitStructure;
	
	/* Enable driverLCD_Timer clock */
	RCC_APB1PeriphClockCmd(RCC_APB_Periph_driverLCD_Timer, ENABLE);

  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

	
	/* driverLCD_Timer configuration */
	/* ---------------------------------------------------------------
    TIM2 Configuration: Output Compare Timing Mode:
    TIM2 counter clock at 6 MHz
    CC1 update rate = TIM2 counter clock / CCR1_Val = 2000 Hz
  --------------------------------------------------------------- */

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemFrequency / 6000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}

void setupDriverLCD_BP_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable driverLCD_BackPlanes_PORT PB12, PB13, PB14, PB15 */
	RCC_APB2PeriphClockCmd(RCC_APB_driverLCD_BP_PORT, ENABLE);
	
	/* driverLCD_Clock pin configuration */
	//GPIO_InitStructure.GPIO_Pin = (driverLCD_BP0 | driverLCD_BP1 | driverLCD_BP2);
	GPIO_InitStructure.GPIO_Pin = (driverLCD_BP0 | driverLCD_BP1 | driverLCD_BP2 | driverLCD_BP3);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;
	GPIO_Init(driverLCD_BP_PORT, &GPIO_InitStructure);
}

void driverLCD_enableCMD(void)
{
	I2C_Cmd(driverLCD_I2C, DISABLE);
  
  /* Reset all I2C1 registers */
  I2C_SoftwareResetCmd(driverLCD_I2C, ENABLE);
  I2C_SoftwareResetCmd(driverLCD_I2C, DISABLE);
  
  /* Configure the I2C1 peripheral */
	driverLCD_I2C_Config();
	
	lcdDriverGenerateStart(driverLCD_I2C);
	//I2C_GenerateSTART(driverLCD_I2C, ENABLE);
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	lcdDriverSend7bitAddress(driverLCD_I2C, driverLCD_ADDRESS);
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	lcdDriverSendData(driverLCD_I2C, (driverLCD_MODE_CMD| driverLCD_DEV_ENABLE));
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	lcdDriverGenerateStop(driverLCD_I2C);
	//I2C_GenerateSTOP(driverLCD_I2C, ENABLE);
}

void driverLCD_disableCMD(void)
{
	I2C_Cmd(driverLCD_I2C, DISABLE);
  
  /* Reset all I2C1 registers */
  I2C_SoftwareResetCmd(driverLCD_I2C, ENABLE);
  I2C_SoftwareResetCmd(driverLCD_I2C, DISABLE);
  
  /* Configure the I2C1 peripheral */
	driverLCD_I2C_Config();
	
	lcdDriverGenerateStart(driverLCD_I2C);
	//I2C_GenerateSTART(driverLCD_I2C, ENABLE);
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	lcdDriverSend7bitAddress(driverLCD_I2C, driverLCD_ADDRESS);
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	lcdDriverSendData(driverLCD_I2C, (driverLCD_MODE_CMD| driverLCD_DEV_DISABLE));
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	lcdDriverGenerateStop(driverLCD_I2C);
	//I2C_GenerateSTOP(driverLCD_I2C, ENABLE);
}

void driverLCD_WriteRAM(uint8_t * bufferPtr, uint8_t originRAM, uint8_t bytes)
{
	I2C_Cmd(driverLCD_I2C, DISABLE);
  
  /* Reset all I2C1 registers */
  I2C_SoftwareResetCmd(driverLCD_I2C, ENABLE);
  I2C_SoftwareResetCmd(driverLCD_I2C, DISABLE);
  
  /* Configure the I2C1 peripheral */
	driverLCD_I2C_Config();
	
	lcdDriverGenerateStart(driverLCD_I2C);
	//I2C_GenerateSTART(driverLCD_I2C, ENABLE);
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(driverLCD_I2C, driverLCD_ADDRESS, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(driverLCD_I2C, (driverLCD_NEXT_CMD | driverLCD_SUBADDRESS_CMD | driverLCD_SUBADDR_0));
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(driverLCD_I2C, originRAM);
	while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	while(bytes-- > 0x00)
	{
		I2C_SendData(driverLCD_I2C, *bufferPtr++);
		while (!I2C_CheckEvent(driverLCD_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	lcdDriverGenerateStop(driverLCD_I2C);
	//I2C_GenerateSTOP(driverLCD_I2C, ENABLE);
}

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
	
	switch(value & 0x00F0)
	{
		case 0x0000: bufferLCD[1] |= 0x0F; bufferLCD[2] |= 0xA0; break;
		case 0x0010: bufferLCD[1] |= 0x06; break;
		case 0x0020: bufferLCD[1] |= 0x0D; bufferLCD[2] |= 0x60; break;
		case 0x0030: bufferLCD[1] |= 0x0F; bufferLCD[2] |= 0x40; break;
		case 0x0040: bufferLCD[1] |= 0x06; bufferLCD[2] |= 0xC0; break;
		case 0x0050: bufferLCD[1] |= 0x0B; bufferLCD[2] |= 0xC0; break;
		case 0x0060: bufferLCD[1] |= 0x0B; bufferLCD[2] |= 0xE0; break;
		case 0x0070: bufferLCD[1] |= 0x0E; break;
		case 0x0080: bufferLCD[1] |= 0x0F; bufferLCD[2] |= 0xE0; break;
		case 0x0090: bufferLCD[1] |= 0x0F; bufferLCD[2] |= 0xC0; break;
		default: 
			bufferLCD[0] = 0x80;
			bufferLCD[1] = 0x40;
			bufferLCD[2] = 0x42;
			driverLCD_WriteRAM(bufferLCD, 0x03, 0x04);
			return;
	}
	
	switch(value & 0x000F)
	{
		case 0: bufferLCD[0] |= 0x0F; bufferLCD[1] |= 0xA0; break;
		case 1: bufferLCD[0] |= 0x06; break;
		case 2: bufferLCD[0] |= 0x0D; bufferLCD[1] |= 0x60; break;
		case 3: bufferLCD[0] |= 0x0F; bufferLCD[1] |= 0x40; break;
		case 4: bufferLCD[0] |= 0x06; bufferLCD[1] |= 0xC0; break;
		case 5: bufferLCD[0] |= 0x0B; bufferLCD[1] |= 0xC0; break;
		case 6: bufferLCD[0] |= 0x0B; bufferLCD[1] |= 0xE0; break;
		case 7: bufferLCD[0] |= 0x0E; break;
		case 8: bufferLCD[0] |= 0x0F; bufferLCD[1] |= 0xE0; break;
		case 9: bufferLCD[0] |= 0x0F; bufferLCD[1] |= 0xC0; break;
		default: 
			bufferLCD[0] = 0x80;
			bufferLCD[1] = 0x40;
			bufferLCD[2] = 0x42;
			driverLCD_WriteRAM(bufferLCD, 0x03, 0x04);
			return;
	}
	
	driverLCD_WriteRAM(bufferLCD, 0x03, 0x04);
}

/**************************************************************
---------			LOW LEVEL I2C DRIVER FUNCTION				-------------
**************************************************************/
#define CR1_STOP_Set            ((uint16_t)0x0200)

void lcdDriverGenerateStop(I2C_TypeDef* I2Cx)
{
    I2Cx->CR1 |= CR1_STOP_Set;
}

#define CR1_START_Set           ((uint16_t)0x0100)

void lcdDriverGenerateStart(I2C_TypeDef* I2Cx)
{
   I2Cx->CR1 |= CR1_START_Set;
}

#define OAR1_ADD0_Reset         ((uint8_t)0xFE)
void lcdDriverSend7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address)
{
  Address &= OAR1_ADD0_Reset;
  I2Cx->DR = Address;
}

void lcdDriverSendData(I2C_TypeDef* I2Cx, uint8_t Data)
{
  I2Cx->DR = Data;
}
