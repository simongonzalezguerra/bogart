#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "bogart/service/cmd_line_args.hpp"
#include "bogart/async/message_queue.hpp"
#include "bogart/view.hpp"

#include <memory>

namespace bogart
{
  //------------------------------------------------------------------------------------------------
  //! @class controller
  //! @ingroup bogart
  //!
  //! Thread-safety: all public methods are thread-safe.
  //------------------------------------------------------------------------------------------------
  class controller
  {
  public:
    //----------------------------------------------------------------------------------------------
    //! Types
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    //! Constructor
    //----------------------------------------------------------------------------------------------
    controller(async::message_queue& logic_queue, view& view, service::cmd_line_args& args);

    //----------------------------------------------------------------------------------------------
    //! Destructor
    //----------------------------------------------------------------------------------------------
    ~controller();

    //----------------------------------------------------------------------------------------------
    //! Member functions
    //----------------------------------------------------------------------------------------------
    void async_call();

  private:
    class controller_impl;                            //!< implementation class (Pimpl idiom)
    std::unique_ptr<controller_impl> impl;            //!< pointer to implementation (Pimpl idiom)
  }; // class controller
} // namespace bogart

#endif // CONTROLLER_HPP
