#ifndef LOG_HPP
#define LOG_HPP

#include <string>

namespace bogart
{
namespace log
{
  enum log_level
  {
    NONE = 0,
    ERROR,
    DEBUG
  };

  void set_log_level(log_level level);
  void error(const std::string& message);
  void error(const char* message);
  void debug(const std::string& message);
  void debug(const char* message);
} // namespace log
}

#endif // LOG_HPP
