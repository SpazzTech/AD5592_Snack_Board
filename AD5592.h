/**
 * File: AD5592.h
 * Target: AD5592 
 * Function: Helper file to improve code readability
 * Dependancies: 
 * 		-none
 * Author: Tom Olenik
 * Original Date: 15 November 2016
 * Last Revised Date: 25 November 2016
 * Release Notes:
 * 	* Version 1.0.0: 15 November 2016
 * 		- Initial release includes definitions to help make application
 * 		code more readable when interfacing with an AD5592. 
 *  * Version 1.0.1: 18 November 2016
 * 		- Added file header info.
 * 		- Commented out include SPI.h. This should be done in
 * 		application code. 
 * 		- Added AD5592_GPIO_READ_INPUT mask for reading digital
 *  	inputs.
 *  * Version 1.0.2: 25 November 2016
 *    - Line 58: Fixed AD5592_SW_RESET and formatting update.
 **********************************************************************/

#ifndef SOURCES_AD5592_H_
#define SOURCES_AD5592_H_

/**
 * 	Change the #include to use the Serial Peripheral Diver header.
 */
//#include "SPI.h"

/**
 * Define the number of bytes in a standard word for the
 * SPI device used.
 */
#define SPI_WORD_BYTES				1U		/* Number of bytes in SPI word for device */

/**
 * Control register definitions.
 * Refer to page 24 of AD5592R datasheet Rev A.
 */
#define AD5592_CNTRL_ADDRESS_MASK	  0x7800	/* Control register bit mask */
#define	AD5592_NOP					        0x0000	/* No operation */
#define	AD5592_DAC_READBACK			    0x0800	/* Selects and enables DAC read back */
#define	AD5592_ADC_READ				      0x1000	/* Selects ADCs for conversion */
#define	AD5592_GP_CNTRL				      0x1800	/* General purpose control register */
#define	AD5592_ADC_PIN_SELECT		    0x2000	/* Selects which pins are ADC inputs */
#define AD5592_DAC_PIN_SELECT		    0x2800	/* Selects which pins are DAC outputs */
#define	AD5592_PULL_DOWN_SET		    0x3000	/* Selects which pins have 85kOhm pull-down resistor to GND */
#define	AD5592_CNTRL_REG_READBACK	  0x3800	/* Read back control registers and/or set LDAC */
#define	AD5592_GPIO_WRITE_CONFIG	  0x4000	/* Selects which pins are GPIO outputs */
#define	AD5592_GPIO_WRITE_DATA		  0x4800	/* Writes data to the GPIO outputs */
#define	AD5592_GPIO_READ_CONFIG		  0x5000	/* Selects which pins are GPIO inputs */
#define AD5592_GPIO_READ_INPUT		  0x5400	/* Read GPIO inputs */
#define	AD5592_POWER_DWN_REF_CNTRL	0x5800	/* Powers down DACs and enables/disables the reference */
#define	AD5592_GPIO_DRAIN_CONFIG	  0x6000	/* Selects open-drain or push/pull for GPIO outputs */
#define AD5592_THREE_STATE_CONFIG	  0x6800	/* Selects which pins are three-state */
#define	AD5592_SW_RESET				      0x7DAC	/* Software reset of the AD5592 */

/**
 * Pins
 */
#define AD5592_IO0	0x0001
#define AD5592_IO1	0x0002
#define AD5592_IO2	0x0004
#define	AD5592_IO3	0x0008
#define	AD5592_IO4	0x0010
#define	AD5592_IO5	0x0020
#define	AD5592_IO6	0x0040
#define	AD5592_IO7	0x0080

/**
 * DAC register definitions.
 */
#define AD5592_DAC_WRITE_MASK		0x8000	/* DAC write bit mask */
#define AD5592_DAC_ADDRESS_MASK		0x7000	/* DAC pin address bit mask */
#define AD5592_DAC_VALUE_MASK		0x0FFF	/* DAC output value bit mask */

#define AD5592_PIN_SELECT_MASK		0x00FF	/* Pin select bit mask */

typedef unsigned short int	AD5592_WORD;

#endif /* SOURCES_AD5592_H_ */
