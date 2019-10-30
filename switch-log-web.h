#ifndef SWITCH_LOG_WEB_H
#define SWITCH_LOG_WEB_H

#include <ESPAsyncWebServer.h>
#include "switch-log.h"

//static std::vector<String*> lines;

class WebLog : public Stream
{
  private:
    AsyncEventSource * source;
    std::vector<String*> lines;
    String line;

  public:
    ~WebLog(void)
    {
    }

    int available(void) override
    {
       return 0;
    }

    int peek(void) override
    {
       return 0;
    }
 
    int read(void) override
    {
       return 0;
    }

    size_t write(uint8_t c) override
    {
        return 0;
    }

    size_t write(const uint8_t *buffer, size_t size) override
    {
        if (!source)
        {
            String * str = new String();
        
            str->reserve(size);

            for(size_t i = 0; i < size; i++)
            {
                str->concat(buffer[i]);
            }

            lines.push_back(str);
        }
        
        return Serial.write(buffer, size);        
    }
   
    void flush()
    {
        for (auto line : lines)
        {
             source->send(line->c_str(), "log");
             delete line;
        }

        lines.clear();
    }

    void setup(AsyncEventSource * evs)
    {
        evs->onConnect([&](AsyncEventSourceClient * client){

            if(!source)
            {
                this->source = evs;
                this->flush();
                this->println("[LOG] Connected");

                evs->onConnect(NULL);
            }
        });
    }
};

extern WebLog Log = WebLog();

#endif