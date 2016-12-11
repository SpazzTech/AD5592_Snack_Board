/**
 * File: AD5592RPI.h
 * Target: AD5592 on a Raspberry Pi
 * Function: AD5592 device driver for Raspberry Pi
 * Dependancies: 
 * 		-bcm2835.h v1.20 2015/03/31 04:55:41
 * Author: Tom Olenik
 * Original Date: 11 December 2016
 * Last Revised Date: 11 December 2016
 * Release Notes:
 * 	* Version 1.0.0: 11 December 2016
 * 		- Initial release derrived from from AD5592.h version 1.0.2 and 
 * 			AD5592SnackATP.c version 1.0.0. 
 **********************************************************************/

#ifndef SOURCES_AD5592RPI_H_
#define SOURCES_AD5592RPI_H_

/**
 * 	Change the #include to use the Serial Peripheral Diver header.
 */
#include <bcm2835.h>

/**
 * Define the number of bytes in a standard word for the
 * SPI device used.
 */
#define SPI_WORD_BYTES				1U		/* Number of bytes in SPI word for device */

/**
 * Control register definitions.
 * Refer to page 24 of AD5592R datasheet Rev A.
 */
#define AD5592_CNTRL_ADDRESS_MASK	0x7800	/* Control register bit mask */
#define	AD5592_NOP					0x0000	/* No operation */
#define	AD5592_DAC_READBACK			0x0800	/* Selects and enables DAC read back */
#define	AD5592_ADC_READ				0x1000	/* Selects ADCs for conversion */
#define	AD5592_GP_CNTRL				0x1800	/* General purpose control register */
#define	AD5592_ADC_PIN_SELECT		0x2000	/* Selects which pins are ADC inputs */
#define AD5592_DAC_PIN_SELECT		0x2800	/* Selects which pins are DAC outputs */
#define	AD5592_PULL_DOWN_SET		0x3000	/* Selects which pins have 85kOhm pull-down resistor to GND */
#define	AD5592_CNTRL_REG_READBACK	0x3800	/* Read back control registers and/or set LDAC */
#define	AD5592_GPIO_WRITE_CONFIG	0x4000	/* Selects which pins are GPIO outputs */
#define	AD5592_GPIO_WRITE_DATA		0x4800	/* Writes data to the GPIO outputs */
#define	AD5592_GPIO_READ_CONFIG		0x5000	/* Selects which pins are GPIO inputs */
#define AD5592_GPIO_READ_INPUT		0x5400	/* Read GPIO inputs */
#define	AD5592_POWER_DWN_REF_CNTRL	0x5800	/* Powers down DACs and enables/disables the reference */
#define	AD5592_GPIO_DRAIN_CONFIG	0x6000	/* Selects open-drain or push/pull for GPIO outputs */
#define AD5592_THREE_STATE_CONFIG	0x6800	/* Selects which pins are three-state */
#define	AD5592_SW_RESET				0x7DAC	/* Software reset of the AD5592 */

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

#define AD5592_PIN_SELECT_MASK		0x00FF	/* Pin select bit mask */

/**
 * DAC register definitions.
 */
#define AD5592_DAC_WRITE_MASK		0x8000	/* DAC write bit mask */
#define AD5592_DAC_ADDRESS_MASK		0x7000	/* DAC pin address bit mask */
#define AD5592_DAC_VALUE_MASK		0x0FFF	/* DAC output value bit mask */

/**
 * Other useful macros
 */
#define	SHORT_DELAY	10		/* Delay used to let device do it's thing */
#define LONG_DELAY	50		/* Longer delay to give it more time */

#define CHANNEL0			BCM2835_SPI_CS0
#define	CHANNEL1			BCM2835_SPI_CS1

char spiOut[2]; 			/* SPI output buffer */
char spiIn[2];	 			/* SPI input buffer  */

uint16_t mV;					/* millivolts */
uint16_t result;				/* result */
uint8_t digitalOutPins = 0x00;	/* Bit mask of pins currently set as digital out */
uint8_t digitalInPins = 0x00;	/* Bit mask of pins currently set as digital in */
uint8_t analogOutPins = 0x00;	/* Bit mask of pins currently set as analog out */
uint8_t analogInPins = 0x00;	/* Bit mask of pins currently set as analog in */

typedef unsigned short int	AD5592_WORD;

/**
 * Clear the spi buffer.
 * Parameters:
 * 	spiBuffer[] = spi buffer
 */
void clearBuffer(char spiBuffer[]);

/**
 * Parse the 16 bit word of the AD5592 into two 8 bit chunks that
 * works with the bcm2835 library.
 * Parameters:
 * 	eightBits[] = spi buffer
 * 	sixteenBits = AD5592 spi word
 */
void makeWord(char eightBits[], AD5592_WORD sixteenBits);

/**
 * Select the SPI channel.
 * Parameters:
 * 	ch = channel number 
 */
void setAD5592Ch(int ch);

/**
 * Convert a voltage to an digital value based upon assumptions of
 * 0 - 5V input and 12 bit ADC/DAC.
 * Parameter: millivolts as 16 bit unsigned integer
 * Returns: digital value as 16 bit unsigned integer
 */
 uint16_t a2d(uint16_t millivolts);

/**
 * Convert a digital value to voltage based upon assumptions of
 * 0 - 5V input and 12 bit ADC/DAC.
 * Parameter: digital value as 16 bit unsigned integer
 * Returns: millivolts as 16 bit unsigned integer
 */
uint16_t d2a(uint16_t count);

/**
 * Set pins to digital outputs.
 * Parameter: Pins as bit mask
 */
void setAsDigitalOut(AD5592_WORD pins);

/**
 * Set pins to a digital inputs.
 * Parameter: Pins as bit mask
 */
 void setAsDigitalIn(AD5592_WORD pins);
 
/**
 * Set pins to analog outputs.
 * Parameter: Pins as bit mask
 */
void setAsDAC(AD5592_WORD pins);

/**
 * Set pins to a analog inputs.
 * Parameter: Pins as bit mask
 */
 void setAsADC(AD5592_WORD pins);
 
/**
 * SPI communications
 * Parameter:
 * 	Command to send.
 */
void spiComs(AD5592_WORD command);

/**
 * Set a pin to high or low output.
 * Parameters:
 * 	Pins to output as bit mask
 * 	State to be output as bit mask
 */
void setDigitalOut(uint8_t pins, uint8_t states);
 
/**
 * Get the digital input states
 * Parameter:
 * 	Pins to read as digital inputs as bit mask
 * Returns:
 * 	Pin states as bit mask. Unused pins are 0s.
 */
uint8_t getDigitalIn(uint8_t pins);

/**
 * Set an analog output value
 * Parameters:
 * 	Pin number to write to as number (0 to 7)
 *  Value to write in milivolts (assumes 5V reference)
 */
void setAnalogOut(uint8_t pin, uint16_t milivolts);

/**
 * Get analog input value
 * Parameters:
 * 	Pin number to to get value for as number (0 to 7)
 * Returns:
 * 	milivolts (assumes 5V reference)
 */
 
/**
 * Initialize the SPI for using the AD5592. Does not set channel. Do that
 * after calling this function by calling setAD5592Ch().
 * Parameters:
 * 	none
 */
void AD5592_Init();

#endif /* SOURCES_AD5592RPI_H_ */
