#include "bogart/async/message_queue.hpp"
#include "bogart/log/log.hpp"

#include <condition_variable>
#include <stdexcept>
#include <mutex>
#include <list>

namespace bogart
{
namespace async
{
  //------------------------------------------------------------------------------------------------
  //! Internal helper functions.
  //------------------------------------------------------------------------------------------------
  namespace
  {
    void run_and_catch(runnable_ptr r) {
      try
      {
        r->exec();
      }
      catch(std::exception& ex) {
        log::error("Exception running runnable\n");
      }
      catch(...) {
        log::error("Unknown exception running runnable\n");
      }
    }
  } // Anonymous namespace

  class message_queue::message_queue_impl
  {
  public:
    message_queue_impl() {

    }

    bool wait_work(duration timeout) {
      std::unique_lock<std::mutex> lock(mtx);
      while (runnables.empty()) {
        if (more.wait_for(lock, timeout) == std::cv_status::timeout) {
          return false;
        }
      }

      return true;
    }

    runnable_ptr get_next() {
      std::unique_lock<std::mutex> lock(mtx);
      runnable_ptr ret;
      if (!runnables.empty()) {
        ret = std::move(runnables.front());
        runnables.pop_front();
      }

      return ret;
    }

    //----------------------------------------------------------------------------------------------
    //! Member variables
    //----------------------------------------------------------------------------------------------
    std::list<runnable_ptr> runnables;
    std::condition_variable more;
    std::mutex mtx;
  }; // class message_queue::message_queue_impl

  //------------------------------------------------------------------------------------------------
  //! Public member functions.
  //------------------------------------------------------------------------------------------------
  message_queue::message_queue() : impl(std::make_unique<message_queue::message_queue_impl>()) {

  }

  message_queue::~message_queue() {

  }

  void message_queue::post(runnable_ptr r) {
    std::unique_lock<std::mutex> lock(impl->mtx);
    impl->runnables.push_back(std::move(r));
    impl->more.notify_all();
  }

  void message_queue::run(duration timeout)
  {
    while (impl->wait_work(timeout)) {
      runnable_ptr r;
      while (r = impl->get_next()) {
        run_and_catch(std::move(r));
      }
    }
  }
} // namespace async
} // namespace bogart
