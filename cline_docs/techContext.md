# Tech Context

## Core Technologies and Frameworks
-   **Microcontroller:** ESP32
-   **Programming Language:** C++ (Arduino framework)
-   **Wi-Fi:** `WiFi.h`, `WiFiClient.h`, `WebServer.h`, `ESPmDNS.h`
-   **Asynchronous Web Server:** `AsyncTCP.h`, `ESPAsyncWebServer.h`
-   **MQTT Client:** `PubSubClient.h`
-   **JSON Parsing:** `Arduino_JSON.h`
-   **EEPROM:** `EEPROM.h` for non-volatile storage
-   **Sensors:**
    -   DHT11 (`DHT.h`) for temperature and humidity
    -   Analog soil moisture sensor
-   **OLED Display:** `SPI.h`, `Wire.h`, `Adafruit_GFX.h`, `Adafruit_SH110X.h`
-   **Filtering:** `SimpleKalmanFilter.h` (though currently commented out for `humiValue` and `tempValue`)
-   **Real-time Operating System (RTOS):** FreeRTOS (via `TaskHandle_t`, `xTaskCreatePinnedToCore`) for multi-tasking.

## Integration Patterns
-   **Sensor to Microcontroller:** Direct pin connections (digital for DHT11, analog for soil moisture).
-   **Microcontroller to Actuators:** Direct pin control (digital for LED, Buzzer, Relay).
-   **Web Server Integration:** `ESPAsyncWebServer` handles HTTP requests and serves HTML/JSON.
-   **MQTT Integration:** `PubSubClient` connects to an MQTT broker, publishes sensor data, and subscribes to control topics.
-   **EEPROM Integration:** Standard EEPROM read/write operations for configuration persistence.

## Technical Constraints
-   **Memory:** Limited RAM and Flash memory on ESP32, requiring efficient code and data structures.
-   **Processing Power:** Dual-core, but still a microcontroller, so complex computations should be minimized.
-   **Network Reliability:** Dependent on Wi-Fi signal strength and MQTT broker availability.
-   **EEPROM Write Cycles:** EEPROM has a limited number of write cycles, so frequent writes should be avoided (though `clearEeprom` and `writeEEPROM` are called on every configuration change, which might be a concern for very frequent changes).
-   **Real-time Performance:** FreeRTOS helps, but critical timing might still be affected by network operations or complex display updates.

## Development Environment
-   **PlatformIO:** Implied by the project structure (`platformio.ini`, `src/main.ino`).
-   **Arduino IDE Compatibility:** The `.ino` file suggests compatibility with the Arduino IDE environment.