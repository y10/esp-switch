#ifndef SPRINKLER_WEBSERVER_H
#define SPRINKLER_WEBSERVER_H

#include <ESPAsyncWebServer.h>
#include "switch.h"
#include "switch-settings.h"
#include "switch.html.h"
#include "switch.icon.h"

class SwitchHttp
{
private:
  
  char lastModified[50];
  void respondCachedRequest(AsyncWebServerRequest *request, const String& contentType, const uint8_t * content, size_t len){
    if (request->header("If-Modified-Since").equals(lastModified)) {
      
      request->send(304);
  
    } else {
  
        // Dump the byte array in PROGMEM with a 200 HTTP code (OK)
        AsyncWebServerResponse *response = request->beginResponse_P(200, contentType, content, len);
  
        // Tell the browswer the contemnt is Gzipped
        response->addHeader("Content-Encoding", "gzip");
  
        // And set the last-modified datetime so we can check if we need to send it again next time or not
        response->addHeader("Last-Modified", lastModified);
  
        request->send(response);
    }
  }

  void respondManifestRequest(AsyncWebServerRequest *request)
  {
      request->send(200, "application/json", "{ "
                                         "\"short_name\": \"Switch\","
                                         "\"name\": \"Wifi Switch\","
                                         "\"icons\": ["
                                         "  {"
                                         "    \"src\": \"/icon.png\","
                                         "    \"sizes\": \"180x180\","
                                         "    \"type\": \"image/png\""
                                         "  }"
                                         "],"
                                         "\"start_url\": \"/\","
                                         "\"display\": \"fullscreen\""
                                         "}");
  }

  void respondStateRequest(AsyncWebServerRequest *request)
  {
    request->send(200, "application/json", Switch.toJSON());
  }

  void respondTurnOnRequest(AsyncWebServerRequest *request)
  {
    Switch.turnOn();

    respondStateRequest(request);
  }

  void respondTurnOffRequest(AsyncWebServerRequest *request)
  {
    Switch.turnOff();

    respondStateRequest(request);
  }

  void respondResetRequest(AsyncWebServerRequest *request)
  {
    Switch.reset();

    respondStateRequest(request);
  }

  void respondRestartRequest(AsyncWebServerRequest *request)
  {
    Switch.restart();

    respondStateRequest(request);
  }

  void respondScheduleStateRequest(AsyncWebServerRequest *request)
  {
    request->send(200, "application/json", Schedule.toJSON());
  }

  void respondScheduleStateRequest(timeDayOfWeek_t day, AsyncWebServerRequest *request)
  {
    request->send(200, "application/json", Schedule.get(day).toJSON());
  }

  void respondScheduleRequest(AsyncWebServerRequest *request)
  {
    Switch.schedule(
        request->hasArg("h") ? request->arg("h").toInt() : -1,
        request->hasArg("m") ? request->arg("m").toInt() : -1,
        request->hasArg("d") ? request->arg("d").toInt() : -1,
        request->hasArg("enabled") ? request->arg("enabled").toInt() : -1);

    respondScheduleStateRequest(request);
  }

  void respondScheduleRequest(timeDayOfWeek_t day, AsyncWebServerRequest *request)
  {
    Switch.schedule(
        day,
        request->hasArg("h") ? request->arg("h").toInt() : -1,
        request->hasArg("m") ? request->arg("m").toInt() : -1,
        request->hasArg("d") ? request->arg("d").toInt() : -1,
        request->hasArg("enabled") ? request->arg("enabled").toInt() : -1);

    respondScheduleStateRequest(day, request);
  }

  void respondSettingsRequest(AsyncWebServerRequest *request)
  {
    if (request->hasArg("n"))
    {
      Switch.rename(request->arg("n"));
    }

    request->send(200, "application/json", Settings.toJSON());
  }
  
  void respond404Request(AsyncWebServerRequest *request)
  {
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  }

public:

  SwitchHttp()
  {
    sprintf(lastModified, "%s %s GMT", __DATE__, __TIME__);
  }

  void setup(AsyncWebServer &server)
  {
   
    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondCachedRequest(request, "text/html", SWITCH_INDEX_HTML_GZ, sizeof(SWITCH_INDEX_HTML_GZ));
    });
    server.on("/icon.png", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondCachedRequest(request, "image/png", SWITCH_ICON_PNG_GZ, sizeof(SWITCH_ICON_PNG_GZ));
    });
    server.on("/apple-touch-icon.png", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondCachedRequest(request, "image/png", SWITCH_APPLE_TOUCH_ICON_PNG_GZ, sizeof(SWITCH_APPLE_TOUCH_ICON_PNG_GZ));
    });

    server.on("/manifest.json", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondManifestRequest(request);
    });

    server.on("/reset", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondResetRequest(request);
    });
    server.on("/restart", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondRestartRequest(request);
    });

    server.on("/api/state", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondStateRequest(request);
    });
    server.on("/api/on", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondTurnOnRequest(request);
    });
    server.on("/api/off", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondTurnOffRequest(request);
    });

    server.on("/api/settings", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondSettingsRequest(request);
    });

    server.on("/api/schedule/mon", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondScheduleRequest(dowMonday, request);
    });
    server.on("/api/schedule/tue", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondScheduleRequest(dowTuesday, request);
    });
    server.on("/api/schedule/wed", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondScheduleRequest(dowWednesday, request);
    });
    server.on("/api/schedule/thu", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondScheduleRequest(dowThursday, request);
    });
    server.on("/api/schedule/fri", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondScheduleRequest(dowFriday, request);
    });
    server.on("/api/schedule/sat", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondScheduleRequest(dowSaturday, request);
    });
    server.on("/api/schedule/sun", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondScheduleRequest(dowSunday, request);
    });
    server.on("/api/schedule", HTTP_GET, [&](AsyncWebServerRequest *request){
      respondScheduleRequest(request);
    });
  
    server.onNotFound([&](AsyncWebServerRequest *request){
      respond404Request(request);
    });
  }
};

#endif
