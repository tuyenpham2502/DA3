/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-async-web-server-espasyncwebserver-library/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/
#define BLYNK_PRINT Serial
// You should get Auth Token in the Blynk App.
#define BLYNK_TEMPLATE_ID           "BANLINHKIEN"
#define BLYNK_TEMPLATE_NAME         "BANLINHKIEN"
char BLYNK_AUTH_TOKEN[32]   =   "";
// Import required libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SimpleKalmanFilter.h>
#include "index_html.h"
#include "data_config.h"
#include <EEPROM.h>
#include <Arduino_JSON.h>
// Include PubSubClient library for MQTT
#include <PubSubClient.h>

// MQTT broker details
const int mqtt_port = 1883; // MQTT port
WiFiClient espClient;
PubSubClient client(espClient);

String device_id = "";
void loadDeviceId() {
  // Gán cứng device_id theo yêu cầu
  device_id = "dv-01";
}

// Define ENABLE, DISABLE, and RELAY here to ensure they are globally accessible
#define ENABLE    1
#define DISABLE   0
#define RELAY         25

typedef enum {
  SCREEN0,
  SCREEN1,
  SCREEN2,
  SCREEN3,
  SCREEN4,
  SCREEN5,
  SCREEN6,
  SCREEN7,
  SCREEN8,
  SCREEN9,
  SCREEN10,
  SCREEN11,
  SCREEN12,
  SCREEN13,
  SCREEN14,
  SCREEN15,
  SCREEN_DUST,
  SCREEN_MQ135
}SCREEN;
int screenOLED = SCREEN0;
// #define SCREEN_MQ135 14  // Xóa dòng này để tránh trùng giá trị enum

bool enableShow = DISABLE;
bool autoWarning = DISABLE;

// Function to connect to MQTT broker
void connectMQTT() {
  client.setCallback(mqttCallback); // Set MQTT callback
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(device_id.c_str())) {
      Serial.println("connected");
      client.subscribe(("esp32/" + device_id + "/thresholds").c_str());
      client.subscribe(("esp32/" + device_id + "/relay/control").c_str());
      client.subscribe(("esp32/" + device_id + "/autoWarning/control").c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

// Function to publish sensor data
void publishSensorData() {
  extern float tempValue;
  extern int humiValue;
  extern int dustValue;
  extern int mq135Value;
  extern float mq135Rs;
  extern float mq135GasRatio;
  String payload = String("{\"temperature\": ") + String(tempValue) +
                   String(", \"humidity\": ") + String(humiValue) +
                   String(", \"dust\": ") + String(dustValue) +
                   String(", \"mq135_ratio\": ") + String(mq135GasRatio, 2) + String("}");
  client.publish(("esp32/" + device_id + "/sensors").c_str(), payload.c_str());
}

// Function to publish autoWarning and RELAY status
void publishStatusData() {
  // Read current value from variable to ensure we publish the correct value
  int currentAutoWarning = autoWarning;
  Serial.print("Publishing status - current autoWarning value: ");
  Serial.println(currentAutoWarning);
  
  String payload = String("{\"autoWarning\": ") + String(currentAutoWarning) + String(", \"relayState\": ") + String(digitalRead(RELAY)) + String("}");
  Serial.print("Publishing status to topic: esp32/");
  Serial.print(device_id);
  Serial.println("/status");
  Serial.print("Payload: ");
  Serial.println(payload);
  bool published = client.publish(("esp32/" + device_id + "/status").c_str(), payload.c_str());
  if (published) {
    Serial.println("Status published successfully");
  } else {
    Serial.println("Failed to publish status");
  }
}

// Function to publish autoWarning state immediately when changed
void publishAutoWarning() {
  // Read current value from variable to ensure we publish the correct value
  int currentValue = autoWarning;
  Serial.print("Publishing autoWarning - current variable value: ");
  Serial.println(currentValue);
  
  String payload = String("{\"autoWarning\": ") + String(currentValue) + String("}");
  Serial.print("Publishing autoWarning to topic: esp32/");
  Serial.print(device_id);
  Serial.println("/autoWarning/state");
  Serial.print("Payload: ");
  Serial.println(payload);
  bool published = client.publish(("esp32/" + device_id + "/autoWarning/state").c_str(), payload.c_str());
  if (published) {
    Serial.print("Published autoWarning successfully: ");
    Serial.println(currentValue);
  } else {
    Serial.println("Failed to publish autoWarning");
  }
}

// Function to publish threshold data
void publishThresholdData() {
  // Debug: Print current threshold values before publishing
  Serial.println("Publishing threshold data:");
  Serial.print("  tempThreshold1 = "); Serial.println(EtempThreshold1);
  Serial.print("  tempThreshold2 = "); Serial.println(EtempThreshold2);
  Serial.print("  humiThreshold1 = "); Serial.println(EhumiThreshold1);
  Serial.print("  humiThreshold2 = "); Serial.println(EhumiThreshold2);
  Serial.print("  dustThreshold1 = "); Serial.println(EdustThreshold1);
  Serial.print("  dustThreshold2 = "); Serial.println(EdustThreshold2);
  Serial.print("  mq135Threshold1 = "); Serial.println(Emq135Threshold1);
  Serial.print("  mq135Threshold2 = "); Serial.println(Emq135Threshold2);
  Serial.print("  typePlant = "); Serial.println(EtypePlant);
  
  String payload = String("{\"tempThreshold1\": ") + String(EtempThreshold1) +
                   String(", \"tempThreshold2\": ") + String(EtempThreshold2) +
                   String(", \"humiThreshold1\": ") + String(EhumiThreshold1) +
                   String(", \"humiThreshold2\": ") + String(EhumiThreshold2) +
                   String(", \"dustThreshold1\": ") + String(EdustThreshold1) +
                   String(", \"dustThreshold2\": ") + String(EdustThreshold2) +
                   String(", \"mq135Threshold1\": ") + String(Emq135Threshold1) +
                   String(", \"mq135Threshold2\": ") + String(Emq135Threshold2) +
                   String(", \"typePlant\": ") + String(EtypePlant) + String("}");
  
  Serial.print("Publishing payload: ");
  Serial.println(payload);
  
  // Publish to separate topic to avoid triggering callback on the same topic we subscribe to
  bool published = client.publish(("esp32/" + device_id + "/thresholds/current").c_str(), payload.c_str());
  if (published) {
    Serial.println("Threshold data published successfully");
  } else {
    Serial.println("Failed to publish threshold data");
  }
}

#include "icon.h"

// Helper function to safely parse integer from JSON
int parseJsonInt(JSONVar obj, const char* key) {
  if (!obj.hasOwnProperty(key)) return -1;
  JSONVar val = obj[key];
  // Try to parse as double first (more reliable), then convert to int
  double dval = (double)val;
  // Round to nearest integer
  int ival;
  if (dval >= 0) {
    ival = (int)(dval + 0.5);
  } else {
    ival = (int)(dval - 0.5);
  }
  return ival;
}

// Helper function to update threshold value
bool updateThresholdValue(int& threshold, int val, const char* name) {
  if (val >= 0 && val <= 255) {
    threshold = val;
    Serial.print("Updated ");
    Serial.print(name);
    Serial.print(" = ");
    Serial.println(threshold);
    return true;
  } else {
    Serial.print("Invalid ");
    Serial.print(name);
    Serial.print(" value (must be 0-255): ");
    Serial.println(val);
    return false;
  }
}

// MQTT callback function
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Received message: ");
  Serial.println(message);
  JSONVar myObject = JSON.parse(message);
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Invalid JSON received");
    return;
  }
  if (String(topic) == ("esp32/" + device_id + "/thresholds")) {
    Serial.println("Processing thresholds update from MQTT...");
    bool updated = false;
    
    // Support both old format (tempThreshold1/tempThreshold2) and new format (temperatureMin/temperatureMax)
    // Temperature
    if (myObject.hasOwnProperty("temperatureMin") || myObject.hasOwnProperty("tempThreshold1")) {
      int val = myObject.hasOwnProperty("temperatureMin") ? 
                parseJsonInt(myObject, "temperatureMin") : 
                parseJsonInt(myObject, "tempThreshold1");
      Serial.print("Parsed temperatureMin/tempThreshold1 from MQTT: ");
      Serial.println(val);
      if (updateThresholdValue(EtempThreshold1, val, "tempThreshold1")) {
        updated = true;
      }
    }
    
    if (myObject.hasOwnProperty("temperatureMax") || myObject.hasOwnProperty("tempThreshold2")) {
      int val = myObject.hasOwnProperty("temperatureMax") ? 
                parseJsonInt(myObject, "temperatureMax") : 
                parseJsonInt(myObject, "tempThreshold2");
      Serial.print("Parsed temperatureMax/tempThreshold2 from MQTT: ");
      Serial.println(val);
      if (updateThresholdValue(EtempThreshold2, val, "tempThreshold2")) {
        updated = true;
      }
    }
    
    // Humidity
    if (myObject.hasOwnProperty("humidityMin") || myObject.hasOwnProperty("humiThreshold1")) {
      int val = myObject.hasOwnProperty("humidityMin") ? 
                parseJsonInt(myObject, "humidityMin") : 
                parseJsonInt(myObject, "humiThreshold1");
      Serial.print("Parsed humidityMin/humiThreshold1 from MQTT: ");
      Serial.println(val);
      if (updateThresholdValue(EhumiThreshold1, val, "humiThreshold1")) {
        updated = true;
      }
    }
    
    if (myObject.hasOwnProperty("humidityMax") || myObject.hasOwnProperty("humiThreshold2")) {
      int val = myObject.hasOwnProperty("humidityMax") ? 
                parseJsonInt(myObject, "humidityMax") : 
                parseJsonInt(myObject, "humiThreshold2");
      Serial.print("Parsed humidityMax/humiThreshold2 from MQTT: ");
      Serial.println(val);
      if (updateThresholdValue(EhumiThreshold2, val, "humiThreshold2")) {
        updated = true;
      }
    }
    
    // Dust
    if (myObject.hasOwnProperty("dustMin") || myObject.hasOwnProperty("dustThreshold1")) {
      int val = myObject.hasOwnProperty("dustMin") ? 
                parseJsonInt(myObject, "dustMin") : 
                parseJsonInt(myObject, "dustThreshold1");
      Serial.print("Parsed dustMin/dustThreshold1 from MQTT: ");
      Serial.println(val);
      if (updateThresholdValue(EdustThreshold1, val, "dustThreshold1")) {
        updated = true;
      }
    }
    
    if (myObject.hasOwnProperty("dustMax") || myObject.hasOwnProperty("dustThreshold2")) {
      int val = myObject.hasOwnProperty("dustMax") ? 
                parseJsonInt(myObject, "dustMax") : 
                parseJsonInt(myObject, "dustThreshold2");
      Serial.print("Parsed dustMax/dustThreshold2 from MQTT: ");
      Serial.println(val);
      if (updateThresholdValue(EdustThreshold2, val, "dustThreshold2")) {
        updated = true;
      }
    }
    
    // MQ135
    if (myObject.hasOwnProperty("mq135Min") || myObject.hasOwnProperty("mq135Threshold1")) {
      int val = myObject.hasOwnProperty("mq135Min") ? 
                parseJsonInt(myObject, "mq135Min") : 
                parseJsonInt(myObject, "mq135Threshold1");
      Serial.print("Parsed mq135Min/mq135Threshold1 from MQTT: ");
      Serial.println(val);
      if (updateThresholdValue(Emq135Threshold1, val, "mq135Threshold1")) {
        updated = true;
      }
    }
    
    if (myObject.hasOwnProperty("mq135Max") || myObject.hasOwnProperty("mq135Threshold2")) {
      int val = myObject.hasOwnProperty("mq135Max") ? 
                parseJsonInt(myObject, "mq135Max") : 
                parseJsonInt(myObject, "mq135Threshold2");
      Serial.print("Parsed mq135Max/mq135Threshold2 from MQTT: ");
      Serial.println(val);
      if (updateThresholdValue(Emq135Threshold2, val, "mq135Threshold2")) {
        updated = true;
      }
    }
    
    if (updated) {
      Serial.println("All thresholds after update (before saving):");
      printValueSetup();
      writeEEPROM(); // Save updated thresholds to EEPROM
      Serial.println("Thresholds updated and saved to EEPROM via MQTT");
      Serial.println("Device will continue running with new thresholds (no restart needed)");
      
      // Publish updated thresholds to confirm the change
      if (client.connected()) {
        publishThresholdData();
      }
    } else {
      Serial.println("No valid thresholds to update");
    }
  }
  else if (String(topic) == ("esp32/" + device_id + "/autoWarning/control")) {
    Serial.println("Received autoWarning control message");
    int state = -1;
    
    // Backend sends "autoWarning" field with value 0 or 1
    // Also support legacy format "autoWarningState" for backward compatibility
    if (myObject.hasOwnProperty("autoWarning")) {
      state = parseJsonInt(myObject, "autoWarning");
      Serial.print("Parsed autoWarning from MQTT: ");
      Serial.println(state);
    } else if (myObject.hasOwnProperty("autoWarningState")) {
      state = parseJsonInt(myObject, "autoWarningState");
      Serial.print("Parsed autoWarningState from MQTT (legacy format): ");
      Serial.println(state);
    } else {
      Serial.println("Missing 'autoWarning' or 'autoWarningState' in MQTT payload");
      Serial.print("Available keys in message: ");
      // Print available keys for debugging
      JSONVar keys = myObject.keys();
      for (int i = 0; i < keys.length(); i++) {
        Serial.print((const char*)keys[i]);
        if (i < keys.length() - 1) Serial.print(", ");
      }
      Serial.println();
      return;
    }
    
    // Validate and update autoWarning state
    if (state == 1 || state == 0) {
      int oldState = autoWarning;
      autoWarning = state;
      Serial.print("Writing autoWarning to EEPROM: ");
      Serial.println(autoWarning);
      EEPROM.write(210, (byte)(autoWarning & 0xFF));
      EEPROM.commit();
      
      // Verify by reading back
      int readBack = (int)((unsigned char)EEPROM.read(210));
      Serial.print("Read back autoWarning from EEPROM: ");
      Serial.println(readBack);
      
      Serial.print("Auto Warning changed from ");
      Serial.print(oldState == 1 ? "ENABLE" : "DISABLE");
      Serial.print(" to ");
      Serial.println(autoWarning == 1 ? "ENABLE" : "DISABLE");
      
      enableShow = DISABLE;
      if (autoWarning == 0) screenOLED = SCREEN11;
      else screenOLED = SCREEN10;
      
      // Publish autoWarning state immediately when changed via MQTT
      if (client.connected()) {
        publishAutoWarning();
      }
    } else {
      Serial.print("Invalid autoWarning value received (must be 0 or 1, got: ");
      Serial.print(state);
      Serial.println(")");
    }
  }
  else if (String(topic) == ("esp32/" + device_id + "/relay/control")) {
    if (myObject.hasOwnProperty("relayState")) {
      int relayState = (int)myObject["relayState"];
      if (relayState == 1) {
        digitalWrite(RELAY, ENABLE);
        Serial.println("Relay ON via MQTT");
      } else if (relayState == 0) {
        digitalWrite(RELAY, DISABLE);
        Serial.println("Relay OFF via MQTT");
      } else {
        Serial.println("Invalid relayState value received");
      }
    } else {
      Serial.println("Missing 'relayState' in MQTT payload");
    }
  } 
}

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// ---------------------- Khai báo cho OLED 1.3 --------------------------
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3C //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G oled = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define OLED_SDA      21
#define OLED_SCL      22


#define SAD    0
#define NORMAL 1
#define HAPPY  2
int warningTempState = SAD;
int warningHumiState = NORMAL;
int warningDustState = NORMAL;
int warningMQ135State = NORMAL;

// --------------------- Cảm biến DHT11 ---------------------
#include "DHT.h"
#define DHT11_PIN         26
#define DHTTYPE DHT11
DHT dht(DHT11_PIN, DHTTYPE);
float tempValue = 30;
int humiValue   = 60;
SimpleKalmanFilter tempfilter(2, 2, 0.001);
SimpleKalmanFilter humifilter(2, 2, 0.001);
bool dht11ReadOK = true;

// -------------------- Khai báo cảm biến bụi --------------
#include <GP2Y1010AU0F.h>
#define DUST_TRIG             23
#define DUST_ANALOG           36
GP2Y1010AU0F dustSensor(DUST_TRIG, DUST_ANALOG);
SimpleKalmanFilter dustfilter(2, 2, 0.001);
int dustValue = 10;

// -------------------- Khai báo cảm biến MQ135, MQ3, MQ5 -----------------
#define MQ135_PIN 39
#define MQ3_PIN   38
#define MQ5_PIN   37
int mq135Value = 0;
int mq3Value = 0;
int mq5Value = 0;
float mq135GasRatio = 0.0;  // Tỷ lệ Rs/R0 - chỉ số chất lượng không khí (Ratio < 1.0 = tốt, > 1.0 = có khí độc)
const float MQ135_RL = 10000.0; // Điện trở tải 10k Ohm
const float MQ135_VCC = 3.3;    // Điện áp nguồn 3.3V
const float MQ135_R0 = 10000.0; // Điện trở baseline trong không khí sạch (cần hiệu chuẩn)
TaskHandle_t TaskMQ135Sensor_handle = NULL;

// Khai báo LED
#define LED           33
// Khai báo BUZZER
#define BUZZER        2

uint32_t timeCountBuzzerWarning = 0;
#define TIME_BUZZER_WARNING     300  //thời gian cảnh báo bằng còi (đơn vị giây)
//-------------------- Khai báo Button-----------------------
#include "mybutton.h"
#define BUTTON_DOWN_PIN   34
#define BUTTON_UP_PIN     35
#define BUTTON_SET_PIN    32

#define BUTTON1_ID  1
#define BUTTON2_ID  2
#define BUTTON3_ID  3
Button buttonSET;
Button buttonDOWN;
Button buttonUP;
void button_press_short_callback(uint8_t button_id);
void button_press_long_callback(uint8_t button_id);
//------------------------------------------------------------
TaskHandle_t TaskButton_handle      = NULL;
TaskHandle_t TaskOLEDDisplay_handle = NULL;
TaskHandle_t TaskDHT11_handle = NULL;
TaskHandle_t TaskDustSensor_handle = NULL;
TaskHandle_t TaskAutoWarning_handle = NULL;
void setup(){
  Serial.begin(115200);
  // Đọc data setup từ eeprom
  EEPROM.begin(512);
  readEEPROM();
  loadDeviceId();
  client.setServer(Emqtt_server.c_str(), mqtt_port);
    // Khởi tạo LED
  pinMode(LED, OUTPUT);
  // Khởi tạo RELAY
  pinMode(RELAY, OUTPUT);
  // Khởi tạo BUZZER
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, DISABLE);
  // Khởi tạo OLED
  oled.begin(i2c_Address, true);
  oled.setTextSize(2);
  oled.setTextColor(SH110X_WHITE);
  // In device_id ra Serial để backend dễ lấy
  Serial.print("DEVICE_ID: ");
  Serial.println(device_id);

  // Khởi tạo DHT11
  dht.begin();
  // Khai báo cảm biến bụi
  dustSensor.begin();
  // ---------- Đọc giá trị AutoWarning trong EEPROM ----------------
  autoWarning = (int)((unsigned char)EEPROM.read(210));
  Serial.print("Read autoWarning from EEPROM in setup: ");
  Serial.println(autoWarning);
  // Khởi tạo MQ135
  pinMode(MQ135_PIN, INPUT);
  xTaskCreatePinnedToCore(TaskMQ135Sensor, "TaskMQ135Sensor", 1024*8, NULL, 10, &TaskMQ135Sensor_handle, 1);
  // Khởi tạo nút nhấn
  pinMode(BUTTON_SET_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  button_init(&buttonSET, BUTTON_SET_PIN, BUTTON1_ID);
  button_init(&buttonUP, BUTTON_UP_PIN, BUTTON2_ID);
  button_init(&buttonDOWN,   BUTTON_DOWN_PIN,   BUTTON3_ID);
  button_pressshort_set_callback((void *)button_press_short_callback);
  button_presslong_set_callback((void *)button_press_long_callback);

  xTaskCreatePinnedToCore(TaskButton,          "TaskButton" ,          1024*10 ,  NULL,  20 ,  &TaskButton_handle       , 1);
  xTaskCreatePinnedToCore(TaskOLEDDisplay,     "TaskOLEDDisplay" ,     1024*16 ,  NULL,  20 ,  &TaskOLEDDisplay_handle  , 1);
  xTaskCreatePinnedToCore(TaskDHT11,           "TaskDHT11" ,           1024*10 ,  NULL,  10 ,  &TaskDHT11_handle  , 1);
  xTaskCreatePinnedToCore(TaskDustSensor,      "TaskDustSensor" ,      1024*10 ,  NULL,  10 ,  &TaskDustSensor_handle  , 1);
  xTaskCreatePinnedToCore(TaskAutoWarning,     "TaskAutoWarning" ,     1024*10 ,  NULL,  10  , &TaskAutoWarning_handle ,  1);

  // Kết nối wifi
  connectSTA();
}

unsigned long lastThresholdPublish = 0;
const unsigned long THRESHOLD_PUBLISH_INTERVAL = 30000; // Publish threshold every 30 seconds

unsigned long lastAutoWarningPublish = 0;
const unsigned long AUTO_WARNING_PUBLISH_INTERVAL = 60000; // Publish autoWarning every 60 seconds

void loop() {
  if (!client.connected()) {
    connectMQTT();
    screenOLED = SCREEN5; // Screen for failed MQTT connection
    lastAutoWarningPublish = 0; // Reset timer when disconnected
  } else {
    screenOLED = SCREEN7; // Screen for successful MQTT connection
  }
  client.loop();
  publishSensorData();
  publishStatusData(); // Publish autoWarning and RELAY status
  
  // Publish threshold data every 30 seconds (thresholds don't change often)
  unsigned long now = millis();
  if (now - lastThresholdPublish >= THRESHOLD_PUBLISH_INTERVAL) {
    publishThresholdData();
    lastThresholdPublish = now;
  }
  
  // Publish autoWarning state periodically (every 60 seconds)
  if (client.connected() && (now - lastAutoWarningPublish >= AUTO_WARNING_PUBLISH_INTERVAL || lastAutoWarningPublish == 0)) {
    publishAutoWarning();
    lastAutoWarningPublish = now;
  }
  
  delay(5000);
}

//--------------------Task đo DHT11 ---------------
void TaskDHT11(void *pvParameters) { 
    //delay(10000);
    while(1) {
      int humi =  dht.readHumidity();
      float temp =  dht.readTemperature();
      if (isnan(humi) || isnan(temp) ) {
          Serial.println(F("Failed to read from DHT sensor!"));
          dht11ReadOK = false;
      }
      else if(humi <= 100 && temp < 100) {
          dht11ReadOK = true;
          // humiValue = humifilter.updateEstimate(humi);
          // tempValue = tempfilter.updateEstimate(temp);
          humiValue = humi;
          tempValue = temp;

          Serial.print(F("Humidity: "));
          Serial.print(humiValue);
          Serial.print(F("%  Temperature: "));
          Serial.print(tempValue);
          Serial.print(F("°C "));
          Serial.println();

          if(tempValue < EtempThreshold1 || tempValue > EtempThreshold2) 
            warningTempState = NORMAL;
          else
            warningTempState = HAPPY;
          if(humiValue < EhumiThreshold1 || tempValue > EhumiThreshold2) 
            warningHumiState = NORMAL;
          else
            warningHumiState = HAPPY;
      }
      delay(3000);
    }
}

void TaskDustSensor(void *pvParameters) {
    while(1) {
      dustValue = dustSensor.read();
      dustValue = dustValue - 20;
      if(dustValue <= 0)  dustValue = 0;
      //dustValue = dustfilter.updateEstimate(dustValue);
      Serial.print("Dust Density = ");
      Serial.print(dustValue);
      Serial.println(" ug/m3");

      if(dustValue < EdustThreshold1 || dustValue > EdustThreshold2) 
        warningDustState = NORMAL;
      else
        warningDustState = HAPPY;

      delay(3000);
    }
}


// Xóa 1 ô hình chữ nhật từ tọa độ (x1,y1) đến (x2,y2)
void clearRectangle(int x1, int y1, int x2, int y2) {
   for(int i = y1; i < y2; i++) {
     oled.drawLine(x1, i, x2, i, 0);
   }
}

void clearOLED(){
  oled.clearDisplay();
  oled.display();
}

int countSCREEN9 = 0;
// Task hiển thị OLED
void TaskOLEDDisplay(void *pvParameters) {
  while (1) {
      switch(screenOLED) {
        case SCREEN0: // Hiệu ứng khởi động
          for(int j = 0; j < 3; j++) {
            for(int i = 0; i < FRAME_COUNT_loadingOLED; i++) {
              oled.clearDisplay();
              oled.drawBitmap(32, 0, loadingOLED[i], FRAME_WIDTH_64, FRAME_HEIGHT_64, 1);
              oled.display();
              delay(FRAME_DELAY/4);
            }
          }
          screenOLED = SCREEN4;
          break;
        case SCREEN_DUST:   // Hiển thị bụi
          for(int j = 0; j < 2 && enableShow == ENABLE; j++) {
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(0, 20);
            oled.print("Bui min PM2.5: ");
            oled.setTextSize(2);
            oled.setCursor(0, 32);
            oled.print(dustValue);
            oled.setTextSize(1);
            oled.print(" ug/m3");
          
            for(int i = 0; i < FRAME_COUNT_face1OLED && enableShow == ENABLE; i++) {
                  clearRectangle(96, 0, 128, 64);
                  if(warningDustState == SAD)
                    oled.drawBitmap(96, 20, face1OLED[i], 32, 32, 1);
                  else if(warningDustState == NORMAL)
                    oled.drawBitmap(96, 20, face2OLED[i], 32, 32, 1);
                  else if(warningDustState == HAPPY)
                    oled.drawBitmap(96, 20, face3OLED[i], 32, 32, 1);
                  oled.display();
                  delay(FRAME_DELAY);
            }
            oled.display();
            delay(100);
          }
          if( enableShow == ENABLE)
            screenOLED = SCREEN1;
          break;
        case SCREEN_MQ135:   // Hiển thị MQ135 Gas Ratio
          for(int j = 0; j < 2 && enableShow == ENABLE; j++) {
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(0, 20);
            oled.print("MQ135 Ratio:");
            oled.setTextSize(2);
            oled.setCursor(0, 32);
            oled.print(mq135GasRatio, 2);
          
            for(int i = 0; i < FRAME_COUNT_face1OLED && enableShow == ENABLE; i++) {
                  clearRectangle(96, 0, 128, 64);
                  if(warningMQ135State == SAD)
                    oled.drawBitmap(96, 20, face1OLED[i], 32, 32, 1);
                  else if(warningMQ135State == NORMAL)
                    oled.drawBitmap(96, 20, face2OLED[i], 32, 32, 1);
                  else if(warningMQ135State == HAPPY)
                    oled.drawBitmap(96, 20, face3OLED[i], 32, 32, 1);
                  oled.display();
                  delay(FRAME_DELAY);
            }
            oled.display();
            delay(100);
          }
          if( enableShow == ENABLE)
            screenOLED = SCREEN1;
          break;
        case SCREEN1:   // Hiển thị nhiệt độ
          for(int j = 0; j < 2 && enableShow == ENABLE; j++) {

            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(0, 20);
            oled.print("Nhiet do : ");
            oled.setTextSize(2);
            oled.setCursor(0, 32);
            //if(dht11ReadOK == true)
              oled.print(tempValue,1); 
            //else
            //  oled.print("NaN"); 
            oled.drawCircle(54, 32, 3,SH110X_WHITE); 
            oled.print(" C"); 
          
            for(int i = 0; i < FRAME_COUNT_face1OLED && enableShow == ENABLE; i++) {
                  clearRectangle(96, 0, 128, 64);
                  if(warningTempState == SAD)
                    oled.drawBitmap(96, 20, face1OLED[i], 32, 32, 1);
                  else if(warningTempState == NORMAL)
                    oled.drawBitmap(96, 20, face2OLED[i], 32, 32, 1);
                  else if(warningTempState == HAPPY)
                    oled.drawBitmap(96, 20, face3OLED[i], 32, 32, 1);
                  oled.display();
                  delay(FRAME_DELAY);
            }
            oled.display();
            delay(100);
          }
          if( enableShow == ENABLE)
            screenOLED = SCREEN2;
          break;
        case SCREEN2:   // Hiển thị độ ẩm
          for(int j = 0; j < 2 && enableShow == ENABLE; j++) {
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(0, 20);
            oled.print("Do am khong khi: ");
            oled.setTextSize(2);
            oled.setCursor(0, 32);
            //if(dht11ReadOK == true)
              oled.print(humiValue); 
            //else
            //  oled.print("NaN");
            oled.print(" %"); 
            
            for(int i = 0; i < FRAME_COUNT_face1OLED && enableShow == ENABLE; i++) {
                  clearRectangle(96, 0, 128, 64);
                  if(warningHumiState == SAD)
                    oled.drawBitmap(96, 20, face1OLED[i], 32, 32, 1);
                  else if(warningHumiState == NORMAL)
                    oled.drawBitmap(96, 20, face2OLED[i], 32, 32, 1);
                  else if(warningHumiState == HAPPY)
                    oled.drawBitmap(96, 20, face3OLED[i], 32, 32, 1);
                  oled.display();
                  delay(FRAME_DELAY);
            }
            oled.display();
            delay(100);
          }
          if( enableShow == ENABLE)
            screenOLED = SCREEN14;
          break;
        case SCREEN14:  // Hiển thị Bụi trong chu trình chính
          for(int j = 0; j < 2 && enableShow == ENABLE; j++) {
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(0, 20);
            oled.print("Bui min PM2.5: ");
            oled.setTextSize(2);
            oled.setCursor(0, 32);
            oled.print(dustValue);
            oled.setTextSize(1);
            oled.print(" ug/m3");
          
            for(int i = 0; i < FRAME_COUNT_face1OLED && enableShow == ENABLE; i++) {
                  clearRectangle(96, 0, 128, 64);
                  if(warningDustState == SAD)
                    oled.drawBitmap(96, 20, face1OLED[i], 32, 32, 1);
                  else if(warningDustState == NORMAL)
                    oled.drawBitmap(96, 20, face2OLED[i], 32, 32, 1);
                  else if(warningDustState == HAPPY)
                    oled.drawBitmap(96, 20, face3OLED[i], 32, 32, 1);
                  oled.display();
                  delay(FRAME_DELAY);
            }
            oled.display();
            delay(100);
          }
          if( enableShow == ENABLE)
            screenOLED = SCREEN15;
          break;
        case SCREEN15:  // Hiển thị MQ135 trong chu trình chính
          for(int j = 0; j < 2 && enableShow == ENABLE; j++) {
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(0, 20);
            oled.print("MQ135 Ratio:");
            oled.setTextSize(2);
            oled.setCursor(0, 32);
            oled.print(mq135GasRatio, 2);
          
            for(int i = 0; i < FRAME_COUNT_face1OLED && enableShow == ENABLE; i++) {
                  clearRectangle(96, 0, 128, 64);
                  if(warningMQ135State == SAD)
                    oled.drawBitmap(96, 20, face1OLED[i], 32, 32, 1);
                  else if(warningMQ135State == NORMAL)
                    oled.drawBitmap(96, 20, face2OLED[i], 32, 32, 1);
                  else if(warningMQ135State == HAPPY)
                    oled.drawBitmap(96, 20, face3OLED[i], 32, 32, 1);
                  oled.display();
                  delay(FRAME_DELAY);
            }
            oled.display();
            delay(100);
          }
          if( enableShow == ENABLE)
            screenOLED = SCREEN1;
          break;
        case SCREEN3:  // Hiển thị Bụi
          for(int j = 0; j < 2 && enableShow == ENABLE; j++) {
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(0, 20);
            oled.print("Bui min PM2.5: ");
            oled.setTextSize(2);
            oled.setCursor(0, 32);
            oled.print(dustValue);
            oled.setTextSize(1);
            oled.print(" ug/m3");
          
            for(int i = 0; i < FRAME_COUNT_face1OLED && enableShow == ENABLE; i++) {
                  clearRectangle(96, 0, 128, 64);
                  if(warningDustState == SAD)
                    oled.drawBitmap(96, 20, face1OLED[i], 32, 32, 1);
                  else if(warningDustState == NORMAL)
                    oled.drawBitmap(96, 20, face2OLED[i], 32, 32, 1);
                  else if(warningDustState == HAPPY)
                    oled.drawBitmap(96, 20, face3OLED[i], 32, 32, 1);
                  oled.display();
                  delay(FRAME_DELAY);
            }
            oled.display();
            delay(100);
          }
          if( enableShow == ENABLE)
            screenOLED = SCREEN1;
          break;
        case SCREEN4:    // Đang kết nối Wifi
          oled.clearDisplay();
          oled.setTextSize(1);
          oled.setCursor(40, 5);
          oled.print("WIFI");
          oled.setTextSize(1.5);
          oled.setCursor(40, 17);
          oled.print("Dang ket noi..");
      
          for(int i = 0; i < FRAME_COUNT_wifiOLED; i++) {
            clearRectangle(0, 0, 32, 32);
            oled.drawBitmap(0, 0, wifiOLED[i], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);
            oled.display();
            delay(FRAME_DELAY);
          }
          break;
        case SCREEN5:    // Kết nối wifi thất bại
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(40, 5);
            oled.print("WIFI");
            oled.setTextSize(1.5);
            oled.setCursor(40, 17);
            oled.print("Mat ket noi.");
            oled.drawBitmap(0, 0, wifiOLED[FRAME_COUNT_wifiOLED - 1 ], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);
            oled.drawLine(31, 0 , 0, 31 , 1);
            oled.drawLine(32, 0 , 0, 32 , 1);
            oled.display();
            delay(2000);
            screenOLED = SCREEN9;
          break;
        case SCREEN6:   // Đã kết nối Wifi, đang kết nối Blynk
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(40, 5);
            oled.print("WIFI");
            oled.setTextSize(1.5);
            oled.setCursor(40, 17);
            oled.print("Da ket noi.");
            oled.drawBitmap(0, 0, wifiOLED[FRAME_COUNT_wifiOLED - 1 ], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);

            oled.setTextSize(1);
            oled.setCursor(40, 34);
            oled.print("BLYNK");
            oled.setTextSize(1.5);
            oled.setCursor(40, 51);
            oled.print("Dang ket noi..");
                        

            for (int i = 0; i < FRAME_COUNT_blynkOLED; i++) {
                String topic = "your/mqtt/topic"; // Replace with your MQTT topic
                String payload = String(pgm_read_byte(&blynkOLED[i][0])); // Access data from program memory

                if (client.connected()) {
                    client.publish(topic.c_str(), payload.c_str()); // Publish to MQTT
                } else {
                  screenOLED = SCREEN9;
                }

                delay(FRAME_DELAY); // Optional delay between frames
            }

          break;
        case SCREEN7:   // Đã kết nối Wifi, Đã kết nối Blynk
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(40, 5);
            oled.print("WIFI");
            oled.setTextSize(1.5);
            oled.setCursor(40, 17);
            oled.print("Da ket noi.");
            oled.drawBitmap(0, 0, wifiOLED[FRAME_COUNT_wifiOLED - 1 ], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);

            oled.setTextSize(1);
            oled.setCursor(40, 34);
            oled.print("BLYNK");
            oled.setTextSize(1.5);
            oled.setCursor(40, 51);
            oled.print("Da ket noi.");
            oled.drawBitmap(0, 32, blynkOLED[FRAME_COUNT_wifiOLED/2], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);
            oled.display();
            delay(2000);
            screenOLED = SCREEN1;
            enableShow = ENABLE;
          break;
        case SCREEN8:   // Đã kết nối Wifi, Mat kết nối Blynk
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(40, 5);
            oled.print("WIFI");
            oled.setTextSize(1.5);
            oled.setCursor(40, 17);
            oled.print("Da ket noi.");
            oled.drawBitmap(0, 0, wifiOLED[FRAME_COUNT_wifiOLED - 1 ], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);

            oled.setTextSize(1);
            oled.setCursor(40, 34);
            oled.print("BLYNK");
            oled.setTextSize(1.5);
            oled.setCursor(40, 51);
            oled.print("Mat ket noi.");
            oled.drawBitmap(0, 32, blynkOLED[FRAME_COUNT_wifiOLED/2], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);
            oled.drawLine(31, 32 , 0, 63 , 1);
            oled.drawLine(32, 32 , 0, 64 , 1);
            oled.display();
            delay(2000);
            screenOLED = SCREEN9;
          break;
        case SCREEN9:   // Cai đặt 192.168.4.1
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(40, 5);
            oled.setTextSize(1);
            oled.print("Ket noi Wifi:");
            oled.setCursor(40, 17);
            oled.setTextSize(1);
            oled.print("ESP32_IOT");

            oled.setCursor(40, 38);
            oled.print("Dia chi IP:");
    
            oled.setCursor(40, 50);
            oled.print("192.168.4.1");

            for(int i = 0; i < FRAME_COUNT_settingOLED; i++) {
              clearRectangle(0, 0, 32, 64);
              oled.drawBitmap(0, 16, settingOLED[i], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);
              oled.display();
              delay(FRAME_DELAY*2);
            }
            countSCREEN9++;
            if(countSCREEN9 > 10) {
              countSCREEN9 = 0;
              screenOLED = SCREEN1;
              enableShow = ENABLE;
            }

            break;
          case SCREEN10:    // auto : on
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(40, 20);
            oled.print("Canh bao:");
            oled.setTextSize(2);
            oled.setCursor(40, 32);
            oled.print("DISABLE"); 
            for(int i = 0; i < FRAME_COUNT_autoOnOLED; i++) {
              clearRectangle(0, 0, 32, 64);
              oled.drawBitmap(0, 16, autoOnOLED[i], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);
              oled.display();
              delay(FRAME_DELAY);
            }
            clearRectangle(40, 32, 128, 64);
            oled.setCursor(40, 32);
            oled.print("ENABLE"); 
            oled.display();   
            delay(2000);
            screenOLED = SCREEN1;
            enableShow = ENABLE;
            break;
          case SCREEN11:     // auto : off
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(40, 20);
            oled.print("Canh bao:");
            oled.setTextSize(2);
            oled.setCursor(40, 32);
            oled.print("ENABLE");
            for(int i = 0; i < FRAME_COUNT_autoOffOLED; i++) {
              clearRectangle(0, 0, 32, 64);
              oled.drawBitmap(0, 16, autoOffOLED[i], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);
              oled.display();
              delay(FRAME_DELAY);
            }
            clearRectangle(40, 32, 128, 64);
            oled.setCursor(40, 32);
            oled.print("DISABLE"); 
            oled.display();    
            delay(2000);
            screenOLED = SCREEN1;  
            enableShow = ENABLE;
            break;
          case SCREEN12:  // gui du lieu len blynk
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(40, 20);
            oled.print("Gui du lieu");
            oled.setCursor(40, 32);
            oled.print("den BLYNK"); 
            for(int i = 0; i < FRAME_COUNT_sendDataOLED; i++) {
                clearRectangle(0, 0, 32, 64);
                oled.drawBitmap(0, 16, sendDataOLED[i], FRAME_WIDTH_32, FRAME_HEIGHT_32, 1);
                oled.display();
                delay(FRAME_DELAY);
            } 
            delay(1000);
            screenOLED = SCREEN1; 
            enableShow = ENABLE;
            break;
          case SCREEN13:   // khoi dong lai
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(0, 20);
            oled.print("Khoi dong lai");
            oled.setCursor(0, 32);
            oled.print("Vui long doi ..."); 
            oled.display();
            break;
          default : 
            delay(500);
            break;
      }
      delay(10);

      // Hiển thị MQ135 nếu chọn màn hình này
      if(screenOLED == SCREEN_MQ135) {
        showMQ135OLED();
        // Sau khi hiển thị xong, chuyển về màn hình chính
        screenOLED = SCREEN1;
      }
  }
}

//------------------- Hiển thị MQ135 trên màn hình OLED -------------------
void showMQ135OLED() {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 20);
  oled.print("MQ135 Ratio:");
  oled.setTextSize(2);
  oled.setCursor(0, 32);
  oled.print(mq135GasRatio, 2);
  
  for(int i = 0; i < FRAME_COUNT_face1OLED; i++) {
    clearRectangle(96, 0, 128, 64);
    if(warningMQ135State == SAD)
      oled.drawBitmap(96, 20, face1OLED[i], 32, 32, 1);
    else if(warningMQ135State == NORMAL)
      oled.drawBitmap(96, 20, face2OLED[i], 32, 32, 1);
    else if(warningMQ135State == HAPPY)
      oled.drawBitmap(96, 20, face3OLED[i], 32, 32, 1);
    oled.display();
    delay(FRAME_DELAY);
  }
  oled.display();
  delay(2000);
}



//-----------------Kết nối STA wifi, chuyển sang wifi AP nếu kết nối thất bại ----------------------- 
void connectSTA() {
      delay(5000);
      enableShow = DISABLE;
      if ( Essid.length() > 1 ) {  
      Serial.println(Essid);        //Print SSID
      Serial.println(Epass);        //Print Password
      Serial.println(Etoken);        //Print token
      Etoken = Etoken.c_str();
      WiFi.begin(Essid.c_str(), Epass.c_str());   //c_str()
      int countConnect = 0;
      while (WiFi.status() != WL_CONNECTED) {
          delay(500);   
          if(countConnect++  == 15) {
            Serial.println("Ket noi Wifi that bai");
            Serial.println("Kiem tra SSID & PASS");
            Serial.println("Ket noi Wifi: ESP32 de cau hinh");
            Serial.println("IP: 192.168.4.1");
            screenOLED = SCREEN5;
            digitalWrite(BUZZER, ENABLE);
            delay(2000);
            digitalWrite(BUZZER, DISABLE);
            delay(3000);
            break;
          }
          // MODE đang kết nối wifi
          screenOLED = SCREEN4;
          delay(2000);
      }
      Serial.println("");
      if(WiFi.status() == WL_CONNECTED) {
        Serial.println("Da ket noi Wifi: ");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP()); 
        Serial.println((char*)Essid.c_str());

       // MODE wifi đã kết nối, đang kết nối blynk
       screenOLED = SCREEN6;
       delay(2000);
        strcpy(BLYNK_AUTH_TOKEN,Etoken.c_str());
        
      }
      else {
        digitalWrite(BUZZER, ENABLE);
        delay(2000);
        digitalWrite(BUZZER, DISABLE);
        // MODE truy cập vào 192.168.4.1
        screenOLED = SCREEN9;
        connectAPMode(); 
      }
        
    }
}


//--------------------------- switch AP Mode --------------------------- 
void connectAPMode() {

  // Khởi tạo Wifi AP Mode, vui lòng kết nối wifi ESP32, truy cập 192.168.4.1
  WiFi.softAP(ssidAP, passwordAP);  

  // Gửi trang HTML khi client truy cập 192.168.4.1
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Gửi data ban đầu đến clientgetDataFromClient
  server.on("/data_before", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getJsonData();
    request->send(200, "application/json", json);
  });

  // Get data từ client
  server.on("/post_data", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "SUCCESS");
    enableShow = DISABLE;
    screenOLED = SCREEN13;
    delay(5000);
    ESP.restart();
  }, NULL, getDataFromClient);

  // Start server
  server.begin();
}

//------------------- Hàm đọc data từ client gửi từ HTTP_POST "/post_data" -------------------
void getDataFromClient(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  Serial.print("get data : ");
  Serial.println((char *)data);
  JSONVar myObject = JSON.parse((char *)data);
  if(myObject.hasOwnProperty("ssid"))
    Essid = (const char*) myObject["ssid"];
  if(myObject.hasOwnProperty("pass"))
    Epass = (const char*)myObject["pass"] ;
  if(myObject.hasOwnProperty("token"))
    Etoken = (const char*)myObject["token"];
  if(myObject.hasOwnProperty("mqtt_server"))
    Emqtt_server = (const char*)myObject["mqtt_server"];
  if(myObject.hasOwnProperty("typePlant"))
    EtypePlant = (int) myObject["typePlant"];
  if(myObject.hasOwnProperty("tempThreshold1"))
    EtempThreshold1 = (int) myObject["tempThreshold1"];
  if(myObject.hasOwnProperty("tempThreshold2"))
    EtempThreshold2 = (int) myObject["tempThreshold2"];
  if(myObject.hasOwnProperty("humiThreshold1"))
    EhumiThreshold1 = (int) myObject["humiThreshold1"];
  if(myObject.hasOwnProperty("humiThreshold2"))
    EhumiThreshold2 = (int) myObject["humiThreshold2"];
  if(myObject.hasOwnProperty("dustThreshold1"))
    EdustThreshold1 = (int) myObject["dustThreshold1"];
  if(myObject.hasOwnProperty("dustThreshold2"))
    EdustThreshold2 = (int) myObject["dustThreshold2"];
  if(myObject.hasOwnProperty("mq135Threshold1"))
    Emq135Threshold1 = (int) myObject["mq135Threshold1"];
  if(myObject.hasOwnProperty("mq135Threshold2"))
    Emq135Threshold2 = (int) myObject["mq135Threshold2"];
  writeEEPROM();
  

}

// ------------ Hàm in các giá trị cài đặt ------------
void printValueSetup() {
    Serial.print("ssid = ");
    Serial.println(Essid);
    Serial.print("pass = ");
    Serial.println(Epass);
    Serial.print("token = ");
    Serial.println(Etoken);
    Serial.print("mqtt_server = ");
    Serial.println(Emqtt_server);
    Serial.print("EtypePlant = ");
    Serial.println(EtypePlant);
    Serial.print("tempThreshold1 = ");
    Serial.println(EtempThreshold1);
    Serial.print("tempThreshold2 = ");
    Serial.println(EtempThreshold2);
    Serial.print("humiThreshold1 = ");
    Serial.println(EhumiThreshold1);
    Serial.print("humiThreshold2 = ");
    Serial.println(EhumiThreshold2);
    Serial.print("dustThreshold1 = ");
    Serial.println(EdustThreshold1);
    Serial.print("dustThreshold2 = ");
    Serial.println(EdustThreshold2);
    Serial.print("mq135Threshold1 = ");
    Serial.println(Emq135Threshold1);
    Serial.print("mq135Threshold2 = ");
    Serial.println(Emq135Threshold2);
    Serial.print("autoWarning = ");
    Serial.println(autoWarning);
}

//-------- Hàm tạo biến JSON để gửi đi khi có request HTTP_GET "/" --------
String getJsonData() {
  JSONVar myObject;
  myObject["ssid"]  = Essid;
  myObject["pass"]  = Epass;
  myObject["token"] = Etoken;
  myObject["mqtt_server"] = Emqtt_server;
  myObject["typePlant"] = EtypePlant;
  myObject["tempThreshold1"] = EtempThreshold1;
  myObject["tempThreshold2"] = EtempThreshold2;
  myObject["humiThreshold1"] = EhumiThreshold1;
  myObject["humiThreshold2"] = EhumiThreshold2;
  myObject["dustThreshold1"] = EdustThreshold1;
  myObject["dustThreshold2"] = EdustThreshold2;
  myObject["mq135Threshold1"] = Emq135Threshold1;
  myObject["mq135Threshold2"] = Emq135Threshold2;

  String jsonData = JSON.stringify(myObject);
  return jsonData;
}

//-------------------------------------------------------------------------------
//--------------------------------Task Blynk-------------------------------------

//----------------------------- Task auto Warning--------------------------------
void TaskAutoWarning(void *pvParameters)  {
    delay(20000);
    bool buzzerActive = false;
    unsigned long lastBuzzerToggle = 0;
    const unsigned long BUZZER_TOGGLE_INTERVAL = 500; // Bật/tắt còi mỗi 500ms để tạo tiếng kêu nhấp nháy
    
    while(1) {
      if(autoWarning == 1) {
        // Kiểm tra xem có cảnh báo nào không (khi giá trị nằm ngoài ngưỡng an toàn)
        bool hasWarning = false;
        
        // Kiểm tra nhiệt độ: ngoài ngưỡng khi < threshold1 hoặc > threshold2
        if(tempValue < EtempThreshold1 || tempValue > EtempThreshold2) {
          hasWarning = true;
        }
        
        // Kiểm tra độ ẩm: ngoài ngưỡng khi < threshold1 hoặc > threshold2
        if(humiValue < EhumiThreshold1 || humiValue > EhumiThreshold2) {
          hasWarning = true;
        }
        
        // Kiểm tra bụi: ngoài ngưỡng khi < threshold1 hoặc > threshold2
        if(dustValue < EdustThreshold1 || dustValue > EdustThreshold2) {
          hasWarning = true;
        }
        
        // Kiểm tra MQ135: ngoài ngưỡng khi < threshold1 hoặc > threshold2
        if(mq135GasRatio < Emq135Threshold1 || mq135GasRatio > Emq135Threshold2) {
          hasWarning = true;
        }
        
        // Nếu có cảnh báo, bật còi nhấp nháy
        if(hasWarning) {
          unsigned long currentTime = millis();
          if(currentTime - lastBuzzerToggle >= BUZZER_TOGGLE_INTERVAL) {
            buzzerActive = !buzzerActive;
            digitalWrite(BUZZER, buzzerActive ? ENABLE : DISABLE);
            lastBuzzerToggle = currentTime;
          }
        } else {
          // Không có cảnh báo, tắt còi
          if(buzzerActive) {
            digitalWrite(BUZZER, DISABLE);
            buzzerActive = false;
          }
        }
      } else {
        // autoWarning tắt, đảm bảo còi cũng tắt
        if(buzzerActive) {
          digitalWrite(BUZZER, DISABLE);
          buzzerActive = false;
        }
      }
      delay(100); // Kiểm tra thường xuyên hơn để còi phản ứng nhanh
    }
}

//----------------------- Send send Data value to Blynk every 2 seconds--------
void myTimer() {
    
}
//--------------Read button from BLYNK and send notification back to Blynk-----------------------
int checkAirQuality = 0;

//------------------------- check autoWarning from BLYNK  -----------------------

//---------------------------Task TaskSwitchAPtoSTA---------------------------

/*
 * Các hàm liên quan đến lưu dữ liệu cài đặt vào EEPROM
*/
//--------------------------- Read Eeprom  --------------------------------
void readEEPROM() {
    for (int i = 0; i < 32; ++i)       //Reading SSID
        Essid += char(EEPROM.read(i)); 
    for (int i = 32; i < 64; ++i)      //Reading Password
        Epass += char(EEPROM.read(i)); 
    for (int i = 64; i < 96; ++i)      //Reading Password
        Etoken += char(EEPROM.read(i));
    for (int i = 96; i < 128; ++i)     //Reading MQTT Server
        Emqtt_server += char(EEPROM.read(i));
    if(Essid.length() == 0) Essid = "BLK";

    EtypePlant      = (int)EEPROM.read(199);

    // Read as unsigned byte then cast to int to avoid sign extension issues
    EtempThreshold1 = (int)((unsigned char)EEPROM.read(200));
    EtempThreshold2 = (int)((unsigned char)EEPROM.read(201));

    EhumiThreshold1 = (int)((unsigned char)EEPROM.read(202));
    EhumiThreshold2 = (int)((unsigned char)EEPROM.read(203));

    EdustThreshold1 = (int)((unsigned char)EEPROM.read(204));
    EdustThreshold2 = (int)((unsigned char)EEPROM.read(205));

    Emq135Threshold1 = (int)((unsigned char)EEPROM.read(206));
    Emq135Threshold2 = (int)((unsigned char)EEPROM.read(207));

    // Read as unsigned byte then cast to int to avoid sign extension issues
    autoWarning = (int)((unsigned char)EEPROM.read(210));
    
    Serial.print("Read autoWarning from EEPROM: ");
    Serial.println(autoWarning);

    printValueSetup();
}

// ------------------------ Clear Eeprom ------------------------

void clearEeprom() {
    Serial.println("Clearing Eeprom");
    for (int i = 0; i < 250; ++i) 
      EEPROM.write(i, 0);
}

// -------------------- Hàm ghi data vào EEPROM ------------------
void writeEEPROM() {
    Serial.println("Writing to EEPROM:");
    Serial.print("  tempThreshold1 = "); Serial.println(EtempThreshold1);
    Serial.print("  tempThreshold2 = "); Serial.println(EtempThreshold2);
    Serial.print("  humiThreshold1 = "); Serial.println(EhumiThreshold1);
    Serial.print("  humiThreshold2 = "); Serial.println(EhumiThreshold2);
    Serial.print("  dustThreshold1 = "); Serial.println(EdustThreshold1);
    Serial.print("  dustThreshold2 = "); Serial.println(EdustThreshold2);
    Serial.print("  mq135Threshold1 = "); Serial.println(Emq135Threshold1);
    Serial.print("  mq135Threshold2 = "); Serial.println(Emq135Threshold2);
    Serial.print("  autoWarning = "); Serial.println(autoWarning);
    
    clearEeprom();
    for (int i = 0; i < Essid.length(); ++i)
          EEPROM.write(i, Essid[i]);  
    for (int i = 0; i < Epass.length(); ++i)
          EEPROM.write(32+i, Epass[i]);
    for (int i = 0; i < Etoken.length(); ++i)
          EEPROM.write(64+i, Etoken[i]);
    for (int i = 0; i < Emqtt_server.length(); ++i)
          EEPROM.write(96+i, Emqtt_server[i]);
    EEPROM.write(199, (byte)EtypePlant);               // lưu kiểu cây trồng
    
    // Ensure values are in valid range (0-255) before writing
    EEPROM.write(200, (byte)(EtempThreshold1 & 0xFF));          // lưu ngưỡng nhiệt độ 1
    EEPROM.write(201, (byte)(EtempThreshold2 & 0xFF));          // lưu ngưỡng nhiệt độ 2

    EEPROM.write(202, (byte)(EhumiThreshold1 & 0xFF));          // lưu ngưỡng độ ẩm 1
    EEPROM.write(203, (byte)(EhumiThreshold2 & 0xFF));          // lưu ngưỡng độ ẩm 2

    EEPROM.write(204, (byte)(EdustThreshold1 & 0xFF));          // lưu ngưỡng bụi 1
    EEPROM.write(205, (byte)(EdustThreshold2 & 0xFF));          // lưu ngưỡng bụi 2

    EEPROM.write(206, (byte)(Emq135Threshold1 & 0xFF));         // lưu ngưỡng MQ135 1
    EEPROM.write(207, (byte)(Emq135Threshold2 & 0xFF));         // lưu ngưỡng MQ135 2

    // Lưu autoWarning để không bị mất khi cập nhật threshold
    EEPROM.write(210, (byte)(autoWarning & 0xFF));              // lưu trạng thái autoWarning

    EEPROM.commit();

    Serial.println("EEPROM write completed");
    
    // Verify by reading back
    Serial.println("Verifying EEPROM write:");
    Serial.print("  Read back tempThreshold1 = "); Serial.println((int)((unsigned char)EEPROM.read(200)));
    Serial.print("  Read back tempThreshold2 = "); Serial.println((int)((unsigned char)EEPROM.read(201)));
    Serial.print("  Read back humiThreshold1 = "); Serial.println((int)((unsigned char)EEPROM.read(202)));
    Serial.print("  Read back humiThreshold2 = "); Serial.println((int)((unsigned char)EEPROM.read(203)));
    Serial.print("  Read back dustThreshold1 = "); Serial.println((int)((unsigned char)EEPROM.read(204)));
    Serial.print("  Read back dustThreshold2 = "); Serial.println((int)((unsigned char)EEPROM.read(205)));
    Serial.print("  Read back mq135Threshold1 = "); Serial.println((int)((unsigned char)EEPROM.read(206)));
    Serial.print("  Read back mq135Threshold2 = "); Serial.println((int)((unsigned char)EEPROM.read(207)));
    Serial.print("  Read back autoWarning = "); Serial.println((int)((unsigned char)EEPROM.read(210)));
    
    delay(500);
}


//-----------------------Task Task Button ----------
void TaskButton(void *pvParameters) {
    while(1) {
      handle_button(&buttonSET);
      handle_button(&buttonUP);
      handle_button(&buttonDOWN);
      delay(10);
    }
}
//-----------------Hàm xử lí nút nhấn nhả ----------------------
void button_press_short_callback(uint8_t button_id) {
    switch(button_id) {
      case BUTTON1_ID :  
        buzzerBeep(1);
        Serial.println("btSET press short");
        break;
      case BUTTON2_ID :
        buzzerBeep(1);
        Serial.println("btUP press short");
        break;
      case BUTTON3_ID :
        buzzerBeep(1);
        Serial.println("btDOWN press short");
        enableShow = DISABLE;
        screenOLED = SCREEN12;
        break;  
    } 
} 
//-----------------Hàm xử lí nút nhấn giữ ----------------------
void button_press_long_callback(uint8_t button_id) {
  switch(button_id) {
    case BUTTON1_ID :
      buzzerBeep(2);  
      enableShow = DISABLE;
      Serial.println("btSET press long");
      screenOLED = SCREEN9;
      clearOLED();
      connectAPMode(); 
      break;
    case BUTTON2_ID :
      buzzerBeep(2);
      Serial.println("btUP press long - Reconnecting WiFi and MQTT");
      connectSTA(); // Reconnect WiFi
      connectMQTT(); // Reconnect MQTT
      break;
    case BUTTON3_ID :
      buzzerBeep(2);
      Serial.println("btDOWN press long - Toggle autoWarning");
      enableShow = DISABLE;
      autoWarning = 1 - autoWarning;
      Serial.print("Writing autoWarning to EEPROM (from button): ");
      Serial.println(autoWarning);
      EEPROM.write(210, (byte)(autoWarning & 0xFF));
      EEPROM.commit();
      // Verify by reading back
      int readBack = (int)((unsigned char)EEPROM.read(210));
      Serial.print("Read back autoWarning from EEPROM: ");
      Serial.println(readBack);
      if(autoWarning == 0) screenOLED = SCREEN11;
      else screenOLED = SCREEN10;
      // Publish autoWarning state immediately when changed via button
      if (client.connected()) {
        publishAutoWarning();
      }
      break;  
  } 
} 
// ---------------------- Hàm điều khiển còi -----------------------------
void buzzerBeep(int numberBeep) {
  for(int i = 0; i < numberBeep; ++i) {
    digitalWrite(BUZZER, ENABLE);
    delay(100);
    digitalWrite(BUZZER, DISABLE);
    delay(100);
  }  
}
// ---------------------- Hàm điều khiển LED -----------------------------
void blinkLED(int numberBlink) {
  for(int i = 0; i < numberBlink; ++i) {
    digitalWrite(LED, DISABLE);
    delay(300);
    digitalWrite(LED, ENABLE);
    delay(300);
  }  
}
void TaskMQ135Sensor(void *pvParameters) {
  // MQ135 Ratio = Rs / R0
  // - Ratio < 1.0: Không khí sạch
  // - Ratio ≈ 1.0: Không khí bình thường
  // - Ratio > 1.0: Có khí độc hại (NH3, CO2, Benzen, Alcohol, Smoke...)
  // - Ratio càng cao = nồng độ khí độc càng cao

  analogSetPinAttenuation(MQ135_PIN, ADC_11db); // quan trọng
  analogReadResolution(12);

  while (1) {
    // Đọc giá trị ADC từ cảm biến
    int adc = analogRead(MQ135_PIN);

    // Chuyển đổi ADC sang điện áp (Vout)
    float vout = (adc / 4095.0f) * MQ135_VCC;

    // Tính điện trở của cảm biến (Rs) khi có khí
    float rs = 0.0;
    if (vout > 0.0f) {
      rs = MQ135_RL * (MQ135_VCC - vout) / vout;
    }

    // Tính tỷ lệ Rs/R0 (Gas Ratio) - chỉ số chất lượng không khí
    if (MQ135_R0 > 0.0f) {
      mq135GasRatio = rs / MQ135_R0;
    } else {
      mq135GasRatio = 0.0;
    }

    Serial.printf("MQ135 ADC=%d | Rs=%.2f | Ratio=%.2f\n",
                  adc, rs, mq135GasRatio);

    if(mq135GasRatio < Emq135Threshold1 || mq135GasRatio > Emq135Threshold2) 
      warningMQ135State = NORMAL;
    else
      warningMQ135State = HAPPY;

    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}
