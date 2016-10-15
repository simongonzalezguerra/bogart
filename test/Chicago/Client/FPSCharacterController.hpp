#ifndef FPS_CHARACTER_CONTROLLER_HPP
#define FPS_CHARACTER_CONTROLLER_HPP

#include "FPSCharacter.hpp"
#include "MVC.hpp"

namespace Client
{
  class FPSCharacterController
  {
  public:
    FPSCharacterController(Server::FPSCharacter&, MVC::Application&);
    void FocusIn();
    // Consider passing the event list as an argument here and remove the dependency to Application
    void CheckEvents();

  private:
    Server::FPSCharacter& fpsCharacter;
    MVC::Application& app;
    float lastMouseX;
    float lastMouseY;
  };
} // namespace Client

#endif // FPS_CHARACTER_CONTROLLER_HPP
