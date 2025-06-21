# Technical Context

## Core Technologies
- Platform: ESP32
- Build System: PlatformIO
- Programming Language: C++ (Arduino Framework)

## Key Libraries
### Display
- Adafruit_GFX_Library
  * Purpose: Graphics handling
  * Core functionality: Drawing, fonts, sprites
- Adafruit_SH110X
  * Purpose: OLED display driver
  * Display type: SH1106/SH1107 OLED

### Connectivity
- ESPAsyncWebServer
  * Purpose: Asynchronous web server
  * Features: WebSocket support, async handlers
- AsyncTCP
  * Purpose: Async TCP operations
  * Dependency for ESPAsyncWebServer
- PubSubClient
  * Purpose: MQTT client implementation
  * Features: Subscribe/Publish functionality

### Sensors
- DHT11 Library
  * Purpose: Temperature and humidity sensing
  * Application: Environmental monitoring
- SimpleKalmanFilter
  * Purpose: Sensor data noise reduction
  * Application: Environmental sensor readings
- Arduino_JSON
  * Purpose: JSON data handling
  * Used in: Web interface, data storage

## Technical Constraints
- ESP32 Memory Limitations
- Power consumption considerations
- Real-time processing requirements
- WiFi dependency for remote features
- MQTT broker connectivity requirements
- Removal of Blynk dependency

## Development Environment
- IDE: Visual Studio Code with PlatformIO
- Build System: PlatformIO
- Version Control: Git
- Hardware Debug: Serial monitoring

## Network Architecture
- Local WiFi connectivity
- MQTT protocol for remote data transmission
- Asynch Web Server for local interface
- JSON formatting for data exchange