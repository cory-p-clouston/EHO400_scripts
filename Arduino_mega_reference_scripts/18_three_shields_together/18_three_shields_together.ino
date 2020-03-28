/*

Basic mix between the script 04 (sd file + ethernet) and toggling the relays 
to see if we can control everything when the three boards are stacked togheter
(ethernet shield in the middle because of the required ISCP connector)

Note: relay 4 cannot be used (remove the jumper) because there is a pin conflict with 
the CS pin for the ethernet controller. 

The ethernet board used is: https://www.mouser.com/catalog/specsheets/A000056_DATASHEET.pdf 
(the latest ethernet board from Arduino uses some different pins). 

the relay board is DFR0144 rev v2.1 from DF robot 

Need the ethernet cable and the dc barrel power to work properly.

 */
 
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>

/************ ETHERNET STUFF ************/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // change if necessary
byte ip[] = { 192, 168, 11, 177 };                     // change if necessary
EthernetServer server(80);


/************ RELAY STUFF ************/
byte relayPin[3] = { 2, 7, 8};
//D2 -> RELAY1
//D7 -> RELAY2
//D8 -> RELAY3
//D10 -> RELAY4 = do not use

int state = LOW;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 1000;           // interval at which to blink (milliseconds)

/************ SDCARD STUFF ************/
#define SDCARD_CS 4
File root;

#define WIZ_CS 10

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))

void error_P(const char* str) {
  Serial.print(F("error: "));
  Serial.println(str);

  while(1);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);      // For 32u4 based microcontrollers like 32u4 Adalogger Feather
 
  //Serial.print(F("Free RAM: ")); Serial.println(FreeRam());  

  if (!SD.begin(SDCARD_CS)) {
    error("card.init failed!");
  } 

  for(int i = 0; i<3; i++) pinMode(relayPin[i], OUTPUT);

  // Just following the Arduino specs to keep D53 as output
  pinMode(53, OUTPUT);
  
  root = SD.open("/");
  printDirectory(root, 0);
  
  // Recursive list of all directories
  Serial.println(F("Files found in all dirs:"));
  printDirectory(root, 0);
  
  Serial.println();
  Serial.println(F("Done"));
  
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

void loop()
{
  char clientline[BUFSIZ];
  char name[17];
  int index = 0;
  unsigned long currentMillis = millis();
  
  
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
            index = BUFSIZ -1;
          
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
 
          if(filename[strlen(filename)-1] == '/') {  // Trim a directory filename
            filename[strlen(filename)-1] = 0;        //  as Open throws error with trailing /
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
            ListFiles(client,LS_SIZE,file);  
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
    // save the last time
    previousMillis = currentMillis;

    // if the relay is off turn it on and vice-versa:
    if (state == LOW) {
      state = HIGH;
    } else {
      state = LOW;
    }  
    for(int j=0; j<3; j++) digitalWrite(relayPin[j], state);
  }


  
}


void printDirectory(File dir, int numTabs) {
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
