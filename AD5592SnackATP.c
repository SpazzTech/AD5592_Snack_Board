/**
 * File: AD5592SnackATP.c
 * Target: Raspberry Pi
 * Function: Acceptance Test Procedure for AD5592 Snack board
 * Dependancies: 
 * 		-bcm2835.h v1.20 2015/03/31 04:55:41
 * 		-AD5592.h v1.0.2 25 November 2016
 * Author: Tom Olenik
 * Original Date: 03 December 2016
 * Last Revised Date: 03 December 2016
 * Release Notes:
 * 	* Version: 1.0.0:
 * 		-This test requires two AD5592 Snack boards to be connected. The
 * 		unit under test should be connected to CS1 and the device
 * 		performing the test should be connected to CS0
 * 
 * 		-This test will create a text file named with the current time 
 * 		stamp in UTC time. The results of the test will be recorded in 
 * 		this txt file. It will also print to the terminal what is being 
 * 		logged in the test file so that results can be viewed during the
 * 		test.
 * 
 * 		-The test will verify that each pin of the AD5592 snack board 
 * 		functions properly as:
 * 			-Digital output
 * 			-Digital input
 * 			-Analog output
 * 			-Analog input
 * 		It tests digital IO at both the low and high states. It tests
 * 		the analog IO to be accurate to within 1% of the full range
 * 		(41 counts) at three voltage levels on each pin:
 * 			- 0.5V
 * 			- 2.5V
 * 			- 4.5V
 * 		
 **********************************************************************/
#include <time.h>
#include <bcm2835.h>
#include <stdio.h>
#include "AD5592.h"

#define	SHORT_DELAY	10		/* Delay used to let device do it's thing */
#define LONG_DELAY	50		/* Longer delay to give it more time */
#define	TOLERANCE	41		/* The digital +- tolerance for analog IO test */
#define TEST_DEVICE   BCM2835_SPI_CS0
#define	UNIT_UNDER_TEST BCM2835_SPI_CS1

char spiOut[2]; 			/* SPI output buffer */
char spiIn[2];	 			/* SPI input buffer  */

FILE *filePointer;			/* pointer to file object */
uint16_t mV;				/* millivolts */
uint16_t result;			/* result result */

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
	//printf("\n Command Word = %x %x", eightBits[0],eightBits[1]);
}

/**
 * Set the CS0 line for test device
 * Parameters:
 * 	none
 */
void testDevice()
{
	bcm2835_spi_chipSelect(TEST_DEVICE);        
    bcm2835_spi_setChipSelectPolarity(TEST_DEVICE, LOW);   
}

/**
 * Set the CS1 line for unit under test
 * Parameters:
 * 	none
 */
void uut()
{
	bcm2835_spi_chipSelect(UNIT_UNDER_TEST);        
    bcm2835_spi_setChipSelectPolarity(UNIT_UNDER_TEST, LOW);   
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
void setAsDigitalOut(AD5592_WORD pins)
{
	makeWord(spiOut, AD5592_GPIO_WRITE_CONFIG | pins);
	bcm2835_spi_transfern(spiOut, sizeof(spiOut));
}

/**
 * Set pins to a digital inputs.
 * Parameter: Pins as bit mask
 */
 void setAsDigitalIn(AD5592_WORD pins)
{
	makeWord(spiOut, AD5592_GPIO_READ_CONFIG | pins);
	bcm2835_spi_transfern(spiOut, sizeof(spiOut));
}

/**
 * Set pins to analog outputs.
 * Parameter: Pins as bit mask
 */
void setAsDAC(AD5592_WORD pins)
{
	makeWord(spiOut, AD5592_DAC_PIN_SELECT | pins);
	bcm2835_spi_transfern(spiOut, sizeof(spiOut));
}

/**
 * Set pins to a analog inputs.
 * Parameter: Pins as bit mask
 */
 void setAsADC(AD5592_WORD pins)
{
	makeWord(spiOut, AD5592_ADC_PIN_SELECT | pins);
	bcm2835_spi_transfern(spiOut, sizeof(spiOut));
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
 * Test digital function
 */
void digitalIOTest()
{
	printf("\n\nStarting digital io test\n\n");
	fprintf(filePointer, "\n\nStarting digital io test\n\n");
	
	/* Set test device to digital input */
	testDevice();   
	setAsDigitalIn(AD5592_PIN_SELECT_MASK);
	
	/* Set unit under test to digital output */
	uut();  
    setAsDigitalOut(AD5592_PIN_SELECT_MASK);
    
    /* Write digital output high to all pins of unit under test */
	spiComs(AD5592_GPIO_WRITE_DATA | AD5592_PIN_SELECT_MASK);
	
	/* Read digital values on test device */
	testDevice(); 	
	spiComs(AD5592_GPIO_READ_INPUT | AD5592_PIN_SELECT_MASK);
	spiComs(AD5592_NOP);
	
	/* Check if values are correct */
	result = spiIn[1];
	if(result == 0x00FF)
	{
		printf("\nDigital output high test: PASS ... Value = %x", result);
		fprintf(filePointer, "\nDigital output high test: PASS ... Value = %x", result);
	}else{
		printf("\nDigital output high test: FAIL ... Value = %x", result);
		fprintf(filePointer, "\nDigital output high test: FAIL ... Value = %x", result);
	}
	
	/* Set unit under test to output low values */
	uut();
	
	//printf("/n The SPI command should be: %x", AD5592_GPIO_WRITE_DATA);
	spiComs(AD5592_GPIO_WRITE_DATA); 	
	
	/* Read digital values on test device */
	testDevice(); 
	spiComs(AD5592_GPIO_READ_INPUT | AD5592_PIN_SELECT_MASK);
	spiComs(AD5592_NOP);
	
	/* Check if values are correct */
	result = spiIn[1];
	if(result == 0x000)
	{
		printf("\nDigital output low test: PASS ... Value = %x", result);
		fprintf(filePointer, "\nDigital output low test: PASS ... Value = %x", result);
	}else{
		printf("\nDigital output low test: FAIL ... Value = %x", result);
		fprintf(filePointer, "\nDigital output low test: FAIL ... Value = %x", result);
	}
	
	/* Switch unit under test to input */
	uut(); 
	 
	spiComs(AD5592_SW_RESET);
	delay(SHORT_DELAY);
	
    setAsDigitalIn(AD5592_PIN_SELECT_MASK);
    
    /* Switch test device to output and all pins high */
    testDevice();
     
    spiComs(AD5592_SW_RESET);
    delay(SHORT_DELAY);
       
	setAsDigitalOut(AD5592_PIN_SELECT_MASK);
	spiComs(AD5592_GPIO_WRITE_DATA | AD5592_PIN_SELECT_MASK);
		
	/* Read digital inputs of unit under test */
	uut(); 
	spiComs(AD5592_GPIO_READ_INPUT | AD5592_PIN_SELECT_MASK);
	spiComs(AD5592_NOP);
    
  	/* Check if values are correct */
	result = spiIn[1];
	if(result == 0xFF)
	{
		printf("\nDigital input high test: PASS ... Value = %x", result);
		fprintf(filePointer, "\nDigital input high test: PASS ... Value = %x", result);
	}else{
		printf("\nDigital input high test: FAIL ... Value = %x", result);
		fprintf(filePointer, "\nDigital input high test: FAIL ... Value = %x", result);
	}
	
    /* Set test device output on all pins low */
    testDevice();
    spiComs(AD5592_GPIO_WRITE_DATA);	
	
	/* Read digital inputs of unit under test */
	uut();
	spiComs(AD5592_GPIO_READ_INPUT | AD5592_PIN_SELECT_MASK);
    spiComs(AD5592_NOP);
    
  	/* Check if values are correct */
	result = spiIn[1];
	if(result == 0x00)
	{
		printf("\nDigital input low test: PASS ... Value = %x", result);
		fprintf(filePointer, "\nDigital input low test: PASS ... Value = %x", result);
	}else{
		printf("\nDigital input low test: FAIL ... Value = %x", result);
		fprintf(filePointer, "\nDigital input low test: FAIL ... Value = %x", result);
	}
	
		printf("\n\nDigital io test complete\n\n");
	fprintf(filePointer, "\n\nDigital io test complete\n\n");	  
}

/**
 * Test analog function
 */
void analogIOTest()
{
	printf("\n\nStarting analog io test\n\n");
	fprintf(filePointer, "\n\nStarting analog io test\n\n");	
		
	uint16_t lowVoltage = 500;		/* 500 millivolts = 0.5 V */
	uint16_t midVoltage	 = 2500;	/* 2500 millivolts = 2.5 V */
	uint16_t highVoltage = 4500;	/* 4500 millivolts = 4.5 V */
	
	/* Convert voltage values to digital values */
	uint16_t lowCount = a2d(lowVoltage);
	uint16_t midCount = a2d(midVoltage);
	uint16_t highCount = a2d(highVoltage);
	
	uint16_t targetVal[] = {lowCount,midCount,highCount};
	
	int i;
	int j;
	
/* Test analog out */
	
	for(j = 0; j < 3;j++)
	{
		delay(LONG_DELAY);
		for(i = 0; i<8 ; i++)
		{
			uut(); /* start out addressing unit under test */
			spiComs(AD5592_SW_RESET);
			delay(SHORT_DELAY);
			spiComs(AD5592_DAC_PIN_SELECT | (0x1<<i));
			spiComs(AD5592_DAC_WRITE_MASK | 			/* DAC write command */
				((i <<12) & AD5592_DAC_ADDRESS_MASK)|	/* Set which pin to write */
				targetVal[j]);							/* Load digital value */
						
			/* Check the value */
			testDevice();
			spiComs(AD5592_SW_RESET);
			delay(SHORT_DELAY);
			spiComs(AD5592_ADC_PIN_SELECT | (0x1<<i));
			delay(SHORT_DELAY);
			spiComs(AD5592_ADC_READ | (0x1 << i));
			spiComs(AD5592_NOP);
			spiComs(AD5592_NOP);
			
			/* Get result */
			result = ((spiIn[0] << 8) & 0x0F00) | (spiIn[1] & 0xFF);
			
			/* Report and record result */
			printf("\nDAC test on IO%d target = %d", i, targetVal[j]);
			fprintf(filePointer, "\nDAC test IO%d target = %d", i, targetVal[j]);
			printf("...Result = %d", result);
			fprintf(filePointer, "...Result = %d", result);
			
			/* Test, report, and record pass/fail */
			if((targetVal[j] + TOLERANCE) > result && (targetVal[j] - TOLERANCE) < result)
			{
				/* PASS */
				printf("...PASS");
				fprintf(filePointer, "...PASS");
			}else
			{
				/* FAIL */
				printf("...FAIL");
				fprintf(filePointer, "...FAIL");
			}
		}
	}
	
/* Test analog in */

	bcm2835_delay(LONG_DELAY);

	for(j = 0; j < 3; j++)
	{
		delay(LONG_DELAY);
		for(i = 0; i<8 ; i++)
		{
			
			testDevice(); /* Start out addressing test device */
			spiComs(AD5592_SW_RESET);
			delay(SHORT_DELAY);
			spiComs(AD5592_DAC_PIN_SELECT | (0x1<<i));
			spiComs(AD5592_DAC_WRITE_MASK | 			/* DAC write command */
				((i <<12) & AD5592_DAC_ADDRESS_MASK)|	/* Set which pin to write */
				targetVal[j]);							/* Load digital value */
			
			/* Check the value */
			uut();
			spiComs(AD5592_SW_RESET);
			delay(SHORT_DELAY);
			spiComs(AD5592_ADC_PIN_SELECT | (0x1<<i));
			delay(SHORT_DELAY);
			spiComs(AD5592_ADC_READ | (0x1 << i));
			spiComs(AD5592_NOP);
			spiComs(AD5592_NOP);
			
			/* Get result */
			result = ((spiIn[0] << 8) & 0x0F00) | (spiIn[1] & 0xFF);
			
			/* Report and record result */
			printf("\nADC test on IO%d target = %d", i, targetVal[j]);
			fprintf(filePointer, "\nADC test IO%d target = %d", i, targetVal[j]);
			printf("...Result = %d", result);
			fprintf(filePointer, "...Result = %d", result);
			
			/* Test, report, and record pass/fail */
			if((targetVal[j] + TOLERANCE) > result && (targetVal[j] - TOLERANCE) < result)
			{
				/* PASS */
				printf("...PASS");
				fprintf(filePointer, "...PASS");
			}else
			{
				/* FAIL */
				printf("...FAIL");
				fprintf(filePointer, "...FAIL");
			}
			
		}
	}	

	printf("\n\nAnalog io test complete\n\n");
	fprintf(filePointer, "\n\nAnalog io test complete\n\n");
}

int main(int argc, char **argv)
{
	/* Initialize the bcm2835 library */
	if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }
    
    /* Initialize the SPI module */
    if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failedg. Are you running as root??\n");
      return 1;
    }
    /* Set SPI bit order */
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    
    /* Set SPI polarity and phase */
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                   // Mode 1
    
    /* Set SPI clock */
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16); 	  // 16 = 64ns = 15.625MHz
    
    /* Get a time stamp */
    time_t timeStamp;
	time(&timeStamp);
    filePointer = fopen(ctime(&timeStamp),"w");	/* Create a file for the acceptance test data log */
	
	/* Report and record start of test time */
	printf("Test start time: %s \n", ctime(&timeStamp));
    fprintf(filePointer, "Test start time: %s \n", ctime(&timeStamp));
    
	/* Perform tests */
	digitalIOTest();
	analogIOTest();
	
	uut();
	spiComs(AD5592_SW_RESET);
	testDevice();
	spiComs(AD5592_SW_RESET);
    
    /* Get new time stamp */
    time(&timeStamp);
    
    /* Report and record test finish time */
    printf("\n\nTest finish time: %s", ctime(&timeStamp));
    fprintf(filePointer, "\n\nTest finish time: %s", ctime(&timeStamp));
    
    /* Close the test log file */
    fclose(filePointer);
	return 0;
}

