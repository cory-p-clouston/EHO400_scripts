// Cory Paquet-Clouston
// Based on https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/how-to-build-an-arduino-energy-monitor-measuring-current-only 

// See reference circuit (same link as above)
// For the circuit:
// R1 = R2 = 10k ohm
// C1 = 1uF (That's all I have)
// Burden resistor = three 10k in parallel so 3.33kohm (for I primary = 10 A max). 

// See 'doc_Notes_SCT013_2019_02_17.pdf' for adjusting the current calibration

// Using a 120V fan as the load. 


#include "EmonLib.h"
// Include Emon Library
EnergyMonitor emon1;
// Create an instance
void setup()
{
  Serial.begin(9600);

  // Using A0
  // for the current adjusment: 2000 / 3.33k = 0.6
  emon1.current(0, 0.6);             // Current: input pin, calibration.
  Serial.println("Apparent_Power    Irms"); 
}

void loop()
{
  // Changed to 1500 to be a multiple of 60
  double Irms = emon1.calcIrms(1500);  // Calculate Irms only

  // Assuming constant 120V 
  Serial.print(Irms*120.0);           // Apparent power
  Serial.print(" ");
  Serial.println(Irms);             // Irms
}
