#ifndef VIEW_HPP
#define VIEW_HPP

#include "bogart/service/cmd_line_args.hpp"
#include "bogart/async/message_queue.hpp"
#include "bogart/service/system.hpp"

#include <memory>

namespace bogart
{
  struct view_settings
  {
    view_settings() : window_width(0), window_height(0), fullscreen(false)
    {

    }

    view_settings(unsigned int window_width, unsigned int window_height, bool fullscreen) :
      window_width(window_width), window_height(window_height), fullscreen(fullscreen)
    {

    }

    unsigned int window_width;
    unsigned int window_height;
    bool fullscreen;
  };

  class open_handler
  {
  public:
    open_handler() {}
    virtual ~open_handler() {}
    virtual void on_open(bool is_window_open, const view_settings& current_settings) = 0;
  };

  typedef std::unique_ptr<open_handler> open_handler_ptr;

  template<typename callable>
  class generic_open_handler : public open_handler
  {
  public:
    generic_open_handler(callable c) : c(std::move(c)) {}
    virtual void on_open(bool is_window_open, const view_settings& current_settings)
    {
      c(is_window_open, current_settings);
    }

  private:
    callable c;
  };

  template<typename callable>
  open_handler_ptr make_open_handler(callable c)
  {
    return std::make_unique<generic_open_handler<callable>>(std::move(c));
  }

  struct open_args
  {
    open_args() {}

    view_settings settings;
    open_handler_ptr handler;
  };

  typedef std::unique_ptr<open_args> open_args_ptr;

  //------------------------------------------------------------------------------------------------
  //! @class view
  //! @ingroup bogart
  //!
  //! Thread-safety: all public methods are thread-safe.
  //------------------------------------------------------------------------------------------------
  class view
  {
  public:
    //----------------------------------------------------------------------------------------------
    //! Constructor
    //----------------------------------------------------------------------------------------------
    view(async::message_queue& render_queue,
         async::message_queue& logic_queue,
         service::system& system,
         service::cmd_line_args& args);

    //----------------------------------------------------------------------------------------------
    //! Destructor
    //----------------------------------------------------------------------------------------------
    ~view();

    //----------------------------------------------------------------------------------------------
    //! Member functions
    //----------------------------------------------------------------------------------------------
    void async_open(open_args_ptr args);
    void async_set_up();
    void async_tear_down();
    void async_close();
    void async_update_camera(float x, float y, float z, float rx, float ry);
    void async_set_stats_enabled(bool enable);
    void async_subscribe_to_events(const event_handler& handler);

  private:
    class view_impl;                            //!< implementation class (Pimpl idiom)
    std::unique_ptr<view_impl> impl;            //!< pointer to implementation (Pimpl idiom)
  }; // class view
} // namespace bogart

#endif // VIEW_HPP
