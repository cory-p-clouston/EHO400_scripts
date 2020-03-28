// Cory Paquet-Clouston
// This script is a mix of 1 SHT30 sensor and 1 T6713 co2 sensor

// IMPORTANT: We cannot have twice the same sensor because their
// I2C device IP address is fixed! 

// Based on the example script from CO2 company

#include <Wire.h> // Although Wire.h is part of the Ardunio GUI
//library, this statement is still necessary

#include "SHTSensor.h"
SHTSensor sht;

#define ADDR_6713 0x15 // default I2C slave address
int data [4];
int CO2ppmValue;
// This is the default address of the CO2 sensor, 7bits shifted left.

void setup() {
  Wire.begin ();
  Serial.begin(9600);
  Serial.println("Application Note AN161_ardunio_T6713_I2C");

  delay(1000); // let serial console settle

  if (sht.init()) {
      Serial.print("init(): success\n");
  } else {
      Serial.print("init(): failed\n");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
}
//////////////////////////////////////////////////////////////////

// Function : int readCO2()
// Returns : CO2 Value upon success, 0 upon checksum failure
// Assumes : - Wire library has been imported successfully.
// - LED is connected to IO pin 13
// - CO2 sensor address is defined in co2_addr
//////////////////////////////////////////////////////////////////

int readC02()
{
  // start I2C
  Wire.beginTransmission(ADDR_6713);
  Wire.write(0x04); Wire.write(0x13); Wire.write(0x8B);
  Wire.write(0x00); Wire.write(0x01);
  // end transmission
  Wire.endTransmission();
  // read report of current gas measurement in ppm
  delay(2000);
  Wire.requestFrom(ADDR_6713, 4); // request 4 bytes from slave device
  data[0] = Wire.read();
  data[1] = Wire.read();
  data[2] = Wire.read();
  data[3] = Wire.read();
  Serial.print("Func code: "); Serial.print(data[0],HEX);
  Serial.print(" byte count: "); Serial.println(data[1],HEX);
  Serial.print("MSB: 0x"); Serial.print(data[2],HEX);
  Serial.print(" ");
  Serial.print("LSB: 0x"); Serial.print(data[3],HEX);
  Serial.print(" ");
  CO2ppmValue = ((data[2] * 0xFF ) + data[3]);
}

void loop() {
  int co2Value =readC02();
  {
    Serial.print("CO2 Value: ");
    Serial.println(CO2ppmValue);
  }

  if (sht.readSample()) {
      Serial.print("SHT:\n");
      Serial.print("  RH: ");
      Serial.print(sht.getHumidity(), 2);
      Serial.print("\n");
      Serial.print("  T:  ");
      Serial.print(sht.getTemperature(), 2);
      Serial.print("\n");
  } else {
      Serial.print("Error in readSample()\n");
  }
  delay(2000);
}
