#include <Arduino.h>
#include <DHT.h>

// pin definitions
#define PUMP_RELAY_PIN 5
#define LED_RELAY_PIN 18
#define DHT_PIN 4
#define TRIG_PIN 17
#define ECHO_PIN 16

// DHT type
#define DHT_TYPE DHT22

// Relay polarity: active-LOW module
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// speed of sound in air, cm per microsecond
#define SOUND_SPEED_CM_US 0.0343

DHT dht(DHT_PIN, DHT_TYPE);

float readWaterLevelCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;  // sentinel value: reading failed
  }

  return (duration * SOUND_SPEED_CM_US) / 2.0;
}

void setup() {
  Serial.begin(115200);

  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);

  pinMode(LED_RELAY_PIN, OUTPUT);
  digitalWrite(LED_RELAY_PIN, RELAY_OFF);

  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  pinMode(ECHO_PIN, INPUT);

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

  float waterDistance = readWaterLevelCm();
  if (waterDistance < 0) {
    Serial.println("Water sensor read failed");
  } else {
    Serial.printf("Water distance: %.1f cm\n", waterDistance);
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