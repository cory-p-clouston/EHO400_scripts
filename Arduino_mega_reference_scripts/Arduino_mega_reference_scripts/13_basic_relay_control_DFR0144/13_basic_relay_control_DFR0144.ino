/*
 # This Sample code is for testing the Relay shield V2.1 for Arduino.

 # Editor : Phoebe
 # Date   : 2013.2.28
 # Ver    : 0.1
 # Product: Relay shield for Arduino
 # SKU    : DRF0144

 # Hardwares:
 1. Arduino MEGA
 2. Relay Shield For Arduino V2.1
 3  Power Supply:7~ 12V
 */

byte relayPin[4] = {
  2,7,8,10};
//D2 -> RELAY1
//D7 -> RELAY2
//D8 -> RELAY3
//D10 -> RELAY
char input=0;
int val=1;

void setup() {
  for(int i = 0; i < 4; i++) pinMode(relayPin[i], OUTPUT);

  Serial.begin(57600);
  delay(100);
  Serial.println("Press 1-4 to control the state of the relay");
  Serial.println("waiting for input:");
  for(int j = 0; j < 4; j++)  digitalWrite(relayPin[j], HIGH);
}

void loop() {
  if (Serial.available())
  {
    char input= Serial.read();
    if(input != -1)
    {
      switch(input)
      {

      case '1':
        Serial.println("Relay1");
        val=digitalRead(relayPin[0]);
        val=!val;
        digitalWrite(relayPin[0],val);
        break;
      case '2':
        Serial.println("Relay2");
        val=digitalRead(relayPin[1]);
        val=!val;
        digitalWrite(relayPin[1],val);
        break;
      case '3':
        Serial.println("Relay3");
        val=digitalRead(relayPin[2]);
        val=!val;
        digitalWrite(relayPin[2],val);
        break;
      case '4':
        Serial.println("Relay4");
        val=digitalRead(relayPin[3]);
        val=!val;
        digitalWrite(relayPin[3],val);
        break;

      default:
        if(input != '\r' && input != '\n')
         Serial.println("invalid entry");
        break;
      }
    }
//    else unablerelay();
  }
}
