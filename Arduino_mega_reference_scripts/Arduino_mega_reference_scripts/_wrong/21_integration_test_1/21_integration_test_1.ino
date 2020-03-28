// Cory Paquet-Clouston 2020-01-11
// Integration test 1

// Using Arduino MEGA + Ethernet Shield + DFR0144
// + 2 x DS18B20 + 2 x SHT30 (I2C multiplexer required)
// + T6713 + INA219 + SD Card

// (no pumps in this one)

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "SHTSensor.h"
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <Adafruit_INA219.h>



// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;

// constants won't change:
const long interval = 5000;

/************ ETHERNET STUFF ************/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // change if necessary
byte ip[] = { 192, 168, 11, 177 };                     // change if necessary
EthernetServer server(80);
#define WIZ_CS 10

/************ SDCARD STUFF ************/
#define SDCARD_CS 4
#define FILENAME "raw_data.txt"
File root;
File myFile;

/************ INA219 STUFF ************/
Adafruit_INA219 ina219;

/************ SHT30 STUFF ************/
SHTSensor sht1;
SHTSensor sht2;

#define TCAADDR 0x70
// These are the numbers for the multiplexer
#define SHT30_1_PIN 1
#define SHT30_2_PIN 3

/************ CO2 STUFF ************/
#define ADDR_6713 0x15 // default I2C slave address
int data [4];
int CO2ppmValue;


/************ DS18B20 STUFF ************/
#define ONE_WIRE_BUS 37
#define TEMPERATURE_PRECISION 12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress temperature1, temperature2;

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))

void error_P(const char* str) {
  Serial.print(F("error: "));
  Serial.println(str);

  while (1);
}

void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}



void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


void setup() {
  // start serial port
  Serial.begin(115200);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Disable the ethernet controller while starting SD
  digitalWrite(WIZ_CS, HIGH);
  pinMode(WIZ_CS, OUTPUT);

  // In https://store.arduino.cc/usa/arduino-ethernet-shield-2
  // It says to keep it as output even if it is not used.
  pinMode(53, OUTPUT);
  // Start-up the SD card
  if (!SD.begin(SDCARD_CS)) {
    error("card.init failed!");
  }

  root = SD.open("/");
  printDirectory(root, 0);

  // Recursive list of all directories
  Serial.println(F("Files found in all dirs:"));
  printDirectory(root, 0);

  Serial.println();
  Serial.println(F("Done"));

  // Disable the SD controller while starting Ethernet
  digitalWrite(WIZ_CS, HIGH);
  pinMode(WIZ_CS, OUTPUT);

  // Debugging complete, we start the server!
  Serial.println(F("Initializing WizNet"));
  Ethernet.init(WIZ_CS);
  // give the ethernet module time to boot up
  delay(1000);

  // start the Ethernet connection
  // Use the fixed IP specified. If you want to use DHCP first
  //   then switch the Ethernet.begin statements
  Ethernet.begin(mac, ip);
  // try to congifure using DHCP address instead of IP:
  //  Ethernet.begin(mac);

  // print the Ethernet board/shield's IP address to Serial monitor
  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());

  server.begin();

  // Start up the library
  sensors.begin();

  Wire.begin();

  ina219.begin();
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  if (!sensors.getAddress(temperature1, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(temperature2, 1)) Serial.println("Unable to find address for Device 1");

  // SHT30 stuff

  tcaselect(SHT30_1_PIN);
  if (sht1.init()) {
    Serial.print("init1(): success\n");
  } else {
    Serial.print("init1(): failed\n");
  }
  sht1.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x

  tcaselect(SHT30_2_PIN);
  if (sht2.init()) {
    Serial.print("init2(): success\n");
  } else {
    Serial.print("init2(): failed\n");
  }
  sht2.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x

}


void ListFiles(EthernetClient client, uint8_t flags, File dir) {
  client.println("<ul>");
  while (true) {
    File entry = dir.openNextFile();

    // done if past last used entry
    if (! entry) {
      // no more files
      break;
    }

    // print any indent spaces
    client.print("<li><a href=\"");
    client.print(entry.name());
    if (entry.isDirectory()) {
      client.println("/");
    }
    client.print("\">");

    // print file name with possible blank fill
    client.print(entry.name());
    if (entry.isDirectory()) {
      client.println("/");
    }

    client.print("</a>");
    /*
        // print modify date/time if requested
        if (flags & LS_DATE) {
           dir.printFatDate(p.lastWriteDate);
           client.print(' ');
           dir.printFatTime(p.lastWriteTime);
        }
        // print size if requested
        if (!DIR_IS_SUBDIR(&p) && (flags & LS_SIZE)) {
          client.print(' ');
          client.print(p.fileSize);
        }
    */
    client.println("</li>");
    entry.close();
  }
  client.println("</ul>");
}

// How big our line buffer should be. 100 is plenty!
#define BUFSIZ 100


// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  double tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC, 3);
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  myFile = SD.open(FILENAME, FILE_WRITE);

  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();

  double tempC = sensors.getTempC(deviceAddress);


  // if the file opened okay, write to it:
  if (myFile) {

    // Some gymnastic to get the deviceAddress into a format we can print
    //String deviceID = String(deviceAddress[6], HEX) + String(deviceAddress[7], HEX);
    uint8_t deviceID = -1;
    if (deviceAddress == temperature1) deviceID = 0;
    if (deviceAddress == temperature2) deviceID = 1;

    myFile.print(", DS18B20-" + String(deviceID) + "(degC), "); myFile.print(tempC, 3);
    myFile.close();

  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening "); Serial.println(FILENAME);
  }

}

void getSHTTemp(uint8_t tca_index) {

  tcaselect(tca_index);
  float humidity = -127;
  float temperature = -127;

  switch (tca_index)
  {
    case SHT30_1_PIN:
      if ( sht1.readSample()) {
        humidity = sht1.getHumidity();
        temperature = sht1.getTemperature();
      }
      else {
        Serial.println("ERROR in sht1.readSample -> getSHTTemp function");
      }

      break;
    case SHT30_2_PIN:
      if ( sht2.readSample()) {
        humidity = sht2.getHumidity();
        temperature = sht2.getTemperature();
      }
      else {
        Serial.println("ERROR in sht1.readSample -> getSHTTemp function");
      }
      break;

    default:
      Serial.println("Case not supported in getSHTTemp function");
      break;
  }
  Serial.print("SHT: "); Serial.print(tca_index);
  Serial.print("  RH: "); Serial.print(humidity);
  Serial.print("  T:  "); Serial.println(temperature);

  myFile = SD.open(FILENAME, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(", SHT30-" + String(tca_index) + " RH, "); myFile.print(humidity);
    myFile.print(", SHT30-" + String(tca_index) + " T(degC), "); myFile.print(temperature);
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening "); Serial.println(FILENAME);
  }

}

void readC02()
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
  Serial.print("Func code: "); Serial.print(data[0], HEX);
  Serial.print(" byte count: "); Serial.println(data[1], HEX);
  Serial.print("MSB: 0x"); Serial.print(data[2], HEX);
  Serial.print(" ");
  Serial.print("LSB: 0x"); Serial.print(data[3], HEX);
  Serial.print(" ");
  CO2ppmValue = ((data[2] * 0xFF ) + data[3]);
  Serial.print("CO2ppmValue :"); Serial.println(CO2ppmValue);

  myFile = SD.open(FILENAME, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {

    // CO2 is the first to be written so we start by changing line
    myFile.println("");
    myFile.print("CO2(ppm), "); myFile.print(CO2ppmValue);

    myFile.close();

  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening "); Serial.println(FILENAME);
  }



}

void printINA219meas() {
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");

  myFile = SD.open(FILENAME, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {

    myFile.print(", bus voltage(V), "); myFile.print(busvoltage);
    myFile.print(", shunt voltage(mV), "); myFile.print(shuntvoltage);
    myFile.print(", Load Voltage(V),  "); myFile.print(loadvoltage);
    myFile.print(", Current(mA), "); myFile.print(current_mA);
    myFile.print(", Power(mW), "); myFile.print(power_mW);


    myFile.close();

  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening "); Serial.println(FILENAME);
  }
}



void loop() {
  // put your main code here, to run repeatedly:

  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus

  unsigned long currentMillis = millis();

  // Update the information every interval
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // print the CO2 measurement first because it changes the line
    readC02();

    // print the temperature sensors information
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures();
    Serial.println("DONE");
    printData(temperature1);
    printData(temperature2);

    // print the humidity sensors information
    getSHTTemp(SHT30_1_PIN);
    getSHTTemp(SHT30_2_PIN);

    // print the current measurement
    printINA219meas();

  }



  char clientline[BUFSIZ];
  char name[17];
  int index = 0;

  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean current_line_is_blank = true;

    // reset the input buffer
    index = 0;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        // If it isn't a new line, add the character to the buffer
        if (c != '\n' && c != '\r') {
          clientline[index] = c;
          index++;
          // are we too big for the buffer? start tossing out data
          if (index >= BUFSIZ)
            index = BUFSIZ - 1;

          // continue to read more data!
          continue;
        }

        // got a \n or \r new line, which means the string is done
        clientline[index] = 0;

        // Print it out for debugging
        Serial.println(clientline);

        // Look for substring such as a request to get the file
        if (strstr(clientline, "GET /") != 0) {
          // this time no space after the /, so a sub-file!
          char *filename;

          filename = clientline + 5; // look after the "GET /" (5 chars)  *******
          // a little trick, look for the " HTTP/1.1" string and
          // turn the first character of the substring into a 0 to clear it out.
          (strstr(clientline, " HTTP"))[0] = 0;

          if (filename[strlen(filename) - 1] == '/') { // Trim a directory filename
            filename[strlen(filename) - 1] = 0;      //  as Open throws error with trailing /
          }

          Serial.print(F("Web request for: ")); Serial.println(filename);  // print the file we want

          File file = SD.open(filename, O_READ);
          if ( file == 0 ) {  // Opening the file with return code of 0 is an error in SDFile.open
            client.println("HTTP/1.1 404 Not Found");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<h2>File Not Found!</h2>");
            client.println("<br><h3>Couldn't open the File!</h3>");
            break;
          }

          Serial.println("File Opened!");

          client.println("HTTP/1.1 200 OK");
          if (file.isDirectory()) {
            Serial.println("is a directory");
            //file.close();
            client.println("Content-Type: text/html");
            client.println();
            client.print("<h2>Files in /");
            client.print(filename);
            client.println(":</h2>");
            ListFiles(client, LS_SIZE, file);
            file.close();
          } else { // Any non-directory clicked, server will send file to client for download
            client.println("Content-Type: application/octet-stream");
            client.println();

            char file_buffer[16];
            int avail;
            while (avail = file.available()) {
              int to_read = min(avail, 16);
              if (to_read != file.read(file_buffer, to_read)) {
                break;
              }
              // uncomment the serial to debug (slow!)
              //Serial.write((char)c);
              client.write(file_buffer, to_read);
            }
            file.close();
          }
        } else {
          // everything else is a 404
          client.println("HTTP/1.1 404 Not Found");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<h2>File Not Found!</h2>");
        }
        break;
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }

}
