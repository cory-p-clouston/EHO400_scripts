/*
  WriteMultipleFields

  Description: Writes values to fields 1,2,3 and 4  in a single ThingSpeak update every 20 seconds.

  Hardware: Arduino Ethernet

  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!

  Note:
  - Requires the Ethernet library

  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.

  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.

  For licensing information, see the accompanying license file.

  Copyright 2018, The MathWorks, Inc.
*/


#include "ThingSpeak.h"
#include <Ethernet.h>
#include "secrets.h"
#include <SPI.h>
#include <SD.h>

/************ ETHERNET STUFF ************/
EthernetServer server(80);
#define WS_CS 10

/************ SDCARD STUFF ************/
#define SDCARD_CS 4
File root;

// constants won't change:
const long interval = 30000;
unsigned long previousMillis = 0; 

byte mac[] = SECRET_MAC;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

EthernetClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
int number1 = 0;
int number2 = random(0, 100);
int number3 = random(0, 100);
int number4 = random(0, 100);

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))

void error_P(const char* str) {
  Serial.print(F("error: "));
  Serial.println(str);

  while (1);
}


void setup() {
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

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;


    Serial.println("top of loop");

    // set the fields with the values
    ThingSpeak.setField(1, number1);
    ThingSpeak.setField(2, number2);
    ThingSpeak.setField(3, number3);
    ThingSpeak.setField(4, number4);

    Serial.println("Writing to ThingSpeak");

    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    // change the values
    number1++;
    if (number1 > 99) {
      number1 = 0;
    }
    number2 = random(0, 100);
    number3 = random(0, 100);
    number4 = random(0, 100);

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
}
