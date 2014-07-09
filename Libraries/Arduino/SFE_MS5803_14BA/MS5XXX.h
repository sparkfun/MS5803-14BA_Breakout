/******************************************************************************
MS5XXX.h
Library for MS5XXX pressure sensors.
Casey Kuhns @ SparkFun Electronics
6/26/2014
https://github.com/sparkfun/MS5803-14BA_Breakout

The MS58XX MS57XX and MS56XX by Measurement Specialties is a low cost I2C barometric pressure
sensor.  This sensor can be used in weather stations and for altitude
estimations. In this file are the prototype functions in the T5400 class

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

#ifndef MS5XXX_h
#define MS5XXX_h

#include "Arduino.h"

// The MS5XXX is designed to function over both I2C or SPI. This library supports
// both. The interface mode used must be sent to the MS5XXX constructor. Use
// one of these two as the first parameter of the constructor.
enum interface_mode
{
	MODE_SPI,
	MODE_I2C,
};

// Define units for conversions. 
enum temperature_units
{
	CELSIUS,
	FAHRENHEIT,
};

enum precision
{
	ADC_256  = 0x00,
	ADC_512  = 0x02,
	ADC_1024 = 0x04,
	ADC_2048 = 0x06,
	ADC_4096 = 0x08
};

// Device Address
#define MS5XXX_I2C_ADDR_HIGH	0x76
#define MS5XXX_I2C_ADDR_LOW		0x77
	
// Register addresses for operations
#define MS5XXX_COMMAND_REG	0xF1
#define MS5XXX_DATA_REG		0xF5

// Register addresses for calibration constants
#define MS5XXX_C1		0xA0
#define MS5XXX_C2		0xA2
#define MS5XXX_C3		0xA4
#define MS5XXX_C4		0xA6
#define MS5XXX_C5		0xA8
#define MS5XXX_C6		0xAA
#define MS5XXX_C7		0xAC
#define MS5XXX_C8		0xAE

//Commands
#define CMD_RESET 0x1E // ADC reset command 
#define CMD_ADC_READ 0x00 // ADC read command 
#define CMD_ADC_CONV 0x40 // ADC conversion command 
#define CMD_ADC_D1 0x00 // ADC D1 conversion - Presure
#define CMD_ADC_D2 0x10 // ADC D2 conversion - Temperature

class MS5XXX
{
	public:
		// Select the interface to be used in communication
		MS5XXX(interface_mode interface);
		// Collect constants from sensor for calculations
		void begin(void);
		// Return calculated temperature from sensor
		int16_t getTemperature(temperature_units units, precision _precision);
		// Return calculated pressure from sensor
		int32_t getPressure(precision _precision);

	private:
		// Variable used to store interface selected for communication.
		interface_mode _interface; 

		// Constants filled in with MS5XXX();
		uint16_t c1, c2, c3, c4, c5, c6, c7, c8;
		
		// General delay function.  If delay() is not supported on other 
		// platforms it may be necessary to modify this function.
		// The units for sensorWait() are in ms. 
		void sensorWait(uint8_t time);
		
		// Communication transport functions. These can be modified to allow the 
		// library to work with other platforms and architectures.
		void communicationBegin();
		int8_t getData(uint8_t location, uint16_t* output);
		int8_t sendCommand(uint8_t location, uint8_t command);

};

#endif