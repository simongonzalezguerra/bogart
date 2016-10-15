#include "Log.hpp"
#include <iostream>


namespace Log
{
  namespace
  {
    LogLevel logLevel = NONE;
  
    template<typename T>
    void LogMessage(LogLevel l, const T& value) {
      if (l <= logLevel) {
        std::cout << value << "\n";
      }
    }
  }

  void SetLogLevel(LogLevel l) {
    logLevel = l;
  }

  void Error(const std::string& message) {
    LogMessage(ERROR, message);
  }

  void Error(const char* message) {
    LogMessage(ERROR, message);
  }

  void Debug(const std::string& message) {
    LogMessage(DEBUG, message);
  }

  void Debug(const char* message) {
    LogMessage(DEBUG, message);
  }
}
