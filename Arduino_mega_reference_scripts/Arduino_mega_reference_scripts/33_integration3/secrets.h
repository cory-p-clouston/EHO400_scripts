// Use this file to store all of the private credentials 
// and connection details

#include "ThingSpeak.h"
#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>
#include <EthernetUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "SHTSensor.h"
#include <Adafruit_INA219.h>


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
#define SECRET_MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}

#define SECRET_CH_ID 958125			// replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "R2IPOUM8DFZXVEI6"   // replace XYZ with your channel write API Key
