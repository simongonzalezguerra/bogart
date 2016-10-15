#include "Util.hpp"

#include <sstream>

namespace Util
{
  int ParseInt(const std::string& s)
  {
    int ret = 0;
    std::istringstream is(s);
    is >> ret;

    return ret;
  }

  int ParseInt(const char* s)
  {
    int ret = 0;
    std::istringstream is(s);
    is >> ret;

    return ret;
  }

  CommandLineArguments::CommandLineArguments(unsigned int argc, char** argv)
  {
    for (unsigned int i = 0; i < argc - 1; i++) {
      options[argv[i]] = argv[i + 1];
    }

    options[argv[argc - 1]] = "";
  }

  CommandLineArguments::~CommandLineArguments()
  {

  }

  bool CommandLineArguments::HasOption(const std::string& option) const
  {
    return (options.find(option) != options.end());
  }

  std::string CommandLineArguments::GetOptionValue(const std::string& option) const
  {
    if (options.find(option) != options.end()) {
      return options.at(option);
    }

    return "";
  }

  Event::Event(EventType e, int v, float x, float y) :
    eventType(e), value(v), mouseX(x), mouseY(y) {

  }

  EventCache::EventCache()
  {

  } 
   
  const EventList& EventCache::GetEvents()
  {
    return events;
  }

  void EventCache::AddEvent(const Event& event)
  {
    events.push_back(event);
  }

  void EventCache::ClearEvents() {
    events.clear();
  }
}
