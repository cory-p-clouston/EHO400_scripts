# EHO400
An Arduino Mega-based project for monitoring of a controlled environment lab.

![Picture of EHO400](/Pictures/EHO400.png)

# Summary

This project used several components (sensors, shields, etc) and we tested them individually / using different combinaisons.  It seemed like a waste not to share those scripts with everyone :) . 

The different sensors, boards, shields, features tested: 

	1. Temperature sensor DS18B20
	1. Humidity and temperature sensor SHT30-D
	1. CO2 sensor T6713
	1. Ethernet shield (inc. SD card and ISP breakout)
	1. Relay board DFR0144
	1. Current Sensor INA219
	1. Current probe SCT013
	1. Sending data to ThingSpeak
	1. 3x 12V-pumps
	1. Sending data to thingskeap
	1. Saving data to SD card
	1. Webserver to get the file from the SD card
	1. Using NTP for time stamps
	1. Integrating everything together.

The excel file shows the conclusions from the different tests. 

Some things to keep in mind:
* We had to upgrade from an Arduino UNO to an Arduino MEGA because the UNO did not have enough memory space for the program.
* We had to use an Ethernet shield with an ISP breakout because there were many pins conflicts with the DFR0144 shield.
* Even with the point above, we still had one pin in conflict between two boards (D10). Watch out for that if you stack shields.


We hope it will help you for your projects! 
