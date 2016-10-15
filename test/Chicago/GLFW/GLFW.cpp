#include "GLFW.hpp"
#include "Log.hpp"

#include <functional>

namespace GLFW
{
  namespace
  {
    float stopTime       = 0.0f;
    float getTime        = 0.0f;
    bool timerRunning    = false;
    bool ok              = false;

    // Mapping of window instances to H3DWindow instances
    typedef std::map<GLFWwindow*, std::reference_wrapper<H3DWindow>> InstanceMap;
    InstanceMap instanceMap;

    // Keyboard event capture
    void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods) {
      // Forward the event to the H3DWindow instance that created the window the event comes from
      if (instanceMap.find(w) != instanceMap.end()) {
        instanceMap.at(w).get().OnKeyEvent(key, scancode, action, mods);
      }
    }

    // Mouse event capture
    void mouseMoveCallback(GLFWwindow *w, double x, double y) {
      // Forward the event to the H3DWindow instance that created the window the event comes from
      if (instanceMap.find(w) != instanceMap.end()) {
        instanceMap.at(w).get().OnMouseMoveEvent(x, y);
      }
    }
  }

  void Initialize()
  {
    if (glfwInit()) {
      ok = true;
    }
  }
  
  void Finalize()
  {
    if (ok) {
      glfwTerminate();
    }
  }

  void RegisterWindow(GLFWwindow* window, H3DWindow& instance) {
    // We can't use map::operator[] because it calls the default constructor of
    // std::reference_wrapper and reference_wrapper doesn't define it
    instanceMap.insert(InstanceMap::value_type(window, instance));

    // Register keyboard and mouse callbacks
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);
  }

  void DeregisterWindow(GLFWwindow* window) {
    instanceMap.erase(window);
  }

  void GenerateEvents()
  {
    // Clear event caches in all instances
    for (auto& instance : instanceMap) {
      instance.second.get().ClearEvents();
    }
    // Generate events again for all instances
    glfwPollEvents();
  }

  float GetDeltaT()
  {
    float time = glfwGetTime();
    float delta = time - getTime;
    getTime = time;

    return delta;
  }

  void StartClock()
  {
    if (!timerRunning) {
      timerRunning = true;
      glfwSetTime(stopTime);
    }
  }

  void StopClock()
  {
    if (timerRunning) {
      timerRunning = false;
      stopTime = glfwGetTime();
    }
  }
} // namespace GLFW
