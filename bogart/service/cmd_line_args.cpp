#include "bogart/service/cmd_line_args.hpp"

namespace bogart
{
namespace service
{
  cmd_line_args::cmd_line_args(unsigned int argc, char** argv)
  {
    for (unsigned int i = 0; i < argc - 1; i++) {
      options[argv[i]] = argv[i + 1];
    }

    options[argv[argc - 1]] = "";
  }

  cmd_line_args::~cmd_line_args()
  {

  }

  bool cmd_line_args::has_option(const std::string& option) const
  {
    return (options.find(option) != options.end());
  }

  std::string cmd_line_args::get_option_value(const std::string& option, const std::string& default_value) const
  {
    std::map<std::string, std::string>::const_iterator it = options.find(option);
    if (it != options.end()) {
      return it->second;
    }

    return default_value;
  }
}
}
