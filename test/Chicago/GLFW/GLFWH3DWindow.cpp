#include "GLFWH3DWindow.hpp"
#include "Horde3DUtils.h"
#include "Horde3D.h"
#include "GLFW.hpp"

#include <functional>
#include <map>

// Event representation:
// 
//   value;    // key token (when eventType = Util::KEY_PRESS or Util::KEY_RELEASE) or mouse button token (when eventType = MOUSE_BUTTON_PRESS or MOUSE_BUTTON_RELEASE)
//   mouseX;   // mouse curson position X (only set when eventType = MOUSE_MOVE)
//   mouseY;   // mouse curson position Y (only set when eventType = MOUSE_MOVE)
namespace GLFW
{
  H3DWindow::H3DWindow(Util::EventCache& eventCache, const std::string& title) :
    eventCache(eventCache),
    title(title),
    window(NULL)
  {

  }

  bool H3DWindow::Open(int width, int height, bool fullscreen)
  {
    // If already open, do not open again. Should call Close() first
    if (window) {
      return true;
    }

    if ((window = glfwCreateWindow(width, height, "H3DWindow", fullscreen? glfwGetPrimaryMonitor() : NULL, NULL))) {
      // Make key state stay on after the key is released
      glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
      // Disable vertical synchronization
      glfwSwapInterval(0);
      // Make this H3DWindow instance accessible from the key and mouse callbacks
      GLFW::RegisterWindow(window, *this);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      // Initialize rendering engine
      // We need the context to be current to initialize Horde3D.
      glfwMakeContextCurrent(window);

      if(!h3dInit())
      {
        GLFW::DeregisterWindow(window);
        glfwDestroyWindow(window); // This deregisters keyboard and mouse callbacks
        window = NULL;
        h3dutDumpMessages();
        return false;
      }

      // Set options
      h3dSetOption(H3DOptions::LoadTextures, 1);
      h3dSetOption(H3DOptions::TexCompression, 0);
      h3dSetOption(H3DOptions::MaxAnisotropy, 4);
      h3dSetOption(H3DOptions::ShadowMapSize, 2048);
      h3dSetOption(H3DOptions::FastAnimation, 0);
    }

    return (window != NULL);
  }

  void H3DWindow::Close()
  {
    if (window) {
      h3dRelease();
      GLFW::DeregisterWindow(window);
      glfwDestroyWindow(window); // This deregisters keyboard and mouse callbacks
      window = NULL;
    }
  }

  void H3DWindow::SwapBuffers()
  {
    if (window) {
      glfwSwapBuffers(window);
    }
  }

  void H3DWindow::ClearEvents()
  {
    eventCache.ClearEvents();
  }

  void H3DWindow::OnKeyEvent(int key, int, int action, int) {
    Util::EventType eventType = Util::KEY_PRESS;
    if (action == GLFW_PRESS) {
      eventType = Util::KEY_PRESS;
    } else if (action == GLFW_REPEAT) {
      eventType = Util::KEY_HOLD;
    } else if (action == GLFW_RELEASE) {
      eventType = Util::KEY_RELEASE;
    }
    eventCache.AddEvent(Util::Event(eventType, key, 0.0f, 0.0f));
  }

  void H3DWindow::OnMouseMoveEvent(double posX, double posY) {
    eventCache.AddEvent(Util::Event(Util::MOUSE_MOVE, 0, posX, posY));
  }
} // namespace GLFW
