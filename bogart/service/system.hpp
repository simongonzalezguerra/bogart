#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "bogart/event.hpp"

namespace bogart
{
namespace service
{
  // Thread-safety: methods in this class can only be called from the main thread
  class system
  {
  public:
    system();
    ~system();
    bool open_window(unsigned int width, unsigned int heigth, bool fullscreen);
    void close_window();
    event_vector_ptr poll_events();
    void swap_buffers();
  }; // class system
} // namespace service
}

#endif
