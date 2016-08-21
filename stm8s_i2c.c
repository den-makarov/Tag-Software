#include "stm8s_i2c.h"

/* I2C register mask */
#define REGISTER_Mask               ((uint16_t)0x3000)
#define REGISTER_SR1_Index          ((uint16_t)0x0100)
#define REGISTER_SR2_Index          ((uint16_t)0x0200)
/* I2C Interrupt Enable mask */
#define ITEN_Mask                   ((uint16_t)0x0700)
/* I2C FLAG mask */
#define FLAG_Mask                   ((uint16_t)0x00FF)



void I2C_Init_Simplified(I2C_TypeDef* I2Cx, I2C_CONFIG_TypeDef * I2C_Configuration)
{
	uint16_t result = 0x0004;
	uint16_t tmpval = 0;
	uint8_t tmpccrh = 0;
	/*------------------------- I2C FREQ Configuration ------------------------*/
	/* Clear frequency bits */
	I2Cx->FREQR &= (uint8_t)(~I2C_FREQR_FREQ);
	/* Write new value */
	I2Cx->FREQR |= I2C_Configuration->InputClockFrequencyMHz;

	/*--------------------------- I2C CCR Configuration ------------------------*/
	/* Disable I2C to configure TRISER */
	I2Cx->CR1 &= (uint8_t)(~I2C_CR1_PE);

	/* Clear CCRH & CCRL */
	I2Cx->CCRH &= (uint8_t)(~(I2C_CCRH_FS | I2C_CCRH_DUTY | I2C_CCRH_CCR));
	I2Cx->CCRL &= (uint8_t)(~I2C_CCRL_CCR);

	/* Set F/S bit for fast mode */
	tmpccrh = I2C_CCRH_FS;

		/* Fast mode speed calculate: Tlow/Thigh = 2 */
	result = (uint16_t) ((I2C_Configuration->InputClockFrequencyMHz * 1000000) / (I2C_Configuration->OutputClockFrequencyHz * 3));

	/* Verify and correct CCR value if below minimum value */
	if (result < (uint16_t)0x01)
	{
		/* Set the minimum allowed value */
		result = (uint16_t)0x0001;
	}

	/* Set Maximum Rise Time: 600ns max in Fast Mode
	= [600ns/(1/InputClockFrequencyMHz.10e6)]+1
	= [(InputClockFrequencyMHz * 6)/10]+1 */
	tmpval = ((I2C_Configuration->InputClockFrequencyMHz * 6) / 10) + 1;
	I2Cx->TRISER = (uint8_t)tmpval;


	/* Write CCR with new calculated value */
	I2Cx->CCRL = (uint8_t)result;
	I2Cx->CCRH = (uint8_t)((uint8_t)((uint8_t)(result >> 8) & I2C_CCRH_CCR) | tmpccrh);

	/* Enable I2C */
	I2Cx->CR1 |= I2C_CR1_PE;

	/* Configure I2C acknowledgement */
	I2C->CR2 &= (uint8_t)(~I2C_CR2_ACK);
	
	/*--------------------------- I2C OAR Configuration ------------------------*/
	I2Cx->OARL = (uint8_t)(I2C_Configuration->OwnAddress);
	I2Cx->OARH = (uint8_t)((uint8_t)(I2C_Configuration->AddMode | I2C_OARH_ADDCONF) | (uint8_t)((I2C_Configuration->OwnAddress & (uint16_t)0x0300) >> (uint8_t)7));
}


/**
 * @brief
 ****************************************************************************************
 *
 *                         I2C State Monitoring Functions
 *
 ****************************************************************************************
 * This I2C driver provides three different ways for I2C state monitoring
 *  depending on the application requirements and constraints:
 *
 *
 * 1) Basic state monitoring:
 *    Using I2C_CheckEvent() function:
 *    It compares the status registers (SR1, SR2 and SR3) content to a given event
 *    (can be the combination of one or more flags).
 *    It returns SUCCESS if the current status includes the given flags
 *    and returns ERROR if one or more flags are missing in the current status.
 *    - When to use:
 *      - This function is suitable for most applications as well as for startup
 *      activity since the events are fully described in the product reference manual
 *      (RM0016).
 *      - It is also suitable for users who need to define their own events.
 *    - Limitations:
 *      - If an error occurs (ie. error flags are set besides to the monitored flags),
 *        the I2C_CheckEvent() function may return SUCCESS despite the communication
 *        hold or corrupted real state.
 *        In this case, it is advised to use error interrupts to monitor the error
 *        events and handle them in the interrupt IRQ handler.
 *
 *        @note
 *        For error management, it is advised to use the following functions:
 *          - I2C_ITConfig() to configure and enable the error interrupts (I2C_IT_ERR).
 *          - I2C_IRQHandler() which is called when the I2C interrupts occur.
 *          - I2C_GetFlagStatus() or I2C_GetITStatus() to be called into the
 *           I2C_IRQHandler() function in order to determine which error occurred.
 *          - I2C_ClearFlag() or I2C_ClearITPendingBit() and/or I2C_SoftwareResetCmd()
 *            and/or I2C_GenerateStop() in order to clear the error flag and
 *            source and return to correct communication status.
 *
 *
 *  2) Advanced state monitoring:
 *     Using the function I2C_GetLastEvent() which returns the image of both SR1
 *     & SR3 status registers in a single word (uint16_t) (Status Register 3 value
 *     is shifted left by 8 bits and concatenated to Status Register 1).
 *     - When to use:
 *       - This function is suitable for the same applications above but it allows to
 *         overcome the limitations of I2C_GetFlagStatus() function (see below).
 *         The returned value could be compared to events already defined in the
 *         library (stm8s_i2c.h) or to custom values defined by user.
 *       - This function is suitable when multiple flags are monitored at the same time.
 *       - At the opposite of I2C_CheckEvent() function, this function allows user to
 *         choose when an event is accepted (when all events flags are set and no
 *         other flags are set or just when the needed flags are set like
 *         I2C_CheckEvent() function).
 *     - Limitations:
 *       - User may need to define his own events.
 *       - Same remark concerning the error management is applicable for this
 *         function if user decides to check only regular communication flags (and
 *         ignores error flags).
 *
 *
 *  3) Flag-based state monitoring:
 *     Using the function I2C_GetFlagStatus() which simply returns the status of
 *     one single flag (ie. I2C_FLAG_RXNE ...).
 *     - When to use:
 *        - This function could be used for specific applications or in debug phase.
 *        - It is suitable when only one flag checking is needed (most I2C events
 *          are monitored through multiple flags).
 *     - Limitations:
 *        - When calling this function, the Status register is accessed. Some flags are
 *          cleared when the status register is accessed. So checking the status
 *          of one Flag, may clear other ones.
 *        - Function may need to be called twice or more in order to monitor one
 *          single event.
 *
 *  For detailed description of Events, please refer to section I2C_Events in
 *  stm8s_i2c.h file.
 *
 */
/**
 *
 *  1) Basic state monitoring
 *******************************************************************************
 */

/**
  * @brief  Checks whether the last I2C Event is equal to the one passed
  *   as parameter.
  * @param  I2C_EVENT: specifies the event to be checked.
  *   This parameter can be one of the following values:
  *     @arg I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED           : EV1
  *     @arg I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED              : EV1
  *     @arg I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED            : EV1
  *     @arg I2C_EVENT_SLAVE_BYTE_RECEIVED                         : EV2
  *     @arg (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_GENCALL)    : EV2
  *     @arg I2C_EVENT_SLAVE_BYTE_TRANSMITTED                      : EV3
  *     @arg (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_GENCALL) : EV3
  *     @arg I2C_EVENT_SLAVE_ACK_FAILURE                           : EV3_2
  *     @arg I2C_EVENT_SLAVE_STOP_DETECTED                         : EV4
  *     @arg I2C_EVENT_MASTER_MODE_SELECT                          : EV5
  *     @arg I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED            : EV6
  *     @arg I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED               : EV6
  *     @arg I2C_EVENT_MASTER_BYTE_RECEIVED                        : EV7
  *     @arg I2C_EVENT_MASTER_BYTE_TRANSMITTING                    : EV8
  *     @arg I2C_EVENT_MASTER_BYTE_TRANSMITTED                     : EV8_2
  *     @arg I2C_EVENT_MASTER_MODE_ADDRESS10                       : EV9
  *
  * @note: For detailed description of Events, please refer to section
  *    I2C_Events in stm8s_i2c.h file.
  *
  * @retval An ErrorStatus enumeration value:
  * - SUCCESS: Last event is equal to the I2C_EVENT
  * - ERROR: Last event is different from the I2C_EVENT
  */
ErrorStatus I2C_CheckEvent(I2C_Event_TypeDef I2C_Event)
{
  __IO uint16_t lastevent = 0x00;
  uint8_t flag1 = 0x00 ;
  uint8_t flag2 = 0x00;
  ErrorStatus status = ERROR;

  /* Check the parameters */
  assert_param(IS_I2C_EVENT_OK(I2C_Event));

  if (I2C_Event == I2C_EVENT_SLAVE_ACK_FAILURE)
  {
    lastevent = I2C->SR2 & I2C_SR2_AF;
  }
  else
  {
    flag1 = I2C->SR1;
    flag2 = I2C->SR3;
    lastevent = ((uint16_t)((uint16_t)flag2 << (uint16_t)8) | (uint16_t)flag1);
  }
  /* Check whether the last event is equal to I2C_EVENT */
  if (((uint16_t)lastevent & (uint16_t)I2C_Event) == (uint16_t)I2C_Event)
  {
    /* SUCCESS: last event is equal to I2C_EVENT */
    status = SUCCESS;
  }
  else
  {
    /* ERROR: last event is different from I2C_EVENT */
    status = ERROR;
  }

  /* Return status */
  return status;
}
