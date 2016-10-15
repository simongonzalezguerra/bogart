#ifndef CMD_LINE_ARGS_HPP
#define CMD_LINE_ARGS_HPP

#include <string>
#include <map>

namespace bogart
{
namespace service
{
  class cmd_line_args
  {
  public:
    cmd_line_args(unsigned int argc, char** argv);
    virtual ~cmd_line_args();

    bool has_option(const std::string& option) const;
    std::string get_option_value(const std::string& option, const std::string& default_value) const;

  private:
    std::map<std::string, std::string> options;
  };
}
}

#endif
