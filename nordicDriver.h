#include "stm8s.h"

#define driver_NORDIC_SCK_PIN					GPIO_Pin_5
#define driver_NORDIC_MISO_PIN				GPIO_Pin_6
#define driver_NORDIC_MOSI_PIN				GPIO_Pin_7
#define driver_NORDIC_GPIO_PORT				GPIOA
#define driver_NORDIC_GPIO_CLK				RCC_APB2Periph_GPIOA
#define driver_NORDIC_SPI							SPI1
#define driver_NORDIC_SPI_CLK	        RCC_APB2Periph_SPI1

#define driver_NORDIC_CSN_PIN					GPIO_Pin_12
#define driver_NORDIC_CSN_PORT				GPIOB
#define driver_NORDIC_CSN_CLK					RCC_APB2Periph_GPIOB
	 
#define driver_NORDIC_CE_PIN					GPIO_Pin_0
#define driver_NORDIC_CE_PORT					GPIOB
#define driver_NORDIC_CE_CLK					RCC_APB2Periph_GPIOB

#define driver_NORDIC_IRQ_PIN					GPIO_Pin_4
#define driver_NORDIC_IRQ_PORT				GPIOA
#define driver_NORDIC_IRQ_CLK					RCC_APB2Periph_GPIOA
	 
#define driver_NORDIC_CE_HIGH					GPIO_SetBits(driver_NORDIC_CE_PORT, driver_NORDIC_CE_PIN)
#define driver_NORDIC_CE_LOW					GPIO_ResetBits(driver_NORDIC_CE_PORT, driver_NORDIC_CE_PIN)

#define driver_NORDIC_CSN_HIGH					GPIO_SetBits(driver_NORDIC_CSN_PORT, driver_NORDIC_CSN_PIN)
#define driver_NORDIC_CSN_LOW					GPIO_ResetBits(driver_NORDIC_CSN_PORT, driver_NORDIC_CSN_PIN)
	 

#define CONFIG      						0x00
#define CONFIG_DEFAULT					0x08
#define EN_AA_ESHBURST   				0x01
#define EN_RXADDR		     				0x02
#define SETUP_AW			   				0x03
#define SETUP_RETR	    				0x04
#define RF_CH				    				0x05
#define RF_SETUP		    				0x06
#define STATUS			    				0x07
#define STATUS_DEFAULT					0x0E
#define OBSERVE_TX	    				0x08
#define RPD					    				0x09
#define RX_ADDR_P0	    				0x0A
#define RX_ADDR_P1	    				0x0B
#define RX_ADDR_P2	    				0x0C
#define RX_ADDR_P3	    				0x0D
#define RX_ADDR_P4	    				0x0E
#define RX_ADDR_P5	    				0x0F
#define TX_ADDR			    				0x10
#define RX_PW_P0		    				0x11
#define RX_PW_P1		    				0x12
#define RX_PW_P2		    				0x13
#define RX_PW_P3		    				0x14
#define RX_PW_P4		    				0x15
#define RX_PW_P5		    				0x16
#define FIFO_STATUS	    				0x17
#define DYNPD				    				0x1C
#define FEATURE			    				0x1D
	 

void driverRF_SPIConfig(void);
void driverRF_INIT(void);
uint8_t driverRF_Get_Status(void);
void driverRF_readRegisters(uint16_t registerAddress, uint8_t * data, uint8_t count);
void driverRF_writeRegisters(uint16_t registerAddress, uint8_t * data, uint8_t count);
void driverRF_SYNCHRONIZE(void);
void forceSPI_SCK(void);
void setupRFDriverGPIO(void);
void driverRF_triggerCEPin(void);
void driverRF_writeTx(uint8_t * data, uint8_t count);
void driverRF_testTransmition(void);
