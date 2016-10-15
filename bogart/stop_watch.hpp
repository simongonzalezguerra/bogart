#ifndef STOP_WATCH_HPP
#define STOP_WATCH_HPP

#include <memory>

namespace bogart
{
  //------------------------------------------------------------------------------------------------
  //! @class stop_watch
  //! @ingroup async
  //!
  //! Thread-safety: calling methods in this class from different threads on different instances is
  //! safe. Calling methods in this class from different threads on the same instance is not safe.
  //------------------------------------------------------------------------------------------------
  class stop_watch
  {
  public:
    //----------------------------------------------------------------------------------------------
    //! Constructor
    //----------------------------------------------------------------------------------------------
    stop_watch();

    //----------------------------------------------------------------------------------------------
    //! Destructor
    //----------------------------------------------------------------------------------------------
    ~stop_watch();

    //----------------------------------------------------------------------------------------------
    //! Member functions
    //----------------------------------------------------------------------------------------------
    void start();
    void stop();
    void reset();
    void update();
    float get_elapsed_time();
    float get_dt();

  private:
    class stop_watch_impl;                            //!< implementation class (Pimpl idiom)
    std::unique_ptr<stop_watch_impl> impl;            //!< pointer to implementation (Pimpl idiom)
  }; // class stop_watch
} // namespace bogart

#endif // STOP_WATCH_HPP
