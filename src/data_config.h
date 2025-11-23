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
int  EhumiThreshold1 = 0;             // ngưỡng độ ẩm thấp (humidity threshold low)
int  EhumiThreshold2 = 0;             // ngưỡng độ ẩm cao (humidity threshold high)
int  EdustThreshold1 = 0;             // Ngưỡng bụi thấp (dust sensor threshold low)
int  EdustThreshold2 = 0;             // Ngưỡng bụi cao (dust sensor threshold high)
int  Emq135Threshold1 = 0;            // Ngưỡng khí độc thấp (MQ135 threshold low)
int  Emq135Threshold2 = 0;            // Ngưỡng khí độc cao (MQ135 threshold high)
// ĐÃ XOÁ ngưỡng độ ẩm đất (soil threshold removed)

String snTemp[4] = {"", "Nhiệt độ thấp ","Nhiệt độ phù hợp ","Nhiệt độ cao "};
String snHumi[4] = {"", "Độ ẩm thấp ","Độ ẩm phù hợp ","Độ ẩm cao "};
String snSoilMoisture[4] = {"", "Độ ẩm đất thấp ","Độ ẩm phù hợp ","Độ ẩm cao "};

#endif