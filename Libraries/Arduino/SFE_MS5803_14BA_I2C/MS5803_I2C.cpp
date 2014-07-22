/******************************************************************************
MS5803_I2C.cpp
Library for MS5803 pressure sensor.
Casey Kuhns @ SparkFun Electronics
6/26/2014
https://github.com/sparkfun/MS5803-14BA_Breakout

The MS58XX MS57XX and MS56XX by Measurement Specialties is a low cost I2C barometric pressure
sensor.  This sensor can be used in weather stations and for altitude
estimations. In this file are the functions in the MS5803 class

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

#include <Wire.h> // Wire library is used for I2C
#include "MS5803_I2C.h"

MS5803::MS5803(ms5803_addr address)
// Base library type I2C
{
	Wire.begin(); // Arduino Wire library initializer
	_address = address; //set interface used for communication
}

void MS5803::reset(void)
// Reset device I2C
{
   sendCommand(CMD_RESET);
   sensorWait(3);
}


uint8_t MS5803::begin(void)
// Initialize library for subsequent pressure measurements
{  
	uint8_t i;
	for(i = 0; i <= 7; i++){
		sendCommand(CMD_PROM + (i * 2));
		Wire.requestFrom( _address, 2);
		uint8_t highByte = Wire.read(); 
		uint8_t lowByte = Wire.read();
		coefficient[i] = (highByte << 8)|lowByte;
		Serial.print("C");
		Serial.print(i);
		Serial.print("= ");
		Serial.println(coefficient[i]);
	}

	return 0;
}
	
float MS5803::getTemperature(temperature_units units, precision _precision)
// Return a temperature reading in either F or C.
{
	// Create variables for conversion and raw data. 
	uint32_t temperature_raw; 
	float temperature_actual; 
	float t; // Working variable for calculations
	
	// send command to start temperature measurement
	sendCommand(CMD_ADC_CONV + CMD_ADC_D2 + _precision);
	
	// Wait for conversion to complete
	switch( _precision )
	{ 
		case ADC_256 : sensorWait(1); break; 
		case ADC_512 : sensorWait(3); break; 
		case ADC_1024: sensorWait(4); break; 
		case ADC_2048: sensorWait(6); break; 
		case ADC_4096: sensorWait(10); break; 
	} 
	
	//Retrieve ADC result
	temperature_raw = getADCconversion();
		
	//Now that we have a raw temperature, let's compute our actual.
	t = temperature_raw - ((uint32_t)coefficient[5] << 8);
	temperature_actual = 2000 + (t * coefficient[6]) / pow(2,23);
	temperature_actual = temperature_actual / 100;

	// If Fahrenheit is selected return the temperature converted to F
	if(units == FAHRENHEIT){
		temperature_actual = (((temperature_actual) * 9) / 5) + 32;
		return temperature_actual;
		}
		
	// If Celsius is selected return the temperature converted to C	
	else if(units == CELSIUS){
		return temperature_actual;
	}
}

float MS5803::getPressure(precision _precision)
// Return a pressure reading.
{
	return 0;
}

void MS5803::sendCommand(uint8_t command)
{	
	Wire.beginTransmission( _address);
	Wire.write(command);
	Wire.endTransmission();
	
}

uint32_t MS5803::getADCconversion(void)
{	
	uint32_t result;
	sendCommand(CMD_ADC_READ);
	Wire.requestFrom(_address, 3);
	uint8_t highByte, midByte, lowByte;
	
	while(Wire.available())    
	{ 
		highByte = Wire.read();
		midByte = Wire.read();
		lowByte = Wire.read();	
	}
	
	result = ((uint32_t)highByte << 16) + ((uint32_t)midByte << 8) + lowByte;

Serial.println(highByte);
Serial.println(midByte);
Serial.println(lowByte);


	
	Serial.println(result);
	
	return result;
}

void MS5803::sensorWait(uint8_t time)
// Delay function.  This can be modified to work outside of Arduino based MCU's
{
	delay(time);
};


