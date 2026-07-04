#include <Arduino.h>

// pin definitions
#define PUMP_RELAY_PIN 5
#define LED_RELAY_PIN 18

// Relay polarity: active-LOW module
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

void setup() {
  Serial.begin(115200);

  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(LED_RELAY_PIN, OUTPUT);

  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  digitalWrite(LED_RELAY_PIN, RELAY_OFF);

  Serial.println("Setup complete, starting relays test");
}

void loop() {
  digitalWrite(PUMP_RELAY_PIN, RELAY_ON);
  digitalWrite(LED_RELAY_PIN, RELAY_ON);
  Serial.println("Pump and Led ON");
  delay(2000);

  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  digitalWrite(LED_RELAY_PIN, RELAY_OFF);
  Serial.println("Pump and Led OFF");
  delay(2000);
}