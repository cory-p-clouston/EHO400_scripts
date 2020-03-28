# Summary of Tests

## DISCLAIMER! 

ALL THE SCRIPTS ABOVE ARE BASED ON THE EXAMPLE SCRIPTS OF THE CORRESPONDING LIBRARY (IES).

## Arduino IDE settings of every tests

Board:	" Arduino / Genuino Mega or Mega 2560 " 

Processor:	" ATmega2560 (Mega 2560)"

Port:	" COM8 (Arduino / Genuino Mega or Mega 2560)"

Programmer:	"ArduinoISP"

## 01_blink

**Hardware:** MEGA

**Description:**  Basic Blink LED test

**Comment:** See the led blink

**Static Memory (bytes / percent):** 1448 (0%)

**Dynamic Memory (bytes / percent):** 9 (0%)

**Library (ies) used:** None


## 02_SD_card_basic_test

**Hardware:** MEGA + Ethernet shield

**Description:** Basic SD file create write read delete
 
**Comment:** File gets created, written, read, and deleted

**Static Memory (bytes / percent):** 12160 (4%)

**Dynamic Memory (bytes / percent):** 1134 (13%)

**Library (ies) used:** SPI, SD


## 03_Ethernet_web_server

**Hardware:** MEGA + Ethernet shield

**Description:** Basic Ethernet webserver

**Comment:** 
"See the Analog input 0 to 5 on Firefox (192.168.11.177:80) refreshes every few seconds.
Values are between 330 and 390 (nothing is connected on the Aout).
Direct connection between ethernet shield and computer. Laptop is 192.168.11.155"

**Static Memory (bytes / percent):** 12658 (4%)

**Dynamic Memory (bytes / percent):** 769 (9%)

**Library (ies) used:** SPI, Ethernet


## 04_sd_files_over_ethernet

**Hardware:** MEGA + Ethernet shield

**Description:** Basic Ethernet + SD Card

**Comment:**
"I can connect to the ethernet shield through Firefox and download the file test.txt
Direct connection between ethernet shield and computel. Laptop is 192.168.11.155"

**Static Memory (bytes / percent):** 22238 (8%)

**Dynamic Memory (bytes / percent):** 1395 (17%)

**Library (ies) used:** SPI, SD, Ethernet


## 05_multiple_DS18B20

**Hardware:** MEGA + 2 DS18B20 + 4k7 ohm

**Description:** Basic 2 x DS18B20 temp reading

**Comment:**
"Requesting temperatures...DONE
Device Address: 28AACC44401401D8 Temp C: 18.37 Temp F: 65.07
Device Address: 28AA393440140104 Temp C: 20.25 Temp F: 68.45"

**Static Memory (bytes / percent):** 7008 (2%)

**Dynamic Memory (bytes / percent):** 584 (7%)

**Library (ies) used:** OneWire, DallasTemperature


## 06_single_SHT30-D_auto_detect

**Hardware:** MEGA + 1 SHT30-D

**Description:** Basic 1 SHT30-D humidity reading 

**Comment:** "Serial monitor, see the RH and the temperature refreshing every few seconds.
No external pull-ups are connected to the I2C SDA and SCL"

**Static Memory (bytes / percent):** 7452 (2%)

**Dynamic Memory (bytes / percent):** 544 (6%)

**Library (ies) used:** Wire, SHTSensor (arduino-sht)


## 07_co2_sensor_T6713

**Hardware:** MEGA + 1 T6713

**Description:** Basic 1 T6713 CO2 reading

**Comment:**
"Following Application Note AN161. The CO2 measurement was refreshing every 2s.
Roughly 1100 ppm. Increased when I started to breathe closer to the sensor.
No custom library required."

**Static Memory (bytes / percent):** 4480 (4%)

**Dynamic Memory (bytes / percent):** 502 (6%)

**Library (ies) used:** Wire


## 08_current_measurement_SCT013

**Hardware:** MEGA + 1 SCT013

**Description:** Basic current measurement 1

**Comment:**
"See the current varying when I change the speed of the fan (about 0.15 to 0.18 A). 
It is working properly, however:
The SCT013 is for AC Current only! Cannot use it with the NMP30 motor (12V dc)"

**Static Memory (bytes / percent):** 4354 (1%)

**Dynamic Memory (bytes / percent):** 316 (3%)

**Library (ies) used:** EmonLib


## 09_current_measurement_INA219

**Hardware:** MEGA + INA219

**Description:** Basic current measurement 2

**Comment:**
"There is another library called ArduinoINA219 that seems more complete.
Kept the default range 32V - 2 Amps in case all three motors runs at the same time.
When the INA219 is not connected, it still output some values (not good).
When there is no load, we see see about 1mA."

**Static Memory (bytes / percent):** 6500 (2%)

**Dynamic Memory (bytes / percent):** 573 (3%)

**Library (ies) used:** Wire, Adafruit_INA219


## 11_multiple_I2C_sht30_and_T6713

**Hardware:** MEGA + T6713 + SHT30-D

**Description:** Multiple I2C devices

**Comment:**
"We see the CO2 measure, temp and RH but: 
We cannot change the I2C address of the SHT30-D because it is packaged. 
Therefore we need an I2C multiplexer to have more than one humidity sensor"

**Static Memory (bytes / percent):** 7946 (3%)

**Dynamic Memory (bytes / percent):** 652 (7%)

**Library (ies) used:** Wire, SHTSensor (arduino-sht)


## 12_integration_sd_ethernet_ds18b20_sht30

**Hardware:** MEGA + Ethernet + DS18B20 + SHT30-D 

**Description:** Multiple devices + SD card + ethernet

**Comment:**
"The two ds18b20 and the sht30 writes properly to the SD file.
The file can be transferred from the Firefox browser. The laptop IP address is 192.168.11.155
The web browser is quite slow because I am using  a delay in the script. 
30K is required for the static memory. Uno is 31.5K. 
All the files' names are in capitals. I don't know why."

**Static Memory (bytes / percent):** 30882 (12%)

**Dynamic Memory (bytes / percent):** 2190 (26%)

**Library (ies) used:** Wire, DallasTemperature, Wire, SHTSensor, SPI, SD, Ethernet


## 13_basic_relay_control_DFR0144

**Hardware:** MEGA + DFR0144

**Description:** Basic relay control

**Comment:**
"Relay pins = 2, 7, 8, 10. Specs in the datasheet are only for resistive loads.
Need to use the external power (the jack)."

**Static Memory (bytes / percent):** 2966 (1%)

**Dynamic Memory (bytes / percent):** 296(3%)

**Library (ies) used:** None


## 14_basic_relay_and_motor_control_DFR0144

**Hardware:** MEGA + DFR0144 + NMP30

**Description:** Basic Motor control

**Comment:**
"Added a 1N4004 in parallel to the motor. 
Motor starts and stops properly. 
Same script as test number 13."

**Static Memory (bytes / percent):** 2966 (1%)

**Dynamic Memory (bytes / percent):** 296(3%)

**Library (ies) used:** None


## 15_one_pump_and_current_measurement_INA219

**Hardware:** MEGA + DFR0144 + NMP30 + INA219

**Description:** Basic relay/Motor + current measurement

**Comment:**
"The measurement seems to be working fine in terms of power. 
Need to make another test that reads as often as possible to try to catch the inrush current.
The results are way lower than the motor specs. (we get 500mW but the motor is rated 12V/0.46Amps, so it should be up to 5W)."

**Static Memory (bytes / percent):** 6778 (2%)

**Dynamic Memory (bytes / percent):** 601 (7%)

**Library (ies) used:** Wire, Adafruit_INA219


## 16_basic_I2C_multiplexer_TCA9548A

**Hardware:** MEGA + 2 SHT30 + TCA9548A

**Description:** Multiple I2C devices with multiplexer

**Comment:**
"Works as expected. 
I don't know why but for each SHT30 sensor, there are two I2C addresses displayed (0x0 and 0x44) "

**Static Memory (bytes / percent):** 4210 (1%)

**Dynamic Memory (bytes / percent):** 446(5%)

**Library (ies) used:** Wire, utility/twi.h


## 17_temp_comparison_ds18b20_sht30

**Hardware:** MEGA + DS18B20 + SHT30

**Description:** Sensors Temp. comparison

**Comment:**
"The error between two ds18b20 is bigger than the error between a ds18b20 and an SHT30.
It gives roughly an error of 2% "

**Static Memory (bytes / percent):** 10776 (4%)

**Dynamic Memory (bytes / percent):** 932 (11%)

**Library (ies) used:** OneWire, DallasTemperature, Wire, SHTSensor


## 18_three_shields_together

**Hardware:** MEGA + Ethernet + DFR0144

**Description:** Three shields together

**Comment:**
"I had to unsolder the servo connector and one of the pins from the I2C header. 
The shields still do not fit perfectly but it works.
I added spacers between the ethernet shield and the arduino so that it does not move when pressing on the relays shield.
Added electrical tape on the RJ45 to avoid shorts.
Will need to change the headers (at least 12mm) + add standoffs
SD card not easily accessible. Need an extender
There is one pin conflict: cannot use relay 4. Removed the jumper.
The relays toggle every second. Can fetch the file on internet properly. SD card and ethernet were properly started.
When dowloading a file, sometimes we can see the relay takes more than one second to toggle again.
Disconnecting and reconnecting the ethernet cable does not seem to have an impact."

**Static Memory (bytes / percent):** 22398 (8%)

**Dynamic Memory (bytes / percent):** 1401 (17%)

**Library (ies) used:** SPI, SD, Ethernet


## 19_three_motors_with_relays

**Hardware:** MEGA + DFR0144 + 3 NMP30

**Description:** Three motors at the same time

**Comment:**
"Everything is working as expected.
We will need to fix the motors quite well because they are moving quite a bit.
No fluctuation seen in the LED"

**Static Memory (bytes / percent):** 2706 (1%)

**Dynamic Memory (bytes / percent):** 188 (2%)

**Library (ies) used:** None

## 20_three_motors_with_current_measurement_INA219

**Hardware:** MEGA + DFR0144 + 3 NMP30 + INA219

**Description:** Three motors + current measurement

**Comment:**
"We can see a voltage drop when more than one motor is activated.
We can see current spikes of 1.5A (when starting only one motor).
In steady states all three motors requires only 800mA total.
Need to add capacitors!
It is not that obvious that all motors are on when looking only at the current. 
Need to wait for steady-state.
Added 0.5s of delay after pump ON and before reading measurements and the thread is way easier to analyze."

**Static Memory (bytes / percent):** 7096 (2%) 

**Dynamic Memory (bytes / percent):** 679 (8%)

**Library (ies) used:** Adafruit_INA219


## 21_all_sensors_with_ethernet_and_sd

**Hardware:** MEGA + ethernet shield + 2 DS18B20 + 2 SHT30 + INA219 + T6713

**Description:** All the sensors together + Ethernet + SD

**Comment:**
"Everything is working as expected. 
Need to discuss about the format in the file 
CO2 sensor reading seems to be quite high."

**Static Memory (bytes / percent):** 34036 (13%)

**Dynamic Memory (bytes / percent):** 2505 (30%)

**Library (ies) used:** OneWire, DallasTemperature, Wire, SHTSensor, SPI, SD, Ethernet, Adafruit_INA219


## 22_all_sensors_and_pumps_with_ethernet_and_sd

**Hardware:** MEGA + ethernet shield + 2 DS18B20 + 2 SHT30 + INA219 + T6713 + 3 NMP30 + DFR0144

**Description:** Pumps and sensors

**Comment:**
"Also working as expected. 
It will not be possible to retrieve files when pumps are working."

**Static Memory (bytes / percent):** 36872 (14%)

**Dynamic Memory (bytes / percent):** 2765 (33%)

**Library (ies) used:** OneWire, DallasTemperature, Wire, SHTSensor, SPI, SD, Ethernet, Adafruit_INA219


## 25_Basic_NTP_time_stamps

**Hardware:** MEGA + Ethernet

**Description:** Basic NTP time stamps

**Comment:**
"It is possible to get the time from internet but it requires DHCP
Ethernet.localIP gives the assigned IP. Was able to ping the IP from my computer.
Need to figure out to find the IP address without the serial monitor.
(Sending the IP address to Thingspeak)."

**Static Memory (bytes / percent):** 13496 (5%)

**Dynamic Memory (bytes / percent):** 876 (10%)

**Library (ies) used:** SPI, Ethernet, EthernetUdp


## 26b_thingspeak_multiple_feeds_ethernet_board

**Hardware:** MEGA + Ethernet

**Description:** Display values on internet

**Comment:** "Limited to 8 fields / update every 20seconds."

**Static Memory (bytes / percent):** 23732 (9%)

**Dynamic Memory (bytes / percent):** 1342 (16%)

**Library (ies) used:** ThingSpeak, Ethernet


## 28_thingspeak_NTP_and_SD

**Hardware:** MEGA + Ethernet

**Description:** ThingSpeak + web server

**Comment:**
"Seems to be working fine. 
Script ran for > 10 minutes and it is still working fine"

**Static Memory (bytes / percent):** 34642 (13%)

**Dynamic Memory (bytes / percent):** 2308 (28%)

**Library (ies) used:** ThingSpeak, Ethernet, SPI, SD


## 29_thingspeak_webserver_and_NTP

**Hardware:** MEGA + Ethernet

**Description:** ThingSpeak + web server + NTP

**Comment:**
"After 1h15min, still seems to be working fine.
Most likely the problem of test 27 was the delays."

**Static Memory (bytes / percent):** 35752 (14%)

**Dynamic Memory (bytes / percent):** 2459 (30%)

**Library (ies) used:** ThingSpeak, Ethernet, SPI, EthernetUdp, SD


## 34_integration4

**Hardware:** MEGA + ethernet shield + 2 DS18B20 + 2 SHT30 + INA219 + T6713 + 3 NMP30 + DRF0144 + NTP + AdafruitIO

**Description:** Everything together
**Comment:**
"Need to be careful about the delays! We might loose internet connection.
There are several edge cases that are not taken into account (e.g. what if we loose the ethernet)"

**Static Memory (bytes / percent):** 45958 (18%)

**Dynamic Memory (bytes / percent):** 3755 (45%)

**Library (ies) used:** ThingSpeak, Ethernet, SPI, SD, EthernetUdp, OneWire, DallasTemperature, Wire, SHTSensor, AdafruitINA219


**END OF DOCUMENT**

