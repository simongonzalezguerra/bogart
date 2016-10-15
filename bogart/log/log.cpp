#include "bogart/log/log.hpp"

#include <iostream>

namespace bogart
{
namespace log
{
  namespace
  {
    log_level level = NONE;

    template<typename T>
    void log_message(log_level l, const T& value) {
      if (l <= level) {
        std::cout << value << "\n";
      }
    }
  }

  void set_log_level(log_level l) {
    level = l;
  }

  void error(const std::string& message) {
    log_message(ERROR, message);
  }

  void error(const char* message) {
    log_message(ERROR, message);
  }

  void debug(const std::string& message) {
    log_message(DEBUG, message);
  }

  void debug(const char* message) {
    log_message(DEBUG, message);
  }
} // namespace log
}
