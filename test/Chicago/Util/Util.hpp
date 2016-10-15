#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <list>
#include <map>

namespace Util
{
  int ParseInt(const std::string& s);
  int ParseInt(const char* s);

  class CommandLineArguments
  {
  public:
    CommandLineArguments(unsigned int argc, char** argv);
    virtual ~CommandLineArguments();

    bool HasOption(const std::string& option) const;
    std::string GetOptionValue(const std::string& option) const;

  private:
    std::map<std::string, std::string> options;
  };

  enum EventType
  {
    KEY_PRESS = 0,
    KEY_HOLD,
    KEY_RELEASE,
    MOUSE_MOVE,
    MOUSE_BUTTON_PRESS,
    MOUSE_BUTTON_RELEASE
  };

  // The actual meaning of the values stored in this structure is defined by the user
  struct Event
  {
    Event(EventType eventType, int value, float mouseX, float mouseY);

    EventType eventType;
    int value;
    float mouseX;
    float mouseY;
  };

  typedef std::list<Event> EventList;
  typedef EventList::iterator EventIterator;

  class EventCache
  {
  public:
    EventCache();
    const EventList& GetEvents();
    void AddEvent(const Event& event);
    void ClearEvents();

  private:
    EventList events;
  };
}

#endif // UTIL_HPP
