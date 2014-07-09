/******************************************************************************
MS5XXX.h
Library for MS5XXX pressure sensors.
Casey Kuhns @ SparkFun Electronics
6/26/2014
https://github.com/sparkfun/MS5803-14BA_Breakout

The MS58XX MS57XX and MS56XX by Measurement Specialties is a low cost I2C barometric pressure
sensor.  This sensor can be used in weather stations and for altitude
estimations. In this file are the functions in the MS5XXX class

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

#include "MS5XXX.h"
#include <Wire.h> // Wire library is used for I2C
#include <SPI.h> // SPI library is used for SPI.

MS5XXX::MS5XXX(interface_mode interface)
// Base library type
{
	_interface = interface; //set interface used for communication
}

void MS5XXX::begin(void)
// Initialize library for subsequent pressure measurements
{  
	// Set up the communication port
	communicationBegin();

	getData(MS5XXX_C1, &c1);  //Retrieve C1 from device
	getData(MS5XXX_C2, &c2);  //Retrieve C2 from device
	getData(MS5XXX_C3, &c3);  //Retrieve C3 from device
	getData(MS5XXX_C4, &c4);  //Retrieve C4 from device
	getData(MS5XXX_C5, &c5);  //Retrieve C5 from device
	getData(MS5XXX_C6, &c6);  //Retrieve C6 from device
	getData(MS5XXX_C7, &c7);  //Retrieve C7 from device
	getData(MS5XXX_C8, &c8);  //Retrieve C8 from device
}
	
int16_t MS5XXX::getTemperature(temperature_units units, precision _precision))
// Return a temperature reading.
{
	// Create variables for conversion and raw data. 
	uint16_t temperature_raw; 
	double temperature_actual; 
	
	// Start temperature measurement
	sendCommand(CMD_ADC_CONV + CMD_ADC_D2 + precision); 
	// Wait for conversion to complete
	switch(precision)
	{ 
		case ADC_256 : sensorWait(1); break; 
		case ADC_512 : sensorWait(3); break; 
		case ADC_1024: sensorWait(4); break; 
		case ADC_2048: sensorWait(6); break; 
		case ADC_4096: sensorWait(10); break; 
	} 

	// Receive raw temp value from device.
	getData(CMD_ADC_READ, &temperature_raw);		
	// Perform calculation specified in data sheet
	temperature_actual = (((((int32_t) c1 * temperature_raw) >> 8) 
						 + ((int32_t) c2 << 6)) * 100) >> 16;


	// If Fahrenheit is selected return the temperature converted to F
	if(units == FAHRENHEIT){
		temperature_actual = ((temperature_actual * 9) / 5) + 3200;
		return temperature_actual;
		}
		
	// If Celsius is selected return the temperature converted to C	
	else if(units == CELSIUS){
		return (int16_t) temperature_actual;
	}
}

int32_t MS5XXX::getPressure(uint8_t commanded_precision)
// Return a pressure reading.
{
	// Create variables for conversion and raw data. 
	int16_t temperature_raw; 
	uint16_t pressure_raw;

	// Start temperature measurement
	sendCommand(MS5XXX_COMMAND_REG, COMMAND_GET_TEMP); 
	// Wait 5ms for conversion to complete
	sensorWait(5); 
	// Receive raw temp value from device.
	getData(MS5XXX_DATA_REG, &temperature_raw);		
	
	// Load measurement noise level into command along with start command bit.
	commanded_precision = (commanded_precision << 3)|(0x01); 
	// Start pressure measurement
	sendCommand(MS5XXX_COMMAND_REG, commanded_precision); 
	
	//Select delay time based on precision level selected.
	switch(commanded_precision){
		case MODE_LOW:
		{	
			sensorWait(5); //  Wait 5ms for conversion to complete
			break;
		}
		case MODE_STANDARD:
		{
			sensorWait(11); //  Wait 11 ms for conversion to complete
			break;
		}
		case MODE_HIGH:{
			sensorWait(19); //  Wait 19 ms for conversion to complete
			break;
		}
		case MODE_ULTRA:{
			sensorWait(67); //  Wait 67 ms for conversion to complete
			break;
		}
	};
	
	//  Receive raw pressure value from device.
	getData(MS5XXX_DATA_REG, (int16_t*)&pressure_raw);	
	
	// Create variables to hold calculated pressure and working variables for 
	// calculations.
	int32_t pressure_actual, s, o; 

	// Calculations come from application note. 
	s = (((((int32_t) c5 * temperature_raw)  >> 15) * temperature_raw) >> 19)
			+ c3 + (((int32_t) c4 * temperature_raw) >> 17); 
			
	o = (((((int32_t) c8 * temperature_raw) >> 15) * temperature_raw) >> 4) 
			+ (((int32_t) c7 * temperature_raw) >> 3) + ((int32_t)c6 * 0x4000);
			
	pressure_actual = (s * pressure_raw + o) >> 14;

	return pressure_actual;
}

void MS5XXX::sensorWait(uint8_t time)
// Delay function.  This can be modified to work outside of Arduino based MCU's
{
	delay(time);
};

void MS5XXX::communicationBegin()
// Initialize the communication protocol used.  SPI is currently unsupported in 
// the hardware.  If a release comes with hardware support this file will be 
// updated.  All reference to SPI is currently a place holder for future 
// development.
// This can be modified to work outside of Arduino based MCU's
{
	// If SPI is selected use SPI begin
	if( _interface == MODE_SPI){  
		SPI.begin();
	}
	// If i2c is selected for communication use i2c begin
	else{
		Wire.begin();
	}

}

int8_t MS5XXX::getData(uint8_t location, uint16_t* output)
// Receive data from the device.  
// This can be modified to work outside of Arduino based MCU's  
{
	uint8_t byteLow, byteHigh;
	uint16_t _output;
		
	if( _interface == MODE_SPI){  
		SPI.transfer(location);
		byteLow = SPI.transfer(0x00);
		byteHigh = SPI.transter(0x00);
	}
	else {  // If i2c is selected for communication use i2c commands
		Wire.beginTransmission(MS5XXX_I2C_ADDR); 
		Wire.write(location);
		Wire.endTransmission();    // Transmit data
		Wire.requestFrom(MS5XXX_I2C_ADDR,2);

		while(Wire.available()){
			byteLow = Wire.read(); // receive low byte 
			byteHigh = Wire.read(); // receive high byte
		}
	}
	
	_output = (byteHigh << 8)|(byteLow);
	*output = _output;
	
}

int8_t MS5XXX::sendCommand(uint8_t command)
// Send command to the device.  SPI is currently unsupported in the hardware.  
// If a release comes with hardware support this function will be updated.  
// All reference to SPI is currently a place holder for future development.
// This can be modified to work outside of Arduino based MCU's
{
	// If SPI is selected for communication use SPI commands
	if(_interface == MODE_SPI){
		SPI.transter(command);
	}
	// If i2c is selected for communication use i2c commands
	else{
		Wire.beginTransmission(MS5XXX_I2C_ADDR); 
		Wire.write(command);
		Wire.endTransmission();
	}
}