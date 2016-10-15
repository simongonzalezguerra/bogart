#include "bogart/service/system.hpp"
#include "bogart/log/log.hpp"
#include "glfw3.h"

#include <sstream>
#include <vector>
#include <map>

namespace bogart
{
namespace service
{
  namespace
  {
    //----------------------------------------------------------------------------------------------
    //! Types
    //----------------------------------------------------------------------------------------------
    typedef std::map<int, key_code> key_code_map;
    typedef key_code_map::iterator key_code_it;
    typedef std::map<int, event_type> key_event_type_map;
    typedef key_event_type_map::iterator event_type_it;
    typedef std::map<int, std::string> error_code_map;

    //----------------------------------------------------------------------------------------------
    //! Internal global variables
    //----------------------------------------------------------------------------------------------
    event_vector_ptr events;
    GLFWwindow* window = NULL;
    bool ok = false;
    key_code_map key_codes;         // map from GLFW key value to key_code value
    key_event_type_map event_types; // map from GLFW key action value to event_type value
    error_code_map error_codes;     // map from GLFW error codes to their names

    //----------------------------------------------------------------------------------------------
    //! Internal functions
    //----------------------------------------------------------------------------------------------
    void fill_key_code_map()
    {
      if (key_codes.empty()) {
        key_codes[GLFW_KEY_UNKNOWN]       = KEY_UNKNOWN;
        key_codes[GLFW_KEY_SPACE]         = KEY_SPACE;
        key_codes[GLFW_KEY_APOSTROPHE]    = KEY_APOSTROPHE;
        key_codes[GLFW_KEY_COMMA]         = KEY_COMMA;
        key_codes[GLFW_KEY_MINUS]         = KEY_MINUS;
        key_codes[GLFW_KEY_PERIOD]        = KEY_PERIOD;
        key_codes[GLFW_KEY_SLASH]         = KEY_SLASH;
        key_codes[GLFW_KEY_0]             = KEY_0;
        key_codes[GLFW_KEY_1]             = KEY_1;
        key_codes[GLFW_KEY_2]             = KEY_2;
        key_codes[GLFW_KEY_3]             = KEY_3;
        key_codes[GLFW_KEY_4]             = KEY_4;
        key_codes[GLFW_KEY_5]             = KEY_5;
        key_codes[GLFW_KEY_6]             = KEY_6;
        key_codes[GLFW_KEY_7]             = KEY_7;
        key_codes[GLFW_KEY_8]             = KEY_8;
        key_codes[GLFW_KEY_9]             = KEY_9;
        key_codes[GLFW_KEY_SEMICOLON]     = KEY_SEMICOLON;
        key_codes[GLFW_KEY_EQUAL]         = KEY_EQUAL;
        key_codes[GLFW_KEY_A]             = KEY_A;
        key_codes[GLFW_KEY_B]             = KEY_B;
        key_codes[GLFW_KEY_C]             = KEY_C;
        key_codes[GLFW_KEY_D]             = KEY_D;
        key_codes[GLFW_KEY_E]             = KEY_E;
        key_codes[GLFW_KEY_F]             = KEY_F;
        key_codes[GLFW_KEY_G]             = KEY_G;
        key_codes[GLFW_KEY_H]             = KEY_H;
        key_codes[GLFW_KEY_I]             = KEY_I;
        key_codes[GLFW_KEY_J]             = KEY_J;
        key_codes[GLFW_KEY_K]             = KEY_K;
        key_codes[GLFW_KEY_L]             = KEY_L;
        key_codes[GLFW_KEY_M]             = KEY_M;
        key_codes[GLFW_KEY_N]             = KEY_N;
        key_codes[GLFW_KEY_O]             = KEY_O;
        key_codes[GLFW_KEY_P]             = KEY_P;
        key_codes[GLFW_KEY_Q]             = KEY_Q;
        key_codes[GLFW_KEY_R]             = KEY_R;
        key_codes[GLFW_KEY_S]             = KEY_S;
        key_codes[GLFW_KEY_T]             = KEY_T;
        key_codes[GLFW_KEY_U]             = KEY_U;
        key_codes[GLFW_KEY_V]             = KEY_V;
        key_codes[GLFW_KEY_W]             = KEY_W;
        key_codes[GLFW_KEY_X]             = KEY_X;
        key_codes[GLFW_KEY_Y]             = KEY_Y;
        key_codes[GLFW_KEY_Z]             = KEY_Z;
        key_codes[GLFW_KEY_LEFT_BRACKET]  = KEY_LEFT_BRACKET;
        key_codes[GLFW_KEY_BACKSLASH]     = KEY_BACKSLASH;
        key_codes[GLFW_KEY_RIGHT_BRACKET] = KEY_RIGHT_BRACKET;
        key_codes[GLFW_KEY_GRAVE_ACCENT]  = KEY_GRAVE_ACCENT;
        key_codes[GLFW_KEY_WORLD_1]       = KEY_WORLD_1;
        key_codes[GLFW_KEY_WORLD_2]       = KEY_WORLD_2;
        key_codes[GLFW_KEY_ESCAPE]        = KEY_ESCAPE;
        key_codes[GLFW_KEY_ENTER]         = KEY_ENTER;
        key_codes[GLFW_KEY_TAB]           = KEY_TAB;
        key_codes[GLFW_KEY_BACKSPACE]     = KEY_BACKSPACE;
        key_codes[GLFW_KEY_INSERT]        = KEY_INSERT;
        key_codes[GLFW_KEY_DELETE]        = KEY_DELETE;
        key_codes[GLFW_KEY_RIGHT]         = KEY_RIGHT;
        key_codes[GLFW_KEY_LEFT]          = KEY_LEFT;
        key_codes[GLFW_KEY_DOWN]          = KEY_DOWN;
        key_codes[GLFW_KEY_UP]            = KEY_UP;
        key_codes[GLFW_KEY_PAGE_UP]       = KEY_PAGE_UP;
        key_codes[GLFW_KEY_PAGE_DOWN]     = KEY_PAGE_DOWN;
        key_codes[GLFW_KEY_HOME]          = KEY_HOME;
        key_codes[GLFW_KEY_END]           = KEY_END;
        key_codes[GLFW_KEY_CAPS_LOCK]     = KEY_CAPS_LOCK;
        key_codes[GLFW_KEY_SCROLL_LOCK]   = KEY_SCROLL_LOCK;
        key_codes[GLFW_KEY_NUM_LOCK]      = KEY_NUM_LOCK;
        key_codes[GLFW_KEY_PRINT_SCREEN]  = KEY_PRINT_SCREEN;
        key_codes[GLFW_KEY_PAUSE]         = KEY_PAUSE;
        key_codes[GLFW_KEY_F1]            = KEY_F1;
        key_codes[GLFW_KEY_F2]            = KEY_F2;
        key_codes[GLFW_KEY_F3]            = KEY_F3;
        key_codes[GLFW_KEY_F4]            = KEY_F4;
        key_codes[GLFW_KEY_F5]            = KEY_F5;
        key_codes[GLFW_KEY_F6]            = KEY_F6;
        key_codes[GLFW_KEY_F7]            = KEY_F7;
        key_codes[GLFW_KEY_F8]            = KEY_F8;
        key_codes[GLFW_KEY_F9]            = KEY_F9;
        key_codes[GLFW_KEY_F10]           = KEY_F10;
        key_codes[GLFW_KEY_F11]           = KEY_F11;
        key_codes[GLFW_KEY_F12]           = KEY_F12;
        key_codes[GLFW_KEY_F13]           = KEY_F13;
        key_codes[GLFW_KEY_F14]           = KEY_F14;
        key_codes[GLFW_KEY_F15]           = KEY_F15;
        key_codes[GLFW_KEY_F16]           = KEY_F16;
        key_codes[GLFW_KEY_F17]           = KEY_F17;
        key_codes[GLFW_KEY_F18]           = KEY_F18;
        key_codes[GLFW_KEY_F19]           = KEY_F19;
        key_codes[GLFW_KEY_F20]           = KEY_F20;
        key_codes[GLFW_KEY_F21]           = KEY_F21;
        key_codes[GLFW_KEY_F22]           = KEY_F22;
        key_codes[GLFW_KEY_F23]           = KEY_F23;
        key_codes[GLFW_KEY_F24]           = KEY_F24;
        key_codes[GLFW_KEY_F25]           = KEY_F25;
        key_codes[GLFW_KEY_KP_0]          = KEY_KP_0;
        key_codes[GLFW_KEY_KP_1]          = KEY_KP_1;
        key_codes[GLFW_KEY_KP_2]          = KEY_KP_2;
        key_codes[GLFW_KEY_KP_3]          = KEY_KP_3;
        key_codes[GLFW_KEY_KP_4]          = KEY_KP_4;
        key_codes[GLFW_KEY_KP_5]          = KEY_KP_5;
        key_codes[GLFW_KEY_KP_6]          = KEY_KP_6;
        key_codes[GLFW_KEY_KP_7]          = KEY_KP_7;
        key_codes[GLFW_KEY_KP_8]          = KEY_KP_8;
        key_codes[GLFW_KEY_KP_9]          = KEY_KP_9;
        key_codes[GLFW_KEY_KP_DECIMAL]    = KEY_KP_DECIMAL;
        key_codes[GLFW_KEY_KP_DIVIDE]     = KEY_KP_DIVIDE;
        key_codes[GLFW_KEY_KP_MULTIPLY]   = KEY_KP_MULTIPLY;
        key_codes[GLFW_KEY_KP_SUBTRACT]   = KEY_KP_SUBTRACT;
        key_codes[GLFW_KEY_KP_ADD]        = KEY_KP_ADD;
        key_codes[GLFW_KEY_KP_ENTER]      = KEY_KP_ENTER;
        key_codes[GLFW_KEY_KP_EQUAL]      = KEY_KP_EQUAL;
        key_codes[GLFW_KEY_LEFT_SHIFT]    = KEY_LEFT_SHIFT;
        key_codes[GLFW_KEY_LEFT_CONTROL]  = KEY_LEFT_CONTROL;
        key_codes[GLFW_KEY_LEFT_ALT]      = KEY_LEFT_ALT;
        key_codes[GLFW_KEY_LEFT_SUPER]    = KEY_LEFT_SUPER;
        key_codes[GLFW_KEY_RIGHT_SHIFT]   = KEY_RIGHT_SHIFT;
        key_codes[GLFW_KEY_RIGHT_CONTROL] = KEY_RIGHT_CONTROL;
        key_codes[GLFW_KEY_RIGHT_ALT]     = KEY_RIGHT_ALT;
        key_codes[GLFW_KEY_RIGHT_SUPER]   = KEY_RIGHT_SUPER;
        key_codes[GLFW_KEY_MENU]          = KEY_MENU;
        key_codes[GLFW_KEY_LAST]          = KEY_LAST;
      }
    }

    void fill_error_code_map()
    {
      if (error_codes.empty()) {
        error_codes[GLFW_NOT_INITIALIZED]     = "GLFW_NOT_INITIALIZED";
        error_codes[GLFW_NO_CURRENT_CONTEXT]  = "GLFW_NO_CURRENT_CONTEXT";
        error_codes[GLFW_INVALID_ENUM]        = "GLFW_INVALID_ENUM";
        error_codes[GLFW_INVALID_VALUE]       = "GLFW_INVALID_VALUE";
        error_codes[GLFW_OUT_OF_MEMORY]       = "GLFW_OUT_OF_MEMORY";
        error_codes[GLFW_API_UNAVAILABLE]     = "GLFW_API_UNAVAILABLE";
        error_codes[GLFW_VERSION_UNAVAILABLE] = "GLFW_VERSION_UNAVAILABLE";
        error_codes[GLFW_PLATFORM_ERROR]      = "GLFW_PLATFORM_ERROR";
        error_codes[GLFW_FORMAT_UNAVAILABLE]  = "GLFW_FORMAT_UNAVAILABLE";
        //error_codes[GLFW_NO_WINDOW_CONTEXT]   = "GLFW_NO_WINDOW_CONTEXT"; Not defined in our version of GLFW
      }
    }

    void fill_event_type_map() {
      if (event_types.empty()) {
        event_types[GLFW_PRESS]   = EVENT_KEY_PRESS;
        event_types[GLFW_REPEAT]  = EVENT_KEY_HOLD;
        event_types[GLFW_RELEASE] = EVENT_KEY_RELEASE;
      }
    }

    void key_callback(GLFWwindow*, int key, int, int action, int)
    {
      if (ok && window) {
        // Translate GLFW key code to event_queue's enum
        fill_key_code_map();
        key_code key_code = KEY_UNKNOWN;
        key_code_it kit = key_codes.find(key);
        if (kit != key_codes.end()) {
          key_code = kit->second;
        }

        // Translate GLFW key action value to an event type
        fill_event_type_map();
        event_type event_type = EVENT_KEY_PRESS;
        event_type_it eit = event_types.find(action);
        if (eit != event_types.end()) {
          event_type = eit->second;
        }

        if (events) {
          events->push_back(event(event_type, key_code, 0.0f, 0.0f));
        }
      }
    }

    void mouse_move_callback(GLFWwindow*, double x, double y)
    {
      if (events) {
        events->push_back(event(EVENT_MOUSE_MOVE, KEY_UNKNOWN, x, y));
      }
    }

    void error_callback(int error_code, const char* description)
    {
      fill_error_code_map();
      log::debug("GLFW error callback\n");
      std::ostringstream os;
      os << "GLFW error " << error_codes[error_code] << ", " << description << "\n";
      log::error(os.str());
    }
  }


  //------------------------------------------------------------------------------------------------
  //! Public member functions.
  //------------------------------------------------------------------------------------------------
  system::system()
  {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
      log::error("Could not initialize GLFW\n");
      return;
    }
    ok = true;
    log::debug("Initialized GLFW");
  }

  system::~system()
  {
    if (ok) {
      glfwTerminate();
      log::debug("Finalized GLFW");
    }
  }

  bool system::open_window(unsigned int width, unsigned int height, bool fullscreen)
  {
    // If already open, do not open again. Should call close_window() first
    if (window) {
      log::error("Warning: calling system::open_window when the window is already open. Doing nothing, should call system::close_window first\n");
      return false;
    }

    if ((window = glfwCreateWindow(width, height, "H3DWindow", fullscreen? glfwGetPrimaryMonitor() : NULL, NULL))) {
      // Make key state stay on after the key is released
      glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
      // Register keyboard and mouse callbacks
      glfwSetKeyCallback(window, key_callback);
      glfwSetCursorPosCallback(window, mouse_move_callback);
      // Hide mouse cursor and center it
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      // Make OpenGL context current in the calling thread
      glfwMakeContextCurrent(window);
      // Disable vertical synchronization (results in a noticeable fps gain). This needs to be done
      // after calling glfwMakeContextCurrent, since it acts on the current context, and the context
      // created with glfwCreateWindow is not current until we make it explicitly so with
      // glfwMakeContextCurrent
      glfwSwapInterval(0);
    }

    return (window != NULL);
  }

  void system::close_window()
  {
    if (window) {
      glfwDestroyWindow(window); // This deregisters keyboard and mouse callbacks
      window = NULL;
    }
  }

  event_vector_ptr system::poll_events()
  {
    events = std::make_unique<event_vector>();
    glfwPollEvents();
    return std::move(events);
  }

  void system::swap_buffers()
  {
    if (window) {
      glfwSwapBuffers(window);
    }
  }
} // namespace service
} // namespace bogart
