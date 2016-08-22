#include "stm8s.h"


/*---------------------------------------------------------------------------------------------
				Hardware definitions for RF driver
---------------------------------------------------------------------------------------------*/
#define driver_NORDIC_SCK_PIN					GPIO_PIN_5
#define driver_NORDIC_MISO_PIN					GPIO_PIN_7
#define driver_NORDIC_MOSI_PIN					GPIO_PIN_6
#define driver_NORDIC_SPI_PORT					GPIOC
##define driver_NORDIC_SPI						SPI

#define driver_NORDIC_CSN_PIN					GPIO_PIN_2
#define driver_NORDIC_CSN_PORT					GPIOA

#define driver_NORDIC_CE_PIN					GPIO_PIN_2
#define driver_NORDIC_CE_PORT					GPIOD

#define driver_NORDIC_IRQ_PIN					GPIO_PIN_3
#define driver_NORDIC_IRQ_PORT					GPIOD


/*---------------------------------------------------------------------------------------------
				Macro functions to directly control state of pins
---------------------------------------------------------------------------------------------*/
#define driver_NORDIC_CE_HIGH					driver_NORDIC_CE_PORT->ODR |= driver_NORDIC_CE_PIN
#define driver_NORDIC_CE_LOW					driver_NORDIC_CE_PORT->ODR &= ~driver_NORDIC_CE_PIN

#define driver_NORDIC_CSN_HIGH					driver_NORDIC_CSN_PORT->ODR |= driver_NORDIC_CSN_PIN
#define driver_NORDIC_CSN_LOW					driver_NORDIC_CSN_PORT->ODR &= ~driver_NORDIC_CSN_PIN


/*---------------------------------------------------------------------------------------------
				Command Set
---------------------------------------------------------------------------------------------*/
#define READ_REGISTER_CMD						0x00
#define WRITE_REGISTER_CMD						0x20
#define R_RX_PYLD								0x61
#define W_TX_PYLD								0xA0
#define FLUSH_TX								0xE1
#define FLUSH_RX								0xE2
#define REUSE_TX_PYLD							0xE3
#define R_RX_PYLD_WID							0x60
#define W_ACK_PYLD								0xA8
#define W_TX_PYLD_NOACK							0xB0
#define NOP_CMD									0xFF


/*---------------------------------------------------------------------------------------------
				Memory Map of a RF driver
---------------------------------------------------------------------------------------------*/
#define CONFIG      						0x00
#define CONFIG_DEFAULT						0x08
#define EN_AA_ESHBURST   					0x01
#define EN_RXADDR		     				0x02
#define SETUP_AW			   				0x03
#define SETUP_RETR	    					0x04
#define RF_CH				    			0x05
#define RF_SETUP		    				0x06
#define STATUS			    				0x07
#define STATUS_DEFAULT						0x0E
#define OBSERVE_TX	    					0x08
#define RPD					    			0x09
#define RX_ADDR_P0	    					0x0A
#define RX_ADDR_P1	    					0x0B
#define RX_ADDR_P2	    					0x0C
#define RX_ADDR_P3	    					0x0D
#define RX_ADDR_P4	    					0x0E
#define RX_ADDR_P5	    					0x0F
#define TX_ADDR			    				0x10
#define RX_PW_P0		    				0x11
#define RX_PW_P1		    				0x12
#define RX_PW_P2		    				0x13
#define RX_PW_P3		    				0x14
#define RX_PW_P4		    				0x15
#define RX_PW_P5		    				0x16
#define FIFO_STATUS	    					0x17
#define DYNPD				    			0x1C
#define FEATURE			    				0x1D


/*---------------------------------------------------------------------------------------------
				driver functions
---------------------------------------------------------------------------------------------*/
void driverNordic_SPIConfig(void);
void driverNordicInit(void);
void driverNordic_GPIOConfig(void);
uint8_t driverNordic_GetStatus(void);
void driverNordic_ReadRegisters(uint8_t registerAddress, uint8_t * data, uint8_t count);
void driverNordic_WriteRegisters(uint8_t registerAddress, uint8_t * data, uint8_t count);
void driverNordic_WriteTx(uint8_t * data, uint8_t count);
void driverNordic_TestTransmition(void);
