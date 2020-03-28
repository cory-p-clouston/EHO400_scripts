// Simple test turning pumps on and off
// Adding INA219 as current measurements
// Each relay is controlling one pump
// Cory Paquet-Clouston

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

byte relayPin[3] = {
  2,7,8};
//D2 -> RELAY1
//D7 -> RELAY2
//D8 -> RELAY3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);

  for(int i = 0; i < 3; i++){
    pinMode(relayPin[i],OUTPUT);
    digitalWrite(relayPin[i],LOW);
  }

  ina219.begin();

  
}

void displayCurrentMeasurements(){
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
  
}

void loop() {
  // put your main code here, to run repeatedly:

  // Activating only one pump at the time
  Serial.println("Cycle 1 ------------------ ");
  displayCurrentMeasurements();
  for(int j = 0; j < 3; j++){
    Serial.print("turning ON pump "); Serial.println(j);
    digitalWrite(relayPin[j], HIGH);
    delay(500);
    displayCurrentMeasurements();
    delay(1000);
    Serial.print("turning OFF pump "); Serial.println(j);
    digitalWrite(relayPin[j], LOW);
    delay(500);
    displayCurrentMeasurements();
  }

  // Activating one pump after the other
  Serial.println("Cycle 2 ------------------- ");
  for(int k = 0; k < 3; k++){
    Serial.print("turning ON pump "); Serial.println(k);
    digitalWrite(relayPin[k], HIGH);
    delay(500);
    displayCurrentMeasurements();
    delay(1000);
  }

  // All pumps are ON
  Serial.println("All pumps are ON");
  delay(3000);
  displayCurrentMeasurements();

  // Desactivating one pump at the time
  for(int m = 0; m < 3; m++){
    Serial.print("turning OFF pump "); Serial.println(m);
    digitalWrite(relayPin[m],LOW);
    delay(500);
    displayCurrentMeasurements();
    delay(1000);
  }

  // All pumps are OFF
  Serial.println("All pumps are OFF");
  delay(2500);
  displayCurrentMeasurements();
  delay(2500);

  Serial.println(" END ----------------------- ");
  
}
