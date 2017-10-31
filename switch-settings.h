#ifndef SWITCH_SETTINGS_H
#define SWITCH_SETTINGS_H

#include <Arduino.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson 
#include "switch.h"

class SwitchSettings
{
private:

  char device_name[50] = "Switch";
  
public:

  char * getDeviceName(char * str, size_t len) {
      if (device_name) {
          strncpy(str, device_name, len);
      }
      return str;
  }
  
  bool setDeviceName(const char * value)
  {
    if (value && sizeof(value) > 1 && strcmp(device_name, value) != 0)
    {
      strcpy(device_name, value);

      return true;
    }

    return false;
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
          json.printTo(Serial);
          if (json.success())
          {
            Serial.println("[SETTINGS] parsed json");
            strcpy(device_name, json["relay_name"]);
            Serial.print("[SETTINGS] device_name ");
            Serial.println(device_name);
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

  void save()
  {
    Serial.println("[MAIN] saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["relay_name"] = device_name;
  
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("[MAIN] failed to open config file for writing");
    }
  
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
  }

  String toJSON()
  {

    return "{\r\n" +
           ((timeStatus() != timeNotSet) ? " \"time\": \""+ (String)hour() + ":" + (String)minute() + "\",\r\n" : "") + "" +
           " \"name\": \"" + (String)device_name + "\""
           "\r\n}";
  }
};

extern SwitchSettings Settings = SwitchSettings();

#endif
