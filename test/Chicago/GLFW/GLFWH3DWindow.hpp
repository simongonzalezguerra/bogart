#ifndef GLFW_WINDOW_HPP
#define GLFW_WINDOW_HPP

#include "Util.hpp"
#include "glfw3.h"

#include <string>
#include <list>

namespace GLFW
{
  class H3DWindow
  {
  public:
    H3DWindow(Util::EventCache& eventCache, const std::string& title);
    bool Open(int width, int height, bool fullscreen);
    void Close();
    void SwapBuffers();
    void ClearEvents();
    void OnKeyEvent(int key, int, int action, int);
    void OnMouseMoveEvent(double posX, double posY);

  private:
    Util::EventCache& eventCache;
    std::string title;
    GLFWwindow* window;
  };
} // namespace GLFW

#endif // GLFW_WINDOW_HPP
