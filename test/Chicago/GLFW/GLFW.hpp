#ifndef GLFW_HPP
#define GLFW_HPP

#include "GLFWH3DWindow.hpp"
#include "glfw3.h"

namespace GLFW
{
  void Initialize();
  void Finalize();
  void RegisterWindow(GLFWwindow* window, H3DWindow& instance);
  void DeregisterWindow(GLFWwindow* window);
  void GenerateEvents();
  float GetDeltaT();
  void StartClock();
  void StopClock();
} // namespace GLFW

#endif // GLFW_HPP
