/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/*------------------------------------------------------------------------------
    Hardware Configuration 
------------------------------------------------------------------------------*/

#define LCD_I2C_PORT									GPIOB
#define LCD_SCL_PIN										GPIO_PIN_4
#define LCD_SDA_PIN										GPIO_PIN_5
#define LCD_I2C											I2C


/*------------------------------------------------------------------------------
	Software Logical Model of a LCD Controller PCA85176
------------------------------------------------------------------------------*/

#define driverLCD_ADDRESS								(uint8_t)0x70				
#define driverLCD_NEXT_CMD								(uint8_t)0x80

#define driverLCD_BLINK_CONFIG							(uint8_t)0x70
#define driverLCD_NORMAL_BLINKING						(uint8_t)0x00
#define driverLCD_AL_RAM_BLINKING						(uint8_t)0x04
#define driverLCD_BLINK_OFF								(uint8_t)0x00
#define driverLCD_BLINK_1x								(uint8_t)0x01 /* Blinking frequency is clock frequency divided by 768 */
#define driverLCD_BLINK_2x								(uint8_t)0x10 /* Blinking frequency is clock frequency divided by 1536 */
#define driverLCD_BLINK_3x								(uint8_t)0x11 /* Blinking frequency is clock frequency divided by 3072 */

#define driverLCD_BANK_SELECT_CMD						(uint8_t)0x78
#define driverLCD_IN_BANK_0								(uint8_t)0x00 /* Use to storage RAM rows 0 and 1 in 1:2 multiplex MODE */
#define driverLCD_IN_BANK_2								(uint8_t)0x02 /* Use to storage RAM rows 2 and 3 in 1:2 multiplex MODE */
#define driverLCD_OUT_BANK_0							(uint8_t)0x00 /* Use to retrieval RAM rows 0 and 1 in 1:2 multiplex MODE */
#define driverLCD_OUT_BANK_2							(uint8_t)0x01 /* Use to retrieval RAM rows 2 and 3 in 1:2 multiplex MODE */

#define driverLCD_SUBADDRESS_CMD						(uint8_t)0x60
#define driverLCD_SUBADDR_0								(uint8_t)0x00
#define driverLCD_SUBADDR_1								(uint8_t)0x01
#define driverLCD_SUBADDR_2								(uint8_t)0x02
#define driverLCD_SUBADDR_3								(uint8_t)0x03
#define driverLCD_SUBADDR_4								(uint8_t)0x04
#define driverLCD_SUBADDR_5								(uint8_t)0x05
#define driverLCD_SUBADDR_6								(uint8_t)0x06
#define driverLCD_SUBADDR_7								(uint8_t)0x07

#define driverLCD_MODE_CMD								(uint8_t)0x40
#define driverLCD_DEV_ENABLE							(uint8_t)0x08
#define driverLCD_DEV_DISABLE							(uint8_t)0x00
#define driverLCD_BIAS_1TO3								(uint8_t)0x00
#define driverLCD_BIAS_1TO2								(uint8_t)0x04
#define driverLCD_ONE_BP								(uint8_t)0x01
#define driverLCD_TWO_BP								(uint8_t)0x02
#define driverLCD_THREE_BP								(uint8_t)0x03
#define driverLCD_FOUR_BP								(uint8_t)0x00

/*------------------------------------------------------------------------------
    LCD Driver Functions 
------------------------------------------------------------------------------*/
void driverLCD_INIT(void);
//void driverLCD_GPIO_I2C_Config(void);
void driverLCD_I2C_Config(void);
//void driverLCD_ClockDomainConfig(void);
void driverLCD_enableCMD(void);
void driverLCD_disableCMD(void);
void driverLCD_WriteRAM(uint8_t * bufferPtr, uint8_t originRAM, uint8_t bytes);
void driverLCD_drawDigits(uint16_t value, bool fraction);
