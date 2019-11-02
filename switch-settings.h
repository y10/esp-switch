#ifndef SWITCH_SETTINGS_H
#define SWITCH_SETTINGS_H

#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <Time.h>
#include "switch.h"

class SwitchSettings
{
private:
  String host_name;
  String disp_name;
  String safe_name;
  String pair_addr;
  String mqtt_host;
  String mqtt_port;
  String mqtt_user;
  String mqtt_pwrd;

public:
  SwitchSettings()
  {
    disp_name = "Switch";
    safe_name = "switch";
    host_name = "switch-" + String(ESP.getChipId(), HEX);
    mqtt_host = "mqqt";
    mqtt_port = 1883;
    mqtt_user = "homeassistant";
    mqtt_pwrd = "123456";
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

  const String mqtthost() const {
    return mqtt_host;
  }

  void mqtthost(const char* host){
    if (strlen(host) > 0) mqtt_host = host;
  }

  const String mqttport() const {
    return mqtt_port;
  }

  void mqttport(int port){
    if (port > 0) mqtt_port = port;
  }

  const String mqttuser() const {
    return mqtt_user;
  }

  void mqttuser(const char* user){
    if (strlen(user) > 0) mqtt_user = user;
  }

  const String mqttpwrd() const {
    return mqtt_pwrd;
  }

  void mqttpwrd(const char* pwrd){
    mqtt_pwrd = pwrd;
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
    //read configuration from FS json
    Serial.println("[SETTINGS] mounting FS...");

    if (SPIFFS.begin())
    {
      Serial.println("[SETTINGS] mounted file system");
      if (SPIFFS.exists("/config.json"))
      {
        //file exists, reading and loading
        Serial.println("[SETTINGS] reading config file");
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile)
        {
          Serial.println("[SETTINGS] opened config file");
          size_t size = configFile.size();
          // Allocate a buffer to store contents of the file..0
          std::unique_ptr<char[]> buf(new char[size]);
          configFile.readBytes(buf.get(), size);

          DynamicJsonBuffer jsonBuffer;
          JsonObject &json = jsonBuffer.parseObject(buf.get());
          if (json.success())
          {
            Serial.println("[SETTINGS] parsed json");
            json.printTo(Serial);

            if (json.containsKey("disp_name")) dispname(json["disp_name"]);
            if (json.containsKey("pair_addr")) pairaddr(json["pair_addr"]);
            if (json.containsKey("mqtt_host")) mqtthost(json["mqtt_host"]);
            if (json.containsKey("mqtt_port")) mqttport(json["mqtt_port"]);
            if (json.containsKey("mqtt_user")) mqttuser(json["mqtt_user"]);
            if (json.containsKey("mqtt_pwrd")) mqttpwrd(json["mqtt_pwrd"]);
          }
          else
          {
            Serial.println("[SETTINGS] failed to load json config");
          }
        }
      }
    }
    else
    {
      Serial.println("[SETTINGS] failed to mount FS");
    }
  }

  void setupMQTT(AsyncMqttClient& mqttClient)
  {
    IPAddress mqtt_ip;

    if (mqtt_ip.fromString(mqtt_host))
    {
      mqttClient.setServer(mqtt_ip, mqtt_port.toInt());
    }
    else
    {
      mqttClient.setServer(mqtt_host.c_str(), mqtt_port.toInt());
    }
    
    mqttClient.setKeepAlive(5);
    mqttClient.setCredentials(mqtt_user.c_str(), (mqtt_pwrd.length() == 0) ? nullptr : mqtt_pwrd.c_str());
    mqttClient.setClientId(host_name.c_str());
  }

  void save()
  {
    Serial.println("[MAIN] saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();

    json["disp_name"] = disp_name.c_str();
    json["pair_addr"] = pair_addr.c_str();
    json["mqtt_host"] = mqtt_host.c_str();
    json["mqtt_port"] = mqtt_port.toInt();
    json["mqtt_user"] = mqtt_user.c_str();
    json["mqtt_pwrd"] = mqtt_pwrd.c_str();

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
      Serial.println("[MAIN] failed to open config file for writing");
    }

    json.printTo(Serial);
    Serial.println();
    json.printTo(configFile);
    configFile.close();
  }

  String toJSON()
  {
    return "{\r\n" + ((timeStatus() != timeNotSet) ? " \"time\": \"" + (String)hour() + ":" + (String)minute() + "\",\r\n" : "") + " \"name\": \"" + disp_name + "\",\r\n" + " \"addr\": \"" + pair_addr + "\" " + "\r\n}";
  }
};

extern SwitchSettings Settings = SwitchSettings();

#endif
