#include "bogart/async/timer.hpp"
#include "bogart/log/log.hpp"

#include <condition_variable>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <map>
#include <set>

namespace bogart
{
namespace async
{
  //------------------------------------------------------------------------------------------------
  //! Internal helper functions.
  //------------------------------------------------------------------------------------------------
  namespace
  {
    //----------------------------------------------------------------------------------------------
    //! Helper types
    //----------------------------------------------------------------------------------------------
    enum timer_state {
      IDLE      = 0,
      WAITING
    };

    typedef std::multimap<timer::time_point, std::reference_wrapper<timer>> timer_map;
    typedef timer_map::iterator timer_it;
    typedef std::set<timer::time_point> time_point_set;
    typedef time_point_set::iterator time_point_it;

    class timer_loop
    {
    public:
      timer_loop() :
        keep_running(true),
        thr(&timer_loop::loop, this) {

      }

      ~timer_loop() {
        try
        {
          stop();
          thr.join();
        }
        catch(std::exception& ex) {
          // We don't log since this is program finalization
        }
      }

      void add_timer(timer& t, timer::time_point deadline) {
        std::unique_lock<std::mutex> lock(mtx);
        timer_map::value_type value(deadline, t);
        timers.insert(value);
        cond.notify_one();
      }

      void remove_timer(timer& t) {
        // Iterate timers and save all matching keys
        std::unique_lock<std::mutex> lock(mtx);
        time_point_set ts;
        for (timer_it tit = timers.begin(); tit != timers.end(); tit++) {
          if (&tit->second.get() == &t) {
            ts.insert(tit->first);
          }
        }

        // Remove keys from map
        for (time_point_it kit = ts.begin(); kit != ts.end(); kit++) {
          timers.erase(*kit);
        }
      }

    private:
      void stop() {
        std::unique_lock<std::mutex> lock(mtx);
        keep_running = false;
        cond.notify_one();
      }

      void dispatch_ready_timers() {
        // Iterate timers and post the ones that are ready
        time_point_set ready_keys;
        timer::time_point now = std::chrono::high_resolution_clock::now();
        timer_it it = timers.begin();
        while (it != timers.end() && it->first <= now) {
          ready_keys.insert(it->first);
          timer& t = it->second;
          t.dispatch();
          it++;
        }

        // Remove ready timers from map
        for (auto k : ready_keys) {
          timers.erase(k);
        }
      }

      void loop() {
        std::unique_lock<std::mutex> lock(mtx);
        while (keep_running) {
          timer::time_point timeout = std::chrono::high_resolution_clock::now() + std::chrono::seconds(5);
          if (!timers.empty()) {
            timeout = timers.begin()->first;
          }
          cond.wait_until(lock, timeout);

          if (keep_running) {
            dispatch_ready_timers();
          }
        }

        cond.notify_one();
      }

      bool keep_running;
      timer_map timers;
      std::mutex mtx;
      std::condition_variable cond;
      std::thread thr;
    };

    //----------------------------------------------------------------------------------------------
    //! Global variables.
    //----------------------------------------------------------------------------------------------
    timer_loop loop;
  } // Anonymous namespace

  class timer::timer_impl
  {
  public:
    timer_impl(message_queue& queue) :
      queue(queue),
      state(IDLE) {

    }

    void set_state(timer_state s) {
      std::unique_lock<std::mutex> lock(mtx);
      state = s;
    }

    timer_state get_state() {
      std::unique_lock<std::mutex> lock(mtx);
      return state;
    }

    void set_handler(bogart::async::runnable_ptr h)
    {
      std::unique_lock<std::mutex> lock(mtx);
      handler = std::move(h);
    }

    //--------------------------------------------------------------------------------------------
    //! Member variables
    //--------------------------------------------------------------------------------------------
    message_queue& queue;
    timer::time_point deadline;
    bogart::async::runnable_ptr handler;
    timer_state state;
    std::mutex mtx;
  }; // class timer::timer_impl

  //------------------------------------------------------------------------------------------------
  //! Public member functions.
  //------------------------------------------------------------------------------------------------
  timer::timer(message_queue& queue) :
    impl(std::make_unique<timer::timer_impl>(queue)) {

  }

  timer::~timer() {
    timer_state state = impl->get_state();
    if (state == WAITING) {
      loop.remove_timer(*this);
    }
  }

  void timer::async_wait(time_point t, bogart::async::runnable_ptr handler) {
    if (impl->get_state() == IDLE) {
      impl->set_state(WAITING);
      impl->set_handler(std::move(handler));
      loop.add_timer(*this, t);
    }
  }

  void timer::dispatch() {
    std::unique_lock<std::mutex> lock(impl->mtx);
    impl->state = IDLE;
    impl->queue.post(std::move(impl->handler));
  }
} // namespace async
} // namespace bogart
