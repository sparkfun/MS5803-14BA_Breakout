
/******************************************************************************
t5403_demo.ino
Demo Program for MS5803 pressure sensors.
Casey Kuhns @ SparkFun Electronics
6/26/2014



Resources:
This library uses the Arduino Wire.h to complete I2C transactions.

Development environment specifics:
	IDE: Arduino 1.0.5
	Hardware Platform: Arduino Pro 3.3V/8MHz
	T5403 Breakout Version: 1.0

This code is beerware. If you see me (or any other SparkFun employee) at the
local pub, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include <Wire.h>
#include <MS5803_I2C.h>

MS5803 sensor(ADDRESS_HIGH);

//Create variables to store results
float temperature_c, temperature_f;
double pressure_abs, pressure_relative, altitude_delta, pressure_baseline;

// Create Variable to store altitude in (m) for calculations;
double base_altitude = 1655.0; // Altitude of SparkFun's HQ in Boulder, CO. in (m)

void setup() {
    Serial.begin(9600);
    //Retrieve calibration constants for conversion math.
    sensor.reset();
    sensor.begin();
}

void loop() {
  
  // Read temperature from the sensor in deg C. This operation takes about 
  // 4.5ms to complete.
  temperature_c = sensor.getTemperature(CELSIUS, ADC_512);
  pressure_abs = sensor.getPressure(ADC_512);
  
  // Report values via UART
  Serial.print("Temperature C = ");
  Serial.println((uint32_t)temperature_c);

  Serial.print("Pressure C = ");
  Serial.println( pressure_abs);
    


  delay(1000);

  }