#ifndef LOG_HPP
#define LOG_HPP

#include <string>

namespace Log
{
  enum LogLevel
  {
    NONE = 0,
    ERROR,
    DEBUG
  };

  void SetLogLevel(LogLevel logLevel);
  void Error(const std::string& message);
  void Error(const char* message);
  void Debug(const std::string& message);
  void Debug(const char* message);
}

#endif // LOG_HPP
