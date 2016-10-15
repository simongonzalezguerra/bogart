#ifndef TIMER_HPP
#define TIMER_HPP

#include "bogart/async/message_queue.hpp"

#include <chrono>
#include <memory>

namespace bogart
{
namespace async
{
  //------------------------------------------------------------------------------------------------
  //! @class timer
  //! @ingroup async
  //!
  //------------------------------------------------------------------------------------------------
  class timer
  {
  public:
    //----------------------------------------------------------------------------------------------
    //! Type for handlers
    //----------------------------------------------------------------------------------------------
    typedef std::chrono::high_resolution_clock::time_point time_point;

    //----------------------------------------------------------------------------------------------
    //! Member functions
    //----------------------------------------------------------------------------------------------
    timer(message_queue& q);
    ~timer();
    void async_wait(time_point t, bogart::async::runnable_ptr handler);
    void dispatch();

  private:
    class timer_impl;                            //!< implementation class (Pimpl idiom)
    std::unique_ptr<timer_impl> impl;            //!< pointer to implementation (Pimpl idiom)
  }; // class timer
} // namespace async
} // namesace bogart

#endif // TIMER_HPP
