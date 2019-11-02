#ifndef SWITCH_LOG_WEB_H
#define SWITCH_LOG_WEB_H

#define WEBLOG

#include <ESPAsyncWebServer.h>

class WebLog : public Print
{
  private:
    AsyncEventSource * source;
    String log;

  public:
    ~WebLog(void)
    {
    }

    virtual size_t write(uint8_t c) override
    {
        return write(&c, sizeof(c));
    }
    
    virtual size_t write(const uint8_t *data, size_t size) override
    {
        char text[size + 1];
        strncpy(text, (const char *)data, size);
        text[size] = 0;
        buffer(text);
        flush(); 

        return Serial.write(data, size);        
    }

    void setup(AsyncEventSource * evs)
    {
        source = evs;
        evs->onConnect([&](AsyncEventSourceClient * client){
            Serial.println("[LOG] Connected.");
            flush();
        });
    }

    void flush() 
    {   
        if(source)
        {
            int index = log.indexOf("\r\n");
            while(index != -1)
            {
                String line = log.substring(0, index);
                log = log.substring(index + 2);
                index = log.indexOf("\r\n");
                source->send(line.c_str(), "log");
            }
        }
    }

private:
    void buffer(const char * text) 
    {
        log += text;
        
        if (log.length() > 1000)
        {
            int index = log.indexOf("\r\n");
            log = log.substring(index + 2);
        }
    }
};

extern WebLog Log = WebLog();

#endif