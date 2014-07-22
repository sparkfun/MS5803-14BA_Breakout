/******************************************************************************
MS5803_I2C.h
Library for MS5803 pressure sensors.
Casey Kuhns @ SparkFun Electronics
6/26/2014
https://github.com/sparkfun/MS5803-14BA_Breakout

The MS58XX MS57XX and MS56XX by Measurement Specialties is a low cost I2C barometric pressure
sensor.  This sensor can be used in weather stations and for altitude
estimations. 

Resources:
This library uses the Arduino Wire.h to complete I2C transactions.

Development environment specifics:
	IDE: Arduino 1.0.5
	Hardware Platform: Arduino Pro 3.3V/8MHz
	MS5803 Breakout Version: 1.0


This code is beerware. If you see me (or any other SparkFun employee) at the
local pub, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef MS5803_I2C_h
#define MS5803_I2C_h

#include "Arduino.h"

// Define units for conversions. 
enum temperature_units
{
	CELSIUS,
	FAHRENHEIT,
};

// Define constants for Conversion precision
enum precision
{
	ADC_256  = 0x00,
	ADC_512  = 0x02,
	ADC_1024 = 0x04,
	ADC_2048 = 0x06,
	ADC_4096 = 0x08
};

// Define address choices for the device (I2C mode)
enum ms5803_addr
{
	ADDRESS_HIGH = 0x76,
	ADDRESS_LOW	= 0x77
};

// Register addresses for operations
#define MS5803_COMMAND_REG	0xF1
#define MS5803_DATA_REG		0xF5


//Commands
#define CMD_RESET 0x1E // ADC reset command 
#define CMD_PROM 0xA0 // Coefficient location
#define CMD_ADC_READ 0x00 // ADC read command 

#define CMD_ADC_CONV 0x40 // ADC conversion command 
#define CMD_ADC_D1 0x00 // ADC D1 conversion - Presure
#define CMD_ADC_D2 0x10 // ADC D2 conversion - Temperature

class MS5803
{
	public:	
		MS5803(ms5803_addr address); 
		//Reset device
		void reset(void);
		// Collect constants from sensor for calculations
		uint8_t begin(void);
		// Return calculated temperature from sensor
		float getTemperature(temperature_units units, precision _precision);
		// Return calculated pressure from sensor
		float getPressure(precision _precision);

	private:

		// Variable used to store I2C device address.
		int8_t _address; 

		// Coefficients filled in with MS5803();
		uint16_t coefficient[8];
		
		// General I2C send command function
		void sendCommand(uint8_t command);
		// Retrieve ADC result
		uint32_t getADCconversion(void);
		// General delay function.  If delay() is not supported on other 
		// platforms it may be necessary to modify this function.
		// The units for sensorWait() are in ms. 
		void sensorWait(uint8_t time);
};

#endif