/******************************************************************************
MS5XXX.cpp
Library for MS5XXX pressure sensors.
Casey Kuhns @ SparkFun Electronics
6/26/2014
https://github.com/sparkfun/MS5803-14BA_Breakout

The MS58XX MS57XX and MS56XX by Measurement Specialties is a low cost I2C barometric pressure
sensor.  This sensor can be used in weather stations and for altitude
estimations. In this file are the functions in the MS5XXX class

Resources:
This library uses the Arduino Wire.h to complete I2C transactions.
This library uses the Arduino SPI.h to complete SPI transactions.

Development environment specifics:
	IDE: Arduino 1.0.5
	Hardware Platform: Arduino Pro 3.3V/8MHz
	MS5803 Breakout Version: 1.0


This code is beerware. If you see me (or any other SparkFun employee) at the
local pub, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include "MS5XXX.h"
#include <Wire.h> // Wire library is used for I2C
#include <SPI.h> // SPI library is used for SPI.

MS5XXX::MS5XXX_SPI(uint8t cs_pin)
// Base library type SPI
{
	_protocol = protocol_SPI;
	SPI.begin(); // Arduino SPI library initializer
	_cs_pin = cs_pin; //set interface used for communication
	pinMode(_cs_pin, OUTPUT);
	DigitalWrite(_cs_pin, HIGH);
}

MS5XXX::MS5XXX_I2C(ms5XXX_addr address)
// Base library type I2C
{
	_protocol = protocol_I2C
	Wire.begin(); // Arduino Wire library initializer
	_address = address; //set interface used for communication
}




void MS5XXX::reset_SPI(void)
// Reset device SPI
{
	DigitalWrite(_cs_pin, LOW);
	SPI.transfer(CMD_RESET);
	sensorWait(3);
	DigitalWrite(_cs_pin, HIGH);
}


uint8_t MS5XXX::begin_SPI(void)
// Initialize library for subsequent pressure measurements
{  
	uint8_t i, byteLow, byteHigh; 
	
	for(i = 0; i <= 8; i++){
		DigitalWrite(_cs_pin, LOW);
		SPI.transfer(CMD_PROM+(i*2));
		byteLow = SPI.transfer(0x00);
		byteHigh = SPI.transter(0x00);
		coefficients[i] = (byteHigh << 8)|(byteLow);
	}
	
	/*uint16_t n_rem = 0x00; // crc remainder 
	uint8_t n_bit; 
	n_prom[7]=(0xFF00 & (n_prom[7])); //CRC byte is replaced by 0 
	for (i = 0; i < 16; i++) // operation is performed on bytes 
	{ // choose LSB or MSB 
		if (i%2==1) 
			n_rem ^= (int16_t) ((n_prom[i>>1]) & 0x00FF); 
		else n_rem ^= (int16_t) (n_prom[i>>1]>>8); 
			for (n_bit = 8; n_bit > 0; n_bit--) 
			{ 
				if (n_rem & (0x8000)) 
				{ 
					n_rem = (n_rem << 1) ^ 0x3000; 
				} 
				else 
				{ 
					n_rem = (n_rem << 1); 
				} 
			} 
	}	 
	n_rem= (0x000F & (n_rem >> 12)); // // final 4-bit reminder is CRC code 
	uint8_t crc_error; // store crc success or failure.
	*/
	return 0;
}
	
double MS5XXX::getTemperature_SPI(temperature_units units, precision _precision))
// Return a temperature reading in either F or C.
{
	// Create variables for conversion and raw data. 
	uint32_t temperature_raw; 
	double temperature_actual; 
	double t; // Working variable for calculations
	
	// Start temperature measurement
	digitalWrite(_cs_pin, LOW); // Set chip select (low)
	// Start ADC conversion
	SPI.transfer(CMD_ADC_CONV + CMD_ADC_D2 + _precision);
	// Wait for conversion to complete
	switch(precision)
	{ 
		case ADC_256 : sensorWait(1); break; 
		case ADC_512 : sensorWait(3); break; 
		case ADC_1024: sensorWait(4); break; 
		case ADC_2048: sensorWait(6); break; 
		case ADC_4096: sensorWait(10); break; 
	} 
	digitalWrite(_cs_pin, HIGH);  // Restore Chip select
	
	//Retrieve ADC result
	// Temporary storage of data
	uint8_t highByte, middleByte, LowByte; 

	digitalWrite(_cs_pin, LOW); // Set chip select (low)
	SPI.transfer(CMD_ADC_READ); // Send ADC read command.
	//Shift in 3 bytes of value
	highByte = SPT.transfer(0x00);
	middleByte = SPT.transfer(0x00);
	lowByte = SPT.transfer(0x00);
	digitalWrite(_cs_pin, HIGH);  // Restore Chip select

	//Combine bytes into raw value
	temperature_raw = (highByte << 16)|(middleByte << 8)|LowByte;  
	
	//Now that we have a raw temperature, let's compute our actual.
	t = temperature_raw - coefficient[5] << 8;
	temperature_actual = (2000 + (dT * coefficient[6]) >> 23)/100;

	// If Fahrenheit is selected return the temperature converted to F
	if(units == FAHRENHEIT){
		temperature_actual = ((temperature_actual * 9) / 5) + 32;
		return temperature_actual;
		}
		
	// If Celsius is selected return the temperature converted to C	
	else if(units == CELSIUS){
		return temperature_actual;
	}
}


double MS5XXX::getPressure_SPI(uint8_t commanded_precision)
// Return a pressure reading.
{
	return 0;
}

void MS5XXX::sensorWait(uint8_t time)
// Delay function.  This can be modified to work outside of Arduino based MCU's
{
	delay(time);
};
