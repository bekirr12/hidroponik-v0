#include <Arduino.h>
#include <DHT.h>

// pin definitions
#define PUMP_RELAY_PIN 5
#define LED_RELAY_PIN 18
#define DHT_PIN 4

// DHT type
#define DHT_TYPE DHT22

// Relay polarity: active-LOW module
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);

  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(LED_RELAY_PIN, OUTPUT);

  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  digitalWrite(LED_RELAY_PIN, RELAY_OFF);

  dht.begin();

  Serial.println("Setup complete, starting relays test");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity    = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT read failed");
  } else {
    Serial.printf("Temperature: %.1f C, Humidity: %.1f %%\n", temperature, humidity);
  }
  delay(2000);

  digitalWrite(PUMP_RELAY_PIN, RELAY_ON);
  digitalWrite(LED_RELAY_PIN, RELAY_ON);
  Serial.println("Pump and Led ON");
  delay(2000);

  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  digitalWrite(LED_RELAY_PIN, RELAY_OFF);
  Serial.println("Pump and Led OFF");
  delay(2000);
}