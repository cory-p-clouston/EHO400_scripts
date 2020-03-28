// CO2 Meter Amphenol T6713 i2c Example Interface
// By Marv Kausch, 12/2016 at CO2 Meter <co2meter.com>
// Revised by John Houck, 05/01/2018
// Talks via I2C to T6713 sensors and displays CO2 values
// We will be using the I2C hardware interface on the Arduino in
// combination with the built-in Wire library to interface.
// Arduino analog input 5 - I2C SCL
// Arduino analog input 4 - I2C SDA

/*
In this example we will do a basic read of the CO2 value and
checksum verification.
For more advanced applications please see the I2C Comm guide.
*/
#include <Wire.h> // Although Wire.h is part of the Ardunio GUI
//library, this statement is still necessary

#define ADDR_6713 0x15 // default I2C slave address
int data [4];
int CO2ppmValue;
// This is the default address of the CO2 sensor, 7bits shifted left.

void setup() {
  Wire.begin ();
  Serial.begin(115200);
  Serial.println("Application Note AN161_ardunio_T6713_I2C");
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
  delay(2000);
}
