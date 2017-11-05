#ifndef SPRINKLER_H
#define SPRINKLER_H

#include <Arduino.h>
#include <Ticker.h>
#include <TimeAlarms.h>
#include <vector>
#include <functional>
#include "schedule.h"
#include "switch-settings.h"

void turnOffSwitch();
void turnOnEveryday();
void turnOnMonday();
void turnOnTuesday();
void turnOnWednesday();
void turnOnThursday();
void turnOnFriday();
void turnOnSaturday();
void turnOnSunday();

typedef std::function<void()> Delegate;

class SwitchClass
{
private:
  std::vector<Delegate> onChangeEventHandlers;

  Delegate onAction;
  Delegate offAction;
  Delegate resetAction;
  Delegate restartAction;

  Ticker countdown;

  unsigned long startTime;

  void notify()
  {
    for (auto &event : onChangeEventHandlers) // access by reference to avoid copying
    {
      event();
    }
  }

public:
  SwitchClass() : startTime(0)
  {
  }

  void onChange(Delegate event)
  {
    onChangeEventHandlers.push_back(event);
  }

  void onTurnOn(Delegate callback)
  {
    onAction = callback;
  }

  void onTurnOff(Delegate callback)
  {
    offAction = callback;
  }

  void onRestart(Delegate callback)
  {
    restartAction = callback;
  }

  void onReset(Delegate callback)
  {
    resetAction = callback;
  }

  String toJSON()
  {

    return "{\r\n" +
           ((startTime) ? " \"duration\": " + (String)(millis() - startTime) + ",\r\n" : "") + "" +
           ((timeStatus() != timeNotSet) ? " \"time\": \"" + (String)hour() + ":" + (String)minute() + "\",\r\n" : "") + "" +
           " \"on\": " + (String)(startTime ? "1" : "0") +
           "\r\n}";
  }

  bool isOn()
  {
    return startTime ? true : false;
  }

  void turnOn(ScheduleClass &skd)
  {
    turnOn(skd.getDuration());
  }

  void turnOn(unsigned int duration = 0)
  {
    if (duration)
      countdown.once_ms(duration * 60 * 1000, turnOffSwitch);

    onAction();
    startTime = millis();
    notify();
  }

  void turnOff()
  {
    offAction();
    startTime = 0;
    countdown.detach();
    notify();
  }

  void restart()
  {
    restartAction();
  }

  void reset()
  {
    resetAction();
  }

  void rename(const String &value)
  {
    if (Settings.setDeviceName(value.c_str()))
    {
      Settings.save();
      restart();
    }
  }

  void schedule(timeDayOfWeek_t day, int hours, int minutes, int duration, int enable)
  {
    Serial.println("[SWITCH] schedule(" + (String)day + ", " + (String)hours + ", " + (String)minutes + ", " + (String)duration + ", " + (String)enable + ")");

    if (enable == 1)
    {
      Schedule.disable();
    }

    ScheduleClass &skd = Schedule.get(day);

    if (hours != -1)
      skd.setHour(hours);

    if (minutes != -1)
      skd.setMinute(minutes);

    if (duration != -1)
      skd.setDuration(duration);

    if (enable == 1)
    {
      switch (day)
      {
      case dowMonday:
        skd.enable(turnOnMonday);
        break;
      case dowTuesday:
        skd.enable(turnOnTuesday);
        break;
      case dowWednesday:
        skd.enable(turnOnWednesday);
        break;
      case dowThursday:
        skd.enable(turnOnThursday);
        break;
      case dowFriday:
        skd.enable(turnOnFriday);
        break;
      case dowSaturday:
        skd.enable(turnOnSaturday);
        break;
      case dowSunday:
        skd.enable(turnOnSunday);
        break;
      }
    }
    else if (enable == 0)
    {
      skd.disable();
    }
  }

  void schedule(int hours, int minutes, int duration, int enable)
  {
    Serial.println("[SWITCH] schedule(" + (String)hours + ", " + (String)minutes + ", " + (String)duration + ", " + (String)enable + ")");

    if (hours != -1)
      Schedule.setHour(hours);

    if (minutes != -1)
      Schedule.setMinute(minutes);

    if (duration != -1)
      Schedule.setDuration(duration);

    if (enable == 1)
    {
      Schedule.enable(turnOnEveryday);
    }
    else if (enable == 0)
    {
      Schedule.disable();
    }
  }
};

extern SwitchClass Switch = SwitchClass();

void turnOffSwitch()
{
  Switch.turnOff();
}

void turnOnEveryday()
{
  if (timeStatus() != timeNotSet)
  {
    ScheduleClass &skd = Schedule;
    Switch.turnOn(skd);
  }
}

void turnOnDay(timeDayOfWeek_t day)
{
  if (timeStatus() != timeNotSet)
  {
    ScheduleClass &skd = Schedule.get(day);
    Switch.turnOn(skd);
  }
}

void turnOnMonday()
{
  turnOnDay(dowMonday);
}

void turnOnTuesday()
{
  turnOnDay(dowTuesday);
}

void turnOnWednesday()
{
  turnOnDay(dowWednesday);
}

void turnOnThursday()
{
  turnOnDay(dowThursday);
}

void turnOnFriday()
{
  turnOnDay(dowFriday);
}

void turnOnSaturday()
{
  turnOnDay(dowSaturday);
}

void turnOnSunday()
{
  turnOnDay(dowSunday);
}

#endif
