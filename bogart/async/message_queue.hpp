#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <chrono>
#include <memory>

namespace bogart
{
namespace async
{
  class runnable
  {
  public:
    runnable() {}
    virtual ~runnable() {}
    virtual void exec() = 0;
  };

  typedef std::unique_ptr<runnable> runnable_ptr;

  //------------------------------------------------------------------------------------------------
  //! @class callable_wrapper
  //! @ingroup async
  //!
  //! We define callable_wrapper as a templated class to act as a generic container of callable
  //! objects. We want to be able to schedule lambdas that have unique_ptrs captured inside of them,
  //! so callable_wraper acquires its parameter via move semantics.
  //!
  //! Note we wouldn't be able to do this using std::function as wrapper. If we used std::function,
  //! we would be able to store the pointer inside the lambda via move-capure, but then the lambda
  //! would be non copyable, which doesn't compile because std::function requires its containeed
  //! callable object to be copyable.
  //!
  //! See the following for more information:
  //! http://stackoverflow.com/questions/31866217/why-does-lambda-init-capture-not-work-for-unique-ptr
  //! http://stackoverflow.com/questions/25330716/move-only-version-of-stdfunction
  //!
  //! A utility template function make_callable is provided to build instances of callable_wrapper.
  //! Use it when calling message_queue::post() with anything that isn't already a unique_ptr.
  //------------------------------------------------------------------------------------------------
  template<typename callable>
  class callable_wrapper : public runnable
  {
  public:
    callable_wrapper(callable c) : c(std::move(c))
    {

    }

    virtual void exec()
    {
      c();
    }

  private:
    callable c;
  };

  template<typename callable>
  runnable_ptr make_callable(callable c)
  {
    return std::make_unique<callable_wrapper<callable>>(std::move(c));
  }

  //------------------------------------------------------------------------------------------------
  //! @class message_queue
  //! @ingroup async
  //!
  //! Thread-safety: all public methods are thread-safe.
  //------------------------------------------------------------------------------------------------
  class message_queue
  {
  public:
    //----------------------------------------------------------------------------------------------
    //! Types
    //----------------------------------------------------------------------------------------------
    typedef std::chrono::high_resolution_clock::duration duration;

    //----------------------------------------------------------------------------------------------
    //! Constructor
    //----------------------------------------------------------------------------------------------
    message_queue();

    //----------------------------------------------------------------------------------------------
    //! Destructor
    //----------------------------------------------------------------------------------------------
    ~message_queue();

    //----------------------------------------------------------------------------------------------
    //! Member functions
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    //! @brief Queues a runnable to run immediately.
    //! @brief It is guaranteed that the runnable will only run from one of the threads that are
    //! currently calling run().
    //----------------------------------------------------------------------------------------------
    void post(runnable_ptr r);

    //----------------------------------------------------------------------------------------------
    //! @brief Executes the event processing loop, running handlers as soon as they are posted.
    //! @param timeout Timeout for handler waits. If no handlers are posted for this duration the
    //!  method returns.
    //! @remarks If handlers are posted continually such that the timeout is never reached this
    //!  method will continue running.
    //----------------------------------------------------------------------------------------------
    void run(duration timeout);

  private:
    class message_queue_impl;                            //!< implementation class (Pimpl idiom)
    std::unique_ptr<message_queue_impl> impl;            //!< pointer to implementation (Pimpl idiom)
  }; // class message_queue
} // namespace async
} // namespace bogart

#endif // MESSAGE_QUEUE_HPP
