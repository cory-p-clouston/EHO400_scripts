# EHO400
An Arduino Mega-based project for monitoring of a mushroom growing lab.

The key features are:
    
    Monitoring of temperature (DS18B20)
    Monitoring of humidity (SHT30-D)
    Monitoring of CO2 (T6713)
    All the data are saved on an SD Card, which is on an Ethernet shield
    The file saved can be downloaded remotely.
    The system controls 3 12V-pumps using DFR0144 board*
    Using a current sensor to validate the pumps started (INA219).
    The Arduino MEGA sends part of the data on ThingSpeak**

*we have only one CO2 sensor but we need to monitoring the in-take and the out-take of the room + outside for the calibration of the sensor. 

** Simply to allow us to do spot check on the behaviour of the system, to make sure everything is running smoothly.

Total budget: 400$ Duration of the monitoring: 6 months

