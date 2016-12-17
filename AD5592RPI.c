/***********************************************************************
 * File: AD5592RPI.c
 * Target: AD5592 on a Raspberry Pi
 * Function: AD5592 device driver for Raspberry Pi
 * Dependancies: 
 * 		-bcm2835.h v1.20 2015/03/31 04:55:41
 * 		-AD5592RPI.h
 * Author: Tom Olenik
 * Original Date: 11 December 2016
 * Last Revised Date: 17 December 2016
 * Release Notes:
 * 	* Version 1.0.0: 11 December 2016
 * 		- Initial release derrived from from AD5592.h version 1.0.2 and 
 * 			AD5592SnackATP.c version 1.0.0. 
 *	* Version 1.0.1: 17 December 2016
 		- Several fixes. Version 1.0.0 didn't work. Don't use it.
 **********************************************************************/

#include "AD5592RPI.h"

/**
 * Clear the spi buffer.
 * Parameters:
 * 	spiBuffer[] = spi buffer
 */
void clearBuffer(char spiBuffer[])
{
	int i;
	for(i=0;i<sizeof(spiBuffer);i++)
	{
		spiBuffer[i] = 0x00;
	}
}

/**
 * Parse the 16 bit word of the AD5592 into two 8 bit chunks that
 * works with the bcm2835 library.
 * Parameters:
 * 	eightBits[] = spi buffer
 * 	sixteenBits = AD5592 spi word
 */
void makeWord(char eightBits[], AD5592_WORD sixteenBits)
{
	clearBuffer(eightBits);
	eightBits[0] = (sixteenBits & 0xFF00) >> 8;
	eightBits[1] = sixteenBits & 0xFF;
}

/**
 * Select the SPI channel.
 * Parameters:
 * 	ch = channel number
 */
void setAD5592Ch(int ch)
{
	switch(ch){
		case 0:
			bcm2835_spi_chipSelect(CHANNEL0);
			bcm2835_spi_setChipSelectPolarity(CHANNEL0, LOW);
			break;
		case 1:
			bcm2835_spi_chipSelect(CHANNEL1);
			bcm2835_spi_setChipSelectPolarity(CHANNEL1, LOW);
			break;
	}
}

/**
 * Convert a voltage to an digital value based upon assumptions of
 * 0 - 5V input and 12 bit ADC/DAC.
 * Parameter: millivolts as 16 bit unsigned integer
 * Returns: digital value as 16 bit unsigned integer
 */
 uint16_t a2d(uint16_t millivolts)
 {
	 uint16_t count = millivolts * .819f;
	 return count;
 }

/**
 * Convert a digital value to voltage based upon assumptions of
 * 0 - 5V input and 12 bit ADC/DAC.
 * Parameter: digital value as 16 bit unsigned integer
 * Returns: millivolts as 16 bit unsigned integer
 */
uint16_t d2a(uint16_t count)
{
	uint16_t millivolts = count / .819f;
	return millivolts;
}

/**
 * Set pins to digital outputs.
 * Parameter: Pins as bit mask
 */
void setAsDigitalOut(uint8_t pins)
{
	digitalOutPins = pins;	/* Log which pins are configured */
	makeWord(spiOut, AD5592_GPIO_WRITE_CONFIG | pins); 	/* Make the word */
	bcm2835_spi_transfern(spiOut, sizeof(spiOut));		/* Send it */
}

/**
 * Set pins to a digital inputs.
 * Parameter: Pins as bit mask
 */
 void setAsDigitalIn(uint8_t pins)
{
	digitalInPins = pins;	/* Log which pins are configured */
	makeWord(spiOut, AD5592_GPIO_READ_CONFIG | pins); 	/* Make the word */
	bcm2835_spi_transfern(spiOut, sizeof(spiOut));		/* Send it */
}

/**
 * Set pins to analog outputs.
 * Parameter: Pins as bit mask
 */
void setAsDAC(uint8_t pins)
{
	analogOutPins = pins;	/* Log which pins are configured */
	makeWord(spiOut, AD5592_DAC_PIN_SELECT | pins); 	/* Make the word */
	bcm2835_spi_transfern(spiOut, sizeof(spiOut));		/* Send it */
	bcm2835_delay(SHORT_DELAY);
}

/**
 * Set pins to a analog inputs.
 * Parameter: Pins as bit mask
 */
 void setAsADC(uint8_t pins)
{
	analogInPins = pins;	/* Log which pins are configured */
	makeWord(spiOut, AD5592_ADC_PIN_SELECT | pins);		/* Make the word */
	bcm2835_spi_transfern(spiOut, sizeof(spiOut));		/* Send it */
	bcm2835_delay(SHORT_DELAY);
}

/**
 * SPI communications
 * Parameter:
 * 	Command to send.
 */
void spiComs(AD5592_WORD command)
{
	makeWord(spiOut, command);
	clearBuffer(spiIn);
	bcm2835_spi_transfernb(spiOut, spiIn, sizeof(spiOut));
}

/**
 * Set a pin to high or low output.
 * Paramters:
 * 	All pins with a digital output as bit mask
 * 	State to be output to all diguital out pins as bit mask
 */
void setDigitalOut(uint8_t pins, uint8_t states)
{
	if(!(pins == digitalOutPins))
	{
		setAsDigitalOut(pins | digitalOutPins);
	}
	spiComs(AD5592_GPIO_WRITE_DATA | states);
}

/**
 * Get the digital input states
 * Parameter:
 * 	All pins to be configured as digital inputs as bit mask
 * Returns:
 * 	Pin states as bit mask. Unused pins are 0s.
 */
uint8_t getDigitalIn(uint8_t pins)
{
	if(!(pins == digitalInPins))
	{
		setAsDigitalIn(pins | digitalInPins);
	}
	spiComs(AD5592_GPIO_READ_INPUT | pins);
	spiComs(AD5592_NOP);

	return spiIn[1];
}

/**
 * Set an analog output value
 * Parameters:
 * 	Pin number to write to as number (0 to 7)
 *  Value to write in milivolts (assumes 5V reference)
 */
void setAnalogOut(uint8_t pin, uint16_t milivolts)
{
	if(!((analogOutPins >> pin ) & 0x1))
	{
		setAsDAC(analogOutPins | (0x1 << pin));
	}
	spiComs(AD5592_DAC_WRITE_MASK | 		/* DAC write command */
	((pin <<12) & AD5592_DAC_ADDRESS_MASK)|	/* Set which pin to write */
	a2d(milivolts));						/* Load digital value */
}

/**
 * Get analog input value
 * Parameters:
 * 	Pin number to to get value for as number (0 to 7)
 * Returns:
 * 	milivolts (assumes 5V reference)
 */
uint16_t getAnalogIn(uint8_t pin)
{
	if(!((analogInPins >> pin ) & 0x1))
	{
		setAsADC(analogInPins | (0x1 << pin));
	}
	spiComs(AD5592_ADC_READ | (0x1 << pin));
	spiComs(AD5592_NOP);
	spiComs(AD5592_NOP);
	
	uint16_t result = ((spiIn[0] << 8) & 0x0F00) | (spiIn[1] & 0xFF);
	/* Return result */
	return d2a(result);
}

/**
 * Initialize the SPI for using the AD5592. Does not set channel. Do that
 * after calling this function by calling setAD5592Ch().
 * Parameters:
 * 	none
 */
void AD5592_Init()
{
	//int msg;
	/* Initialize the bcm2835 library */
	bcm2835_init();

    /* Initialize the SPI module */
    bcm2835_spi_begin();

    /* Set SPI bit order */
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default

    /* Set SPI polarity and phase */
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                   // Mode 1

    /* Set SPI clock */
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16); 	  // 16 = 64ns = 15.625MHz
}
