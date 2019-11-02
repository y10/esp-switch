#ifndef SWITCH_LOG_H
#define SWITCH_LOG_H

#include <Arduino.h>

class LogClass : public Print
{
public:
    LogClass()
    {
    }

    ~LogClass(void)
    {
    }

    size_t write(uint8_t c) override
    {
        return Serial.write(c);
    }

    size_t write(const uint8_t *buffer, size_t size) override
    {
        return Serial.write(buffer, size);
    }

    void flush() override
    {
        Serial.flush();
    }
};

extern LogClass Log = LogClass();

#endif
