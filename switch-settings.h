#ifndef SWITCH_SETTINGS_H
#define SWITCH_SETTINGS_H

#include <EEPROM.h>
#include <Time.h>
#include <base64.h>
#include "switch.h"
#define EEPROM_SIZE 1024

struct SwitchConfig {
  uint8_t version;
  char disp_name[50];
  char upds_addr[50];
  char mqtt_host[50];
  uint16_t mqtt_port;
  char mqtt_user[50];
  char mqtt_pwrd[50];
};

class SwitchSettings
{
private:
  String host_name;
  String disp_name;
  String safe_name;
  String pair_addr;
  String upds_addr;
  String mqtt_host;
  String mqtt_port;
  String mqtt_user;
  String mqtt_pwrd;
  AsyncMqttClient * mqtt;

public:
  SwitchSettings()
  {
    disp_name = "Switch";
    safe_name = "switch";
    host_name = "switch-" + String(ESP.getChipId(), HEX);
    upds_addr = "http://ota.voights.net/switch.bin";
    mqtt_host = "";
    mqtt_port = 1883;
    mqtt_user = "homeassistant";
    mqtt_pwrd = "123456";
    mqtt = nullptr;
  }

  const String hostname() const {
    return host_name;
  }

  const String dispname() const {
    return disp_name;
  }

  const String safename() const{
    return safe_name;
  }

  bool dispname(const char* name){
    bool changed = false;
    if (strlen(name) > 0)
    {
      if (!disp_name.equals(name))
      {
        disp_name = name;
        changed = true;
      }
      if (!safe_name.equals(disp_name))
      {
        safe_name = name;
        safe_name.replace(" ", "_");
        safe_name.toLowerCase();
        changed = true;
      }
    }

    return changed;
  }

  const String pairaddr() const {
    return pair_addr;
  }

  bool pairaddr(const char* addr){
    if (!pair_addr.equals(addr))
    {
      pair_addr = addr;
      return true;
    }

    return false;
  }

  const String updsaddr() const {
    return upds_addr;
  }

  const String updsaddr(const char* addr){
    upds_addr = addr;
    
    if (upds_addr.indexOf("://") == -1)
    {
      upds_addr = "http://" + upds_addr;
    }

    return upds_addr;
  }

  const String mqtthost() const {
    return mqtt_host;
  }

  const String mqtthost(const char* host){
    mqtt_host = host;
    return mqtt_host;
  }

  const String mqttport() const {
    return mqtt_port;
  }

  bool mqttport(int port){
    if (port > 0) {
       mqtt_port = port;
       return true;
    }
    return false;
  }

  const String mqttuser() const {
    return mqtt_user;
  }

  const String mqttuser(const char* user){
    mqtt_user = user;
    return mqtt_user;
  }

  const String mqttpwrd() const {
    return mqtt_pwrd;
  }

  const String mqttpwrd(const char* pwrd){
    mqtt_pwrd = pwrd;
    return mqtt_pwrd;
  }

  time_t getBuildDate()
  {
    tmElements_t te;
    breakTime(now(), te);
    te.Month = 1;
    te.Day = 1;
    te.Year = 2017;

    return makeTime(te);
  }

  void setHour(int value)
  {
    tmElements_t te;
    if (timeStatus() == timeNotSet)
    {
      breakTime(getBuildDate(), te);
      te.Minute = 0;
    }
    else
    {
      breakTime(now(), te);
    }
    te.Hour = value;
    setTime(makeTime(te));
  }

  void setMinute(int value)
  {
    tmElements_t te;
    if (timeStatus() == timeNotSet)
    {
      breakTime(getBuildDate(), te);
      te.Hour = 0;
    }
    else
    {
      breakTime(now(), te);
    }
    te.Minute = value;
    setTime(makeTime(te));
  }

  void load()
  {
    Serial.println("[EEPROM] reading...");
    EEPROM.begin(EEPROM_SIZE);
    SwitchConfig config;
    EEPROM.get(0, config);
    if(config.version == 1) 
    {
        dispname(config.disp_name);
        updsaddr(config.upds_addr);
        mqtthost(config.mqtt_host);
        mqttport(config.mqtt_port);
        mqttuser(config.mqtt_user);
        mqttpwrd(config.mqtt_pwrd);
    }
    else
    {
      Serial.println("[EEPROM] not found.");
    }
  }

  bool setupMQTT(AsyncMqttClient& mqttClient)
  {
    IPAddress mqtt_ip;

    if (mqtt_host.length() > 0)
    {
      if (mqtt_ip.fromString(mqtt_host))
      {
        mqttClient.setServer(mqtt_ip, mqtt_port.toInt());
      }
      else
      {
        mqttClient.setServer(mqtt_host.c_str(), mqtt_port.toInt());
      }
      
      mqttClient.setKeepAlive(30);
      mqttClient.setCredentials(mqtt_user.c_str(), (mqtt_pwrd.length() == 0) ? nullptr : mqtt_pwrd.c_str());
      mqttClient.setClientId(host_name.c_str());
      mqtt = &mqttClient;
      return true;
    }

    return false;
  }

  void save()
  {
    Serial.println("[EEPROM] saving");
    SwitchConfig config = {1, 0, 0, 0, mqtt_port.toInt(), 0, 0 };
    strcpy(config.disp_name, disp_name.c_str());
    strcpy(config.upds_addr, upds_addr.c_str());
    strcpy(config.mqtt_host, mqtt_host.c_str());
    strcpy(config.mqtt_user, mqtt_user.c_str());
    strcpy(config.mqtt_pwrd, mqtt_pwrd.c_str());
    EEPROM.put(0, config);
    EEPROM.commit();
  }

  
  void clear()
  {
    Serial.println("[EEPROM] clear");
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
  }


  String toJSON()
  {
    base64 b;
    return (String) "{" +
    "\r\n  \"disp_name\": \"" + disp_name + "\"" +
    "\r\n ,\"host_name\": \"" + host_name + "\"" 
    "\r\n ,\"upds_addr\": \"" + upds_addr + "\"" 
    "\r\n ,\"mqtt_addr\": \"" + mqtt_host + ":" + (String)mqtt_port + "\"" +
    "\r\n ,\"mqtt_user\": \"" + mqtt_user + "\"" 
    "\r\n ,\"mqtt_pwrd\": \"" + b.encode(mqtt_pwrd) + "\"" 
    "\r\n ,\"mqtt_conn\": \"" + (String)(mqtt && mqtt->connected()) + "\"" 
     + ((timeStatus() != timeNotSet) ? "\r\n ,\"time\": \"" + (String)hour() + ":" + (String)minute() + "\"" : "") +
    "\r\n}";
  }
};

extern SwitchSettings Settings = SwitchSettings();

#endif
