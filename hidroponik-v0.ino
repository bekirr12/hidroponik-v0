#include <Arduino.h>

// pin definition
#define PUMP_RELAY_PIN 5

// Relay polarity: active-LOW module
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

void setup() {
  Serial.begin(115200);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  Serial.println("Setup complete, starting relay test");
}

void loop() {
  digitalWrite(PUMP_RELAY_PIN, RELAY_ON);
  Serial.println("Pump ON");
  delay(2000);

  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  Serial.println("Pump OFF");
  delay(2000);
}