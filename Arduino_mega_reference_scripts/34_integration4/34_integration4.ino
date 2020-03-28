/*
  Cory Paquet-Clouston: Integration 4 
  Cory: merging script 29 and 22 together to have everything
  
*/

#include "secrets.h"

/************ THINGSPEAK STUFF ************/
double thingSpeakData[8] = {
  -128, -128, -128, -128, -128, -128, -128, -128
};

#define TS_CO2_OUT 0
#define TS_CO2_IN 1
#define TS_TEMP_IN 2
#define TS_TEMP_OUT 3
#define TS_HUM_IN 4
#define TS_HUM_OUT 5
/************ NTP STUFF ************/
unsigned int localPort = 8888;       // local port to listen for UDP packets
const char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;


/************ ETHERNET STUFF ************/
EthernetServer server(80);
#define WS_CS 10

/************ SDCARD STUFF ************/
#define SDCARD_CS 4
#define FILENAME "test2.txt"
File root;
File myFile;

/************ INA219 STUFF ************/
Adafruit_INA219 ina219;
#define THRESHOLD_CURRENT_OFF 5.0 //mA
#define T_WAIT_TRANSIENT 500 // milliseconds
/************ SHT30 STUFF ************/
SHTSensor sht1;
SHTSensor sht2;

#define TCAADDR 0x70
// These are the numbers for the multiplexer
#define SHT30_1_PIN 1
#define SHT30_2_PIN 3

/************ CO2 STUFF ************/
#define ADDR_6713 0x15 // default I2C slave address
int data[4];
int CO2ppmValue;


/************ RELAY STUFF ************/
byte relayPin[3] = {
  2, 7, 8
};
//D2 -> RELAY1
//D7 -> RELAY2
//D8 -> RELAY3

// Don't want to use delays.
#define T_SAMPLING 10 // in seconds


/************ DS18B20 STUFF ************/
#define ONE_WIRE_BUS 37
#define TEMPERATURE_PRECISION 12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress temperature1, temperature2;


// constants won't change:
const long interval = 30000;  // for thingspeak
const long interval2 = 60000; // for UDP and measurements
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

byte mac[] = SECRET_MAC;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

EthernetClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;


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


void setup() {

  Serial.begin(115200);

  // Motor stuff
  for (int i = 0; i < sizeof(relayPin); i++)  pinMode(relayPin[i], OUTPUT);
  for (int j = 0; j < sizeof(relayPin); j++)  digitalWrite(relayPin[j], LOW);

  // In https://store.arduino.cc/usa/arduino-ethernet-shield-2
  // It says to keep it as output even if it is not used.
  pinMode(53, OUTPUT);

  // Disable the ethernet controller while starting SD
  digitalWrite(WS_CS, HIGH);
  pinMode(WS_CS, OUTPUT);

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
  digitalWrite(SDCARD_CS, HIGH);
  pinMode(SDCARD_CS, OUTPUT);


  Ethernet.init(WS_CS);  // Most Arduino Ethernet hardware
  Serial.begin(115200);  //Initialize serial

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());

    Serial.print(" My DNS IP: ");
    Serial.println(Ethernet.dnsServerIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);

  ThingSpeak.begin(client);  // Initialize ThingSpeak

  // Start server
  server.begin();

  // Start UDP for NTP
  Udp.begin(localPort);

  // Start-up the sensors

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
  
  // Some gymnastic to get the deviceAddress into a format we can print
  //String deviceID = String(deviceAddress[6], HEX) + String(deviceAddress[7], HEX);
  uint8_t deviceID = -1;
  if (deviceAddress == temperature1) deviceID = 0;
  if (deviceAddress == temperature2) deviceID = 1;


  // if the file opened okay, write to it:
  if (myFile) {


    myFile.print(", DS18B20-" + String(deviceID) + "(degC), "); myFile.print(tempC, 3);
    myFile.close();

    thingSpeakData[deviceID + 2] = tempC;

  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening "); Serial.println(FILENAME);
    thingSpeakData[deviceID + 2] = -129;
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
        thingSpeakData[TS_HUM_IN] = humidity;
      }
      else {
        Serial.println("ERROR in sht1.readSample -> getSHTTemp function");
        thingSpeakData[TS_HUM_IN] = -129;
      }

      break;
    case SHT30_2_PIN:
      if ( sht2.readSample()) {
        humidity = sht2.getHumidity();
        temperature = sht2.getTemperature();

        thingSpeakData[TS_HUM_OUT] = humidity;
      }
      else {
        Serial.println("ERROR in sht1.readSample -> getSHTTemp function");
        thingSpeakData[TS_HUM_OUT] = -129;
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
    myFile.print(", CO2(ppm), "); myFile.print(CO2ppmValue);
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


void CO2MeasurementProcedure(uint8_t index) {

  // Start by measuring the current
  float current_mA = ina219.getCurrent_mA();

  if (current_mA < THRESHOLD_CURRENT_OFF) {

    digitalWrite(relayPin[index], HIGH);
    delay(T_WAIT_TRANSIENT);
    current_mA = ina219.getCurrent_mA();

    if (current_mA < THRESHOLD_CURRENT_OFF) {
      Serial.println("ERROR: The motor did not start ");
      Serial.println("Current measurement is : " + String(current_mA));
      Serial.println("Threshold is : " + String(THRESHOLD_CURRENT_OFF));
      digitalWrite(relayPin[index], LOW);
    }

    myFile = SD.open(FILENAME, FILE_WRITE);
    if (myFile) {

      if (current_mA < THRESHOLD_CURRENT_OFF) {
        myFile.print(", ERROR Pump-" + String(index));
      }
      else {
        myFile.print(", GOOD Pump-" + String(index));
      }

      myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.print("error opening "); Serial.println(FILENAME);
    }

    if (current_mA < THRESHOLD_CURRENT_OFF) {
      //delay(T_SAMPLING); // Don't use delays.
      uint8_t counter = 0;
      while (counter < T_SAMPLING) {
        counter++;
        delay(500);
        Ethernet.maintain();
        delay(500);
      }
    }

    readC02();

    if (index > 2) {
      thingSpeakData[index] = CO2ppmValue;
    }

    digitalWrite(relayPin[index], LOW);
  }
  else {
    Serial.println("ERROR: Measured current is higher than threshold.");
    Serial.println("Skipping CO2 measurement procedure.");

    myFile = SD.open(FILENAME, FILE_WRITE);

    // if the file opened okay, write to it:
    if (myFile) {

      myFile.println("");
      myFile.println("ERROR: Measured current is higher than threshold");
      myFile.println("Threshold is : " + String(THRESHOLD_CURRENT_OFF));
      myFile.println("Measured current is : " + String(current_mA));
      myFile.close();
    }

    for (int p = 0; p < sizeof(relayPin); p++) digitalWrite(relayPin[p], LOW);

    if (index > 2) {
      thingSpeakData[index] = -129;
    }
  }
}




void loop() {

  unsigned long currentMillis = millis();

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

  // For NTP
  if (currentMillis - previousMillis2 >= interval2) {
    uint8_t UpdWorking = 0;
    previousMillis2 = currentMillis;
    sendNTPpacket(timeServer); // send an NTP packet to a time server

    // wait to see if a reply is available
    delay(1000);
    if (Udp.parsePacket()) {
      UpdWorking = 1;
      // We've received a packet, read the data from it
      Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

      // the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, extract the two words:

      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;

      // Start by changing the line in the file
      // and adding the time stamp
      myFile = SD.open(FILENAME, FILE_WRITE);
      if (myFile) {
        myFile.println("");
        myFile.println(secsSince1900);
        myFile.close();
      } else {
        Serial.println("Isch");
      }

      Serial.print("Seconds since Jan 1 1900 = ");
      Serial.println(secsSince1900);

      // now convert NTP time into everyday time:
      Serial.print("Unix time = ");
      // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      unsigned long epoch = secsSince1900 - seventyYears;
      // print Unix time:
      Serial.println(epoch);


      // print the hour, minute and second:
      Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
      Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
      Serial.print(':');
      if (((epoch % 3600) / 60) < 10) {
        // In the first 10 minutes of each hour, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
      Serial.print(':');
      if ((epoch % 60) < 10) {
        // In the first 10 seconds of each minute, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.println(epoch % 60); // print the second
    }

    // Only if we didn't change the line earlier.
    if (UpdWorking == 0) {
      // Start by changing the line in the file
      // and adding the time stamp
      myFile = SD.open(FILENAME, FILE_WRITE);
      if (myFile) {
        myFile.println("");
        myFile.close();
      } else {
        Serial.println("Isch");
      }
    }


    for (int n = 0; n < sizeof(relayPin); n++) {
      CO2MeasurementProcedure(n);
    }

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

  // For ThingSpeak
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Serial.println("Writing to ThingSpeak");

    // set the fields with the values
    for (uint8_t i = 0; i > 6; i++) ThingSpeak.setField(i + 1, long(thingSpeakData[i]));

    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
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

  // Just to be sure we don't loose connection
  // Can be called as often as desired
  Ethernet.maintain();
}

// send an NTP request to the time server at the given address
void sendNTPpacket(const char * address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); // NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();


}
