#ifndef _MY_CONFIG_H_
#define _MY_CONFIG_H_

#include <Arduino.h>

const char* ssidAP     = "ESP32_IOT";     // Tên wifi AP Mode
const char* passwordAP = "17263544";          // Mật khẩu AP Mode

String  Essid   = "";                 // EEPROM tên wifi nhà bạn
String  Epass   = "";                 // EEPROM mật khẩu wifi nhà bạn
String  Etoken = "";                  // EEPROM mã token blynk
String  Emqtt_server = "";            // EEPROM MQTT server address
int  EtypePlant = 0;                  // Kiểu cây trồng, xem chi tiết bên file index_html.h
int  EtempThreshold1 = 0;             // ngưỡng nhiệt độ 1
int  EtempThreshold2 = 0;             // ngưỡng nhiệt độ 2
int  EhumiThreshold1 = 0;             // ngưỡng độ ẩm 1
int  EhumiThreshold2 = 0;             // ngưỡng độ ẩm 2
int  EsoilMoistureThreshold1 = 0;             // ngưỡng bụi 1
int  EsoilMoistureThreshold2 = 0;             // ngưỡng bụi 2

String snTemp[4] = {"", "Nhiệt độ thấp ","Nhiệt độ phù hợp ","Nhiệt độ cao "};
String snHumi[4] = {"", "Độ ẩm thấp ","Độ ẩm phù hợp ","Độ ẩm cao "};
String snSoilMoisture[4] = {"", "Độ ẩm đất thấp ","Độ ẩm phù hợp ","Độ ẩm cao "};

#endif