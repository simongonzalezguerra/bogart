#include "bogart/stop_watch.hpp"

#include <chrono>

namespace bogart
{
  //------------------------------------------------------------------------------------------------
  //! Internal helper functions.
  //------------------------------------------------------------------------------------------------
  namespace
  {
    typedef std::chrono::high_resolution_clock clock;
    typedef clock::time_point time_point;
    typedef std::chrono::duration<float> duration;
  } // Anonymous namespace

  class stop_watch::stop_watch_impl
  {
  public:
    stop_watch_impl() : physical_time(), elapsed_time(), dt(), running(false) {

    }

    //----------------------------------------------------------------------------------------------
    //! Member variables
    //----------------------------------------------------------------------------------------------
    time_point physical_time;
    duration elapsed_time;
    duration dt;
    bool running;
  }; // class stop_watch::stop_watch_impl

  //------------------------------------------------------------------------------------------------
  //! Public member functions.
  //------------------------------------------------------------------------------------------------
  stop_watch::stop_watch() : impl(std::make_unique<stop_watch::stop_watch_impl>()) {

  }

  stop_watch::~stop_watch() {

  }

  void stop_watch::start()
  {
    if (!impl->running) {
      impl->running = true;
      impl->physical_time = clock::now();
    }
  }

  void stop_watch::stop()
  {
    if (impl->running) {
      impl->running = false;
    }
  }

  void stop_watch::reset()
  {
    impl->elapsed_time = duration();
    impl->dt = duration();
    impl->running = false;
  }

  void stop_watch::update()
  {
    if (impl->running) {
      time_point now = clock::now();
      impl->dt = now - impl->physical_time;
      impl->physical_time = now;
      impl->elapsed_time += impl->dt;
    }
  }

  float stop_watch::get_elapsed_time()
  {
    return impl->elapsed_time.count();
  }

  float stop_watch::get_dt()
  {
    return impl->dt.count();
  }
}
