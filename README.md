# Hidroponik v0

A learning-focused firmware project for an ESP32-S3 based smart planter (hydroponic system). This is a from-scratch, step-by-step rebuild used to understand embedded firmware fundamentals before moving to a more advanced, FreeRTOS-based architecture.

## Purpose

The system has two equally important goals:

1. **Automated plant care** — water the plant (pump) and light it (LED) on a schedule, with a safety cutoff if the water tank runs low.
2. **Remote monitoring** — read sensor data (temperature, humidity, water level) and send it to a cloud service so the system can be observed and controlled remotely.

## System requirements

### Hardware
- ESP32-S3 (Wi-Fi enabled microcontroller)
- DHT22 — temperature and humidity sensor
- HC-SR04 — ultrasonic sensor for water level measurement
- 2x relay module — one for the water pump, one for the LED grow light
- 5V/12V power supply as required by the pump/LED hardware

### Software
- Arduino IDE (or PlatformIO)
- ESP32 board support package
- DHT sensor library
- Wi-Fi connectivity (built into ESP32 core)

## v0 algorithm

This is the first, simplified version of the firmware logic. No FreeRTOS tasks yet — everything runs in the classic Arduino `loop()`, using non-blocking timing (`millis()`) instead of `delay()`, so sensor reading, relay control, and network communication can all happen without freezing the system.

```
setup():
    - start serial communication
    - configure pump/LED relay pins as OUTPUT, set to OFF
    - initialize sensors (DHT22, HC-SR04)
    - connect to Wi-Fi

loop():
    1. Read sensors (temperature, humidity, water level)
    2. Check water level:
         - if critical (tank low) -> force pump OFF (safety, overrides everything else)
         - otherwise -> decide pump/LED state based on schedule
    3. Update relay outputs (only writes GPIO if the state actually changed)
    4. Periodically send sensor data to the cloud (Wi-Fi, HTTP)
    5. Repeat (loop() cycles continuously, timed with millis())
```

**Key design decision:** the safety cutoff (water level check) always takes priority over the schedule. If the tank is low, the pump is forced off regardless of what the schedule says.

## Roadmap (future versions)

- v0 → current: single `.ino` file, classic `loop()`, basic Wi-Fi
- v1: split into `.h`/`.cpp` modules by responsibility (sensors, relays, network)
- v2: move to a FreeRTOS two-task architecture (control task + network task) with a mutex-protected shared state, matching the production Florene firmware design

## Development workflow

Every change follows this cycle:
1. Write/modify code
2. Upload and test on hardware
3. `git add .` → `git status` (verify nothing sensitive, like `secrets.h`, is staged) → `git commit -m "..."` → `git push`

Code, comments, and commit messages are written in English.
