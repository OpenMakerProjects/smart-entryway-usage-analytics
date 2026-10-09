#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>
#include "../usage.h"
#include "../config.h"
WiFiClient wifi;PubSubClient mqtt(wifi);Adafruit_BME280 bme;Adafruit_SSD1306 oled(128,64,&Wire,-1);Usage usage;
bool sensorReady=false,screen=false,requested=false;uint32_t sampled=0,retry=0;
void command(char*,byte* data,unsigned n){if(n==2&&!memcmp(data,"ON",2))requested=true;if(n==3&&!memcmp(data,"OFF",3))requested=false;}
void setup(){
 pinMode(12,OUTPUT);pinMode(13,OUTPUT);pinMode(14,OUTPUT);analogWriteRange(255);
 analogWrite(12,0);analogWrite(13,0);analogWrite(14,0);Serial.begin(115200);Wire.begin(4,5);
 sensorReady=bme.begin(0x76);screen=oled.begin(SSD1306_SWITCHCAPVCC,0x3C);
 mqtt.setServer(MQTT_HOST,MQTT_PORT);mqtt.setCallback(command);mqtt.setSocketTimeout(1);
 if(strlen(WIFI_SSID))WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
}
void loop(){
 uint32_t now=millis();static char line[8];static unsigned count=0;static bool overflow=false;
 while(Serial.available()){char c=Serial.read();if(c=='\n'){if(!overflow)command(nullptr,(byte*)line,count);count=0;overflow=false;}else if(c!='\r'){if(count<sizeof(line))line[count++]=c;else overflow=true;}}
 if(strlen(MQTT_HOST)&&WiFi.status()==WL_CONNECTED&&!mqtt.connected()&&uint32_t(now-retry)>=10000){
  retry=now;String cid="entry17-"+String(ESP.getChipId(),HEX);bool ok=strlen(MQTT_USER)?mqtt.connect(cid.c_str(),MQTT_USER,MQTT_PASSWORD,"entry17/status",0,true,"offline"):mqtt.connect(cid.c_str(),"entry17/status",0,true,"offline");
  if(ok){mqtt.subscribe("entry17/command");mqtt.publish("entry17/status","online",true);}
 }mqtt.loop();
 if(uint32_t(now-sampled)<1000){delay(1);return;}sampled=now;
 Wire.beginTransmission(0x76);bool ack=Wire.endTransmission()==0;if(!sensorReady&&ack)sensorReady=bme.begin(0x76);
 float t=sensorReady&&ack?bme.readTemperature():NAN,h=sensorReady&&ack?bme.readHumidity():NAN;
 bool valid=ack&&isfinite(t)&&isfinite(h)&&t>=-40&&t<=85&&h>=0&&h<=100,active=requested&&valid;usage.tick(now,active);
 analogWrite(12,active&&h>=70?180:0);analogWrite(13,active&&h<70&&t>=15?120:0);analogWrite(14,active&&h<70&&t<15?120:0);
 if(screen){oled.clearDisplay();oled.setTextSize(1);oled.setTextColor(SSD1306_WHITE);oled.setCursor(0,0);if(valid){oled.print("T ");oled.print(t,1);oled.print("C RH ");oled.print(h,0);oled.println("%");}else oled.println("SENSOR FAULT");
  oled.print("Requested ");oled.println(requested?"ON":"OFF");oled.print("Actual ");oled.println(active?"ON":"OFF");oled.print("Day min ");oled.println(usage.currentDay()/60000);oled.print("7 slots min ");oled.println((unsigned long)(usage.weekTotal()/60000));oled.display();}
 char out[340];snprintf(out,sizeof(out),"{\"id\":17,\"uptime_s\":%lu,\"valid\":%s,\"temperature_c\":%.1f,\"humidity_pct\":%.1f,\"requested\":%s,\"active\":%s,\"hour_ms\":%lu,\"day_ms\":%lu,\"slots24_ms\":%lu,\"slots7day_ms\":%lu}",(unsigned long)(usage.elapsed/1000),valid?"true":"false",valid?t:0,valid?h:0,requested?"true":"false",active?"true":"false",(unsigned long)usage.currentHour(),(unsigned long)usage.currentDay(),(unsigned long)usage.hoursTotal(),(unsigned long)usage.weekTotal());Serial.println(out);
 if(mqtt.connected()){mqtt.publish("entry17/state",out,true);mqtt.publish("entry17/switch/state",requested?"ON":"OFF",true);}
}
