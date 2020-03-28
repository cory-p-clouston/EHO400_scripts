/**
 * TCA9548 I2CScanner.pde -- I2C bus scanner for Arduino
 *
 * Based on code c. 2009, Tod E. Kurt, http://todbot.com/blog/
 *
 * Cory:
 * Code is from https://learn.adafruit.com/adafruit-tca9548a-1-to-8-i2c-multiplexer-breakout?view=all
 * I commented out the lines to have measurements from the SHT30 
 * it is only a I2C scanner presently.
 */

#include "Wire.h"
//#include "SHTSensor.h"

extern "C" { 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCAADDR 0x70

// These are the numbers for the multiplexer
//#define SHT30_1_PIN 1
//#define SHT30_2_PIN 3

//SHTSensor sht1;
//SHTSensor sht2;

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}


// standard Arduino setup()
void setup()
{
    while (!Serial);
    delay(1000);

    Wire.begin();
    
    Serial.begin(115200);
    Serial.println("\nTCAScanner ready!");

    delay(1000);

    //tcaselect(SHT30_1_PIN);
    //if (sht1.init()) {
   //   Serial.print("init1(): success\n");
   // } else {
   //   Serial.print("init1(): failed\n");
   // }
//sht1.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x

    //tcaselect(SHT30_2_PIN);
    //if (sht2.init()) {
     // Serial.print("init2(): success\n");
    //} else {
    //  Serial.print("init2(): failed\n");
   // }
   // sht2.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
    
    for (uint8_t t=0; t<8; t++) {
      tcaselect(t);
      delay(100);
      Serial.print("TCA Port #"); Serial.println(t);

      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCAADDR) continue;
      
        uint8_t data;
        if (! twi_writeTo(addr, &data, 0, 1, 1)) {
           Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
        }
      }
    }
    Serial.println("\ndone");
}

void loop() 
{
  
  //tcaselect(SHT30_1_PIN);
  //if (sht1.readSample()) {
//      Serial.print("SHT1:\n");
//      Serial.print("  RH1: ");
//      Serial.print(sht1.getHumidity(), 2);
//      Serial.print("\n");
//      Serial.print("  T1:  ");
//      Serial.print(sht1.getTemperature(), 2);
//      Serial.print("\n");
//  } else {
//      Serial.print("Error in readSample()\n");
//  }

  delay(1000);
//
//  tcaselect(SHT30_2_PIN);
//  if (sht2.readSample()) {
//      Serial.print("SHT2:\n");
//      Serial.print("  RH2: ");
//      Serial.print(sht2.getHumidity(), 2);
//      Serial.print("\n");
//      Serial.print("  T2:  ");
//      Serial.print(sht2.getTemperature(), 2);
//      Serial.print("\n");
//  } else {
//      Serial.print("Error in readSample()\n");
//  }

  delay(1000);
  
}
