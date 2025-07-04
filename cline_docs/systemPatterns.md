# System Patterns

## High-Level Architecture
The system is built around an ESP32 microcontroller, acting as both a Wi-Fi client (STA mode) and an Access Point (AP mode) for configuration. It integrates various sensors (DHT11 for temperature/humidity, analog soil moisture sensor) and actuators (relay, buzzer, LED, OLED display). Communication is handled via Wi-Fi, a local web server (AsyncWebServer), and MQTT (PubSubClient). FreeRTOS tasks are used for concurrent operations like sensor reading, OLED display updates, and button handling.

## Technical Patterns and Data Flow
-   **Sensor Data Acquisition:** DHT11 and soil moisture sensors read data periodically.
-   **Data Processing:** Sensor readings are processed (e.g., Kalman filter for smoothing, mapping for soil moisture).
-   **MQTT Communication:**
    -   **Publishing:** Processed sensor data is formatted as JSON and published to `esp32/sensors` topic.
    -   **Subscribing:** Subscribes to `esp32/thresholds` for configuration updates and will subscribe to a new topic for relay control.
    -   **Callback:** `mqttCallback` handles incoming MQTT messages, parses JSON, and updates system parameters (e.g., thresholds, relay state).
-   **Web Server (AP Mode):**
    -   Serves an HTML page (`index_html.h`) for configuration.
    -   Handles GET requests for initial data (`/data_before`).
    -   Handles POST requests (`/post_data`) to receive and save configuration data (Wi-Fi, MQTT, thresholds) to EEPROM.
-   **EEPROM:** Used for persistent storage of Wi-Fi credentials, MQTT server, and sensor thresholds.
-   **OLED Display:** Updates dynamically to show connection status, sensor values, and warning states.
-   **Button Handling:** Interrupt-driven button presses trigger various actions (e.g., screen changes, Wi-Fi reconnect, auto-warning toggle).
-   **FreeRTOS Tasks:** Multiple tasks run concurrently to manage different functionalities (DHT11, Soil Moisture, OLED, Buttons, Auto Warning).

## Key Technical Decisions
-   **ESP32:** Chosen for its Wi-Fi capabilities and dual-core processor, suitable for concurrent tasks.
-   **AsyncWebServer:** For non-blocking web server operations, improving responsiveness.
-   **PubSubClient:** For lightweight MQTT communication.
-   **EEPROM:** For non-volatile storage of configuration, allowing settings to persist across reboots.
-   **FreeRTOS:** To manage multiple concurrent operations efficiently.
-   **JSON:** For structured data exchange over MQTT and HTTP.

## Operational Patterns and Error Handling
-   **Wi-Fi Connection:** Attempts STA connection; if unsuccessful, falls back to AP mode for configuration.
-   **MQTT Connection:** Attempts reconnection if disconnected.
-   **Sensor Reading Errors:** Handles `NaN` values from DHT11.
-   **EEPROM:** Clears EEPROM before writing new configuration to prevent data corruption.
-   **System Restart:** `ESP.restart()` is used after configuration updates to apply changes.