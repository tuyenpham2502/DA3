# Progress

## Current Capabilities (what works)
-   ESP32 connects to Wi-Fi (STA mode) and falls back to AP mode for configuration.
-   Web server in AP mode allows configuration of Wi-Fi, MQTT server, and sensor thresholds.
-   Reads temperature, humidity (DHT11), and soil moisture sensor data.
-   Publishes sensor data to MQTT topic `esp32/sensors`.
-   Subscribes to `esp32/thresholds` to update sensor thresholds via MQTT.
-   Subscribes to `esp32/relay/control` to open/close the relay via MQTT commands.
-   Subscribes to `esp32/autoWarning/control` to enable/disable auto-warning via MQTT commands.
-   Saves configuration data to EEPROM.
-   OLED display shows various system states and sensor readings.
-   Buttons are handled for various functions (screen navigation, Wi-Fi reconnect, auto-warning toggle).
-   Relay control based on soil moisture thresholds and now also via MQTT.

## Pending Features (what's needed)
-   None for this task.

## Progress Estimate (%)
-   Overall Project: 85%
-   Current Task (MQTT Relay and AutoWarning Control, including error resolution): 100% (completed)