#include "bogart/async/timer.hpp"
#include "bogart/controller.hpp"
#include "bogart/stop_watch.hpp"
#include "bogart/fps_actor.hpp"
#include "bogart/log/log.hpp"
#include "bogart/view.hpp"

#include <iomanip>
#include <sstream>
#include <chrono>
#include <string>
#include <map>

namespace bogart
{
  //----------------------------------------------------------------------------------------------
  //! Constants
  //----------------------------------------------------------------------------------------------
  const view_settings VIEW_SETTINGS_1         = view_settings( 800,  600, false);
  const view_settings VIEW_SETTINGS_2         = view_settings(1024,  768, false);
  const view_settings VIEW_SETTINGS_3         = view_settings(1366,  768, false);
  const view_settings VIEW_SETTINGS_4         = view_settings(1600,  900, false);
  const view_settings VIEW_SETTINGS_5         = view_settings(1920, 1080,  true);
  const std::string OPTION_WIDTH              = "-width";
  const std::string OPTION_HEIGHT             = "-height";
  const std::string OPTION_FULLSCREEN         = "-fullscreen";

  //------------------------------------------------------------------------------------------------
  //! Internal helper functions and types.
  //------------------------------------------------------------------------------------------------
  namespace
  {
    enum controller_state
    {
      STATE_INIT_MODEL_WAIT = 0,
      STATE_OPEN_VIEW_WAIT,
      STATE_OPEN_VIEW_RESULT_WAIT,
      STATE_VIEW_OPENED,
      STATE_CONTROLLING,
      STATE_FAILURE,
      STATE_FINISHED,
    };

    view_settings get_settings_from_cmd_args(const service::cmd_line_args& args)
    {
      view_settings ret;
      std::stringstream ss;

      ss.clear();
      ss << args.get_option_value(OPTION_WIDTH, "1024");
      ss >> ret.window_width;

      ss.clear();
      ss << args.get_option_value(OPTION_HEIGHT, "768");
      ss >> ret.window_height;

      ret.fullscreen = args.has_option(OPTION_FULLSCREEN);

      return ret;
    }

    typedef std::map<key_code, view_settings> view_settings_map;

    view_settings_map s_view_settings;

    void fill_view_settings_map()
    {
      if (s_view_settings.empty()) {
        s_view_settings[KEY_F1] = view_settings( 800,  600, false);
        s_view_settings[KEY_F2] = view_settings(1024,  768, false);
        s_view_settings[KEY_F3] = view_settings(1366,  768, false);
        s_view_settings[KEY_F4] = view_settings(1600,  900, false);
        s_view_settings[KEY_F5] = view_settings(1920, 1080,  true);
      }
    }
  } // Anonymous namespace

  // We don't send self-messages from within the state machine unless we are in a transition
  // that is always followed by the same transtion, such as poll
  class controller::controller_impl
  {
  public:
    controller_impl(
      async::message_queue& m_queue,
      view& view,
      service::cmd_line_args& cmd_args) :
      m_queue(m_queue),
      m_view(view),
      m_cmd_args(cmd_args),
      m_timer(m_queue),
      m_player(),
      m_settings(get_settings_from_cmd_args(cmd_args)),
      m_is_paused(false),
      m_stats_enabled(false),
      m_last_mouse_x(0.0f),
      m_last_mouse_y(0.0f),
      m_stop_watch(),
      m_state(STATE_INIT_MODEL_WAIT)
    {

    }

    void init_model()
    {
      log::debug("controller: init_model");
      if (m_state == STATE_INIT_MODEL_WAIT) {
        // Initialize game state
        m_player.set_position(glm::vec3(12.75f, 2.0f, 0.1f));
        m_player.set_pitch(12.7f);
        m_player.set_yaw(88.0f);
        m_player.set_velocity(7.5f);
        m_is_paused = false;
        m_view.async_subscribe_to_events([=](event_vector_ptr events) {
          process_events(std::move(events));
        });

        m_state = STATE_OPEN_VIEW_WAIT;
      }
    }

    void open_view()
    {
      log::debug("controller: open_view");
      if (m_state == STATE_OPEN_VIEW_WAIT) {
        open_args_ptr args = std::make_unique<open_args>();
        args->settings = m_settings;
        args->handler = make_open_handler([=](bool is_window_open, const view_settings& current_settings) {
          on_open_view_result(is_window_open, current_settings);
        });
        m_view.async_open(std::move(args));
        m_state = STATE_OPEN_VIEW_RESULT_WAIT;
      }
    }

    void on_open_view_result(bool is_window_open, const view_settings& current_settings)
    {
      log::debug("controller: on_open_view_result");
      if (m_state == STATE_OPEN_VIEW_RESULT_WAIT) {
        if (is_window_open) {
          m_state = STATE_VIEW_OPENED;
          m_settings = current_settings;
          m_last_mouse_x = m_settings.window_width / 2.0f;
          m_last_mouse_y = m_settings.window_height / 2.0f;
          m_queue.post(async::make_callable([=](){ set_up(); }));
          m_queue.post(async::make_callable([=](){ poll(); }));
        } else {
          log::error("controller: could not open view, aborting");
          m_state = STATE_FAILURE;
        }
      }
    }

    void set_up()
    {
      log::debug("controller: set_up");
      if (m_state == STATE_VIEW_OPENED) {
        m_view.async_set_up();
        m_stop_watch.start();
        m_state = STATE_CONTROLLING;
      }
    }

    void process_events(event_vector_ptr events)
    {
      for (event_it it = events->begin(); it != events->end(); it++) {
        auto event = *it;
        if (event.type == EVENT_KEY_PRESS) {
          if (event.value == KEY_F6) {
            // F6 toggles stats
            m_stats_enabled = !m_stats_enabled;
            m_view.async_set_stats_enabled(m_stats_enabled);
          } else if (event.value == KEY_ESCAPE) {
            // ESCAPE exits the application
            m_queue.post(async::make_callable([=](){ tear_down(); }));
            m_queue.post(async::make_callable([=](){ close_view(); }));
          } else if (event.value == KEY_SPACE) {
            // SPACE pauses the simulation
            m_is_paused = ~m_is_paused;
          } else if (event.value == KEY_W) {
            // W moves forward
            m_player.start_moving_forward();
          } else if (event.value == KEY_S) {
            // S moves backward
            m_player.start_moving_backward();
          } else if (event.value == KEY_A) {
            // A strafes left
            m_player.start_strafing_left();
          } else if (event.value == KEY_D) {
            // D strafes right
            m_player.start_strafing_right();
          } else {
            // F1 through F5 change video modes
            fill_view_settings_map();
            auto it = s_view_settings.find(event.value);
            if (it != s_view_settings.end()) {
              m_settings = it->second;
              m_queue.post(async::make_callable([=](){ tear_down(); }));
              m_queue.post(async::make_callable([=](){ close_view(); }));
              m_queue.post(async::make_callable([=](){ open_view(); }));
            }
          }
        } else if (event.type == EVENT_KEY_RELEASE) {
          if (event.value == KEY_W || event.value == KEY_S) {
            m_player.stop_moving_forward_back();
          } else if (event.value == KEY_A || event.value == KEY_D) {
            m_player.stop_strafing();
          }
        } else if (event.type == EVENT_MOUSE_MOVE) {
          // Yaw rotates the camera around the Y axis counter-clockwise. Mouse X coordinates increase
          // to the right, so we use mouse motion to substract from yaw.
          float d_yaw = 0.1f * (event.mouse_x - m_last_mouse_x);
          m_last_mouse_x = event.mouse_x;
          m_player.set_yaw(m_player.get_yaw() - d_yaw);

          // Pitch rotates the camera around the X axis counter-clockwise. Mouse Y coordinates
          // increase down, so we use mouse motion to add to yaw.
          float d_pitch = 0.1f * (m_last_mouse_y - event.mouse_y);
          m_last_mouse_y = event.mouse_y;
          m_player.set_pitch(m_player.get_pitch() + d_pitch);

          m_player.log_status();
        }
      }
    }

    void update_simulation()
    {
      if (!m_is_paused) {
        m_stop_watch.update();
        float dt = m_stop_watch.get_dt();
        m_player.update(dt);
      }
    }

    void poll()
    {
      if (m_state == STATE_CONTROLLING) {
        // Update model
        update_simulation();

        // Update view
        glm::vec3 pos = m_player.get_position();
        float pitch = m_player.get_pitch();
        float yaw = m_player.get_yaw();
        m_view.async_update_camera(pos.x, pos.y, pos.z, pitch, yaw);

        // We schedule the poll method every 15 milliseconds. In practice this gives us a dt of
        // around 15.50 milliseconds (as returned by m_stop_watch.get_dt())
        m_state = STATE_CONTROLLING;
        async::timer::time_point now = std::chrono::high_resolution_clock::now();
        m_timer.async_wait(now + std::chrono::milliseconds(15), async::make_callable([=](){ poll(); }));
      }
    }

    void tear_down()
    {
      log::debug("controller: tear_down");
      if (m_state == STATE_CONTROLLING) {
        m_stop_watch.stop();
        m_view.async_tear_down();
        m_state = STATE_VIEW_OPENED;
      }
    }

    void close_view()
    {
      log::debug("controller: close_view");
      if (m_state == STATE_VIEW_OPENED) {
        m_view.async_close();
        m_state = STATE_OPEN_VIEW_WAIT;
      }
    }

    void fin_model()
    {
      log::debug("controller: fin_model");
      if (m_state == STATE_OPEN_VIEW_WAIT) {
        m_state = STATE_INIT_MODEL_WAIT;
      }
    }

    //----------------------------------------------------------------------------------------------
    //! Member variables
    //----------------------------------------------------------------------------------------------
    async::message_queue& m_queue;
    view& m_view;
    service::cmd_line_args& m_cmd_args;
    async::timer m_timer;
    fps_actor m_player;
    view_settings m_settings; // desired view settings (may or may not be what is currently set on the view)
    bool m_is_paused;
    bool m_stats_enabled;
    float m_last_mouse_x;
    float m_last_mouse_y;
    stop_watch m_stop_watch;
    controller_state m_state;
  }; // class controller::controller_impl

  //------------------------------------------------------------------------------------------------
  //! Public member functions.
  //------------------------------------------------------------------------------------------------
  controller::controller(
    async::message_queue& m_queue,
    view& view,
    service::cmd_line_args& args) :
    impl(std::make_unique<controller::controller_impl>(m_queue, view, args)) {

  }

  controller::~controller() {

  }

  void controller::async_call()
  {
    impl->m_queue.post(async::make_callable([&i = *impl](){ i.init_model(); }));
    impl->m_queue.post(async::make_callable([&i = *impl](){ i.open_view(); }));
    // Rest of the flow is scheduled from on_open_view_result(), after the window is opened
  }
} // namespace bogart
