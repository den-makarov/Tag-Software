#include "stm8s_spi.h"

void SPI_Init_Simplified(SPI_CONFIG_TypeDef * SPI_Configuration)
{
	/* Frame Format, BaudRate, Clock Polarity and Phase configuration */
	SPI->CR1 = (uint8_t)((uint8_t)((uint8_t)SPI_Configuration->FirstBit | SPI_Configuration->BaudRatePrescaler) |
					   (uint8_t)((uint8_t)SPI_Configuration->ClockPolarity | SPI_Configuration->ClockPhase));

	/* Data direction configuration: BDM, BDOE and RXONLY bits */
	SPI->CR2 = (uint8_t)((uint8_t)(SPI_Configuration->Data_Direction) | (uint8_t)(SPI_Configuration->Slave_Management));

	if (SPI_Configuration->Mode == SPI_MODE_MASTER)
	{
		SPI->CR2 |= (uint8_t)SPI_CR2_SSI;
	}
	else
	{
		SPI->CR2 &= (uint8_t)~(SPI_CR2_SSI);
	}

	/* Master/Slave mode configuration */
	SPI->CR1 |= (uint8_t)(SPI_Configuration->Mode);
}  




FlagStatus SPI_GetFlagStatus(SPI_Flag_TypeDef SPI_FLAG)
{
  FlagStatus status = RESET;
   
  /* Check the status of the specified SPI flag */
  if ((SPI->SR & (uint8_t)SPI_FLAG) != (uint8_t)RESET)
  {
    status = SET; /* SPI_FLAG is set */
  }
  else
  {
    status = RESET; /* SPI_FLAG is reset*/
  }
  
  /* Return the SPI_FLAG status */
  return status;
}
