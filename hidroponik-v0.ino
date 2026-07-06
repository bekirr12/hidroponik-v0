#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <time.h>
#include "secrets.h"

// pin definitions
#define PUMP_RELAY_PIN 5
#define LED_RELAY_PIN 18
#define DHT_PIN 4
#define DHT_TYPE DHT22
#define TRIG_PIN 17
#define ECHO_PIN 16

// Relay polarity: active-LOW module
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// speed of sound in air, cm per microsecond
#define SOUND_SPEED_CM_US 0.0343

// if measured distance is >= this value, the tank is considered low (unsafe)
#define WATER_SAFETY_DISTANCE_CM 16.0

#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 10800   // Turkey = GMT+3 = 3 * 3600 seconds
#define DAYLIGHT_OFFSET_SEC 0  // Turkey does not use daylight saving time

DHT dht(DHT_PIN, DHT_TYPE);

// track current relay states so we only write GPIO when something changes
bool pumpOn = false;
bool ledOn  = false;

float readWaterLevelCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  return (duration * SOUND_SPEED_CM_US) / 2.0;
}

void connectToWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(300);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.printf("WiFi connected, IP address: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed, continuing without network");
  }
}

void syncTimeWithNTP() {
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Skipping NTP sync, WiFi not connected");
    return;
  }

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.printf("Time synced: %02d:%02d:%02d\n",
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  } else {
    Serial.println("Failed to obtain time from NTP");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(LED_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  digitalWrite(LED_RELAY_PIN, RELAY_OFF);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  dht.begin();

  connectToWiFi();
  syncTimeWithNTP();

  Serial.println("Setup complete, starting safety logic test");
}

void loop() {
  // --- read sensors ---
  float temperature = dht.readTemperature();
  float humidity    = dht.readHumidity();
  float waterDistance = readWaterLevelCm();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT read failed");
  } else {
    Serial.printf("Temperature: %.1f C, Humidity: %.1f %%\n", temperature, humidity);
  }

  if (waterDistance < 0) {
    Serial.println("Water sensor read failed");
  } else {
    Serial.printf("Water distance: %.1f cm\n", waterDistance);
  }

  // decide pump state
  bool desiredPumpState = true;

  bool safetyCutoff = (waterDistance >= WATER_SAFETY_DISTANCE_CM);

  bool newPumpState;
  if (safetyCutoff) {
    newPumpState = false;  // safety always wins, overrides desired state
  } else {
    newPumpState = desiredPumpState;
  }

  // only touch the relay if the state actually changed
  if (newPumpState != pumpOn) {
    pumpOn = newPumpState;
    digitalWrite(PUMP_RELAY_PIN, pumpOn ? RELAY_ON : RELAY_OFF);
    if (safetyCutoff) {
      Serial.println("SAFETY CUTOFF: water level low, pump forced OFF");
    } else {
      Serial.printf("Pump changed to: %s\n", pumpOn ? "ON" : "OFF");
    }
  }

  // LED still runs a simple toggle for now (real schedule comes later)
  ledOn = !ledOn;
  digitalWrite(LED_RELAY_PIN, ledOn ? RELAY_ON : RELAY_OFF);
  Serial.printf("Led changed to: %s\n", ledOn ? "ON" : "OFF");

  delay(2000);
}