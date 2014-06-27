/******************************************************************************
t5403_demo.ino
Demo Program for T5403 barometric pressure sensor.
Casey Kuhns @ SparkFun Electronics
2/12/2014
https://github.com/sparkfun/T5403_Barometric_Breakout

The T5403 by Epcos is a low cost I2C barometric pressure sensor.  This sensor 
can be used in weather stations and for altitude estimations.  This file is a 
simple demonstration of the sensor and some basic functions.

Resources:
This library uses the Arduino Wire.h to complete I2C transactions.

Development environment specifics:
	IDE: Arduino 1.0.5
	Hardware Platform: Arduino Pro 3.3V/8MHz
	T5403 Breakout Version: 1.0

NOTE:  SPI is currently unsupported in the hardware.  If a release comes with
hardware support this file will be updated.  All reference to SPI is currently
a place holder for future development.

This code is beerware. If you see me (or any other SparkFun employee) at the
local pub, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include <Wire.h>
#include <t5403.h>

T5403 barometer(MODE_I2C);
//Create variables to store results
float temperature_c, temperature_f;
double pressure_abs, pressure_relative, altitude_delta, pressure_baseline;

// Create Variable to store altitude in (m) for calculations;
double base_altitude = 1655.0; // Altitude of SparkFun's HQ in Boulder, CO. in (m)

void setup() {
    Serial.begin(9600);
	//Retrieve calibration constants for conversion math.
    barometer.begin();
    // Grab a baseline pressure for delta altitude calculation.
    pressure_baseline = barometer.getPressure(MODE_ULTRA);
}

void loop() {
  
  // Read temperature from the sensor in deg C. This operation takes about 
  // 4.5ms to complete.
  temperature_c = barometer.getTemperature(CELSIUS);
  
  // Read temperature from the sensor in deg F. This operation takes about 
  // 4.5ms to complete. Converting to Fahrenheit is not internal to the sensor.
  // Additional math is done to convert a Celsius reading.
  temperature_f = barometer.getTemperature(FAHRENHEIT);
  
  // Read pressure from the sensor in Pa. This operation takes about 
  // 67ms to complete in ULTRA_MODE.  Other Modes are available for faster, yet
  // less precise measurements.
  // MODE_LOW      = 5 ms
  // MODE_STANDARD = 11 ms
  // MODE_HIGH     = 19 ms
  // MODE_ULTRA    = 67 ms
  
  pressure_abs  = barometer.getPressure(MODE_ULTRA);
  
  // Let's do something interesting with our data.
  
  // Convert abs pressure with the help of altitude into relative pressure
  // This is used in Weather stations.
  pressure_relative = sealevel(pressure_abs, base_altitude);
  
  // Taking our baseline pressure at the beginning we can find an approximate
  // change in altitude based on the differences in pressure.   
  altitude_delta = altitude(pressure_abs , pressure_baseline);
  
  // Report values via UART
  Serial.print("Temperature C = ");
  Serial.println(temperature_c / 100);
  
  Serial.print("Temperature F = ");
  Serial.println(temperature_f / 100);  

  Serial.print("Pressure abs (Pa)= ");
  Serial.println(pressure_abs);  
  
  Serial.print("Pressure relative (hPa)= ");
  Serial.println(pressure_relative); 
  
  Serial.print("Altitude change (m) = ");
  Serial.println(altitude_delta); 
  
  delay(1000);
}

// Thanks to Mike Grusin for letting me borrow the functions below from 
// the BMP180 example code. 

 double sealevel(double P, double A)
// Given a pressure P (Pa) taken at a specific altitude (meters),
// return the equivalent pressure (mb) at sea level.
// This produces pressure readings that can be used for weather measurements.
{
	return((P/100)/pow(1-(A/44330.0),5.255));
}


double altitude(double P, double P0)
// Given a pressure measurement P (Pa) and the pressure at a baseline P0 (Pa),
// return altitude (meters) above baseline.
{
	return(44330.0*(1-pow(P/P0,1/5.255)));
}