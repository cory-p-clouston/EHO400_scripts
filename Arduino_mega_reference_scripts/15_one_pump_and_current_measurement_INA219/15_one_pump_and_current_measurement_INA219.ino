// Cory Paquet-Clouston
// Simply adding and 1 second on and off for relay 1 to script 9



#include <Wire.h>
#include <Adafruit_INA219.h>

// WARNING: Even if there is nothing connected there is never a clean zero amps.

byte relayPin[3] = {
  2,7,8};
//D2 -> RELAY1
//D7 -> RELAY2
//D8 -> RELAY3
int val;


Adafruit_INA219 ina219;

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:

  for(int i = 0; i < 3; i++)  pinMode(relayPin[i],OUTPUT);
  for(int j = 0; j < 3; j++)  digitalWrite(relayPin[j],LOW);

  // Not sure what this is for
  uint32_t currentFrequency;
  
  Serial.println("Hello!");
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  ina219.begin();
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  Serial.println("Measuring voltage and current with INA219 ...");

}

void loop() {
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

  delay(1000);
  
  val=digitalRead(relayPin[0]);
  val=!val;
  digitalWrite(relayPin[0],val);
  
  Serial.print("Pump just changed to: "); Serial.println(val); 
  Serial.println("");
  delay(1000);
}
