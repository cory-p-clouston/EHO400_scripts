// Simple test toggling one relay at the time 
// Each relay is controlling one pump
// Cory Paquet-Clouston

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
}

void loop() {
  // put your main code here, to run repeatedly:

  // Activating only one pump at the time
  for(int j = 0; j < 3; j++){
    digitalWrite(relayPin[j], HIGH);
    delay(1000);
    digitalWrite(relayPin[j], LOW);
    delay(500);
  
  }

  // Activating one pump after the other
  for(int k = 0; k < 3; k++){
    digitalWrite(relayPin[k], HIGH);
    delay(1000);
  }

  // All pumps are ON
  delay(3000);

  // Desactivating one pump at the time
  for(int m = 0; m < 3; m++){
    digitalWrite(relayPin[m],LOW);
    delay(1000);
  }

  // All pumps are OFF
  delay(5000);
}
