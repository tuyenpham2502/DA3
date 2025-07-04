# Product Context

## Project Purpose and Goals
This project aims to provide an automated environmental monitoring and control system using an ESP32, integrating sensor data (temperature, humidity, soil moisture) with a web interface and MQTT communication. The goal is to allow users to monitor environmental conditions and control connected devices (like a relay for irrigation) remotely.

## Core User Problems/Solutions
- **Problem:** Manual monitoring of environmental conditions and device control is time-consuming and inefficient.
- **Solution:** Automated sensor readings and remote control via a web server and MQTT.
- **Problem:** Difficulty in configuring Wi-Fi and MQTT settings on the device.
- **Solution:** A web-based configuration portal (AP mode) for easy setup.
- **Problem:** Lack of real-time feedback on environmental conditions.
- **Solution:** OLED display for local monitoring and MQTT for remote data publishing.

## Key Workflows
1.  **Device Boot-up:** Connects to Wi-Fi (STA mode), if fails, switches to AP mode for configuration.
2.  **Sensor Monitoring:** Continuously reads temperature, humidity, and soil moisture.
3.  **MQTT Communication:** Publishes sensor data and subscribes to control topics (e.g., for relay control).
4.  **Web Configuration (AP Mode):** Allows users to set Wi-Fi credentials, MQTT server, and thresholds.
5.  **OLED Display:** Shows current sensor values, connection status, and warning states.
6.  **Relay Control:** Automatically or manually controls a relay based on soil moisture thresholds or MQTT commands.

## Product Priorities
1.  Reliable Wi-Fi and MQTT connectivity.
2.  Accurate sensor data acquisition.
3.  User-friendly configuration interface.
4.  Robust relay control logic.
5.  Clear status indication via OLED.