#ifndef MENU_CONTROLLER_HPP
#define MENU_CONTROLLER_HPP

#include "FPSCharacterController.hpp"
#include "FPSCharacter.hpp"
#include "MenuView.hpp"
#include "MVC.hpp"

namespace Client
{
namespace Menu
{
  class Controller : public MVC::Controller
  {
  public:
    Controller(View& view, MVC::Application& app);
    ~Controller();
    virtual void Update(float deltaT);
    virtual void Render();
    virtual void FocusIn();
    virtual void FocusOut();

  private:
    View& view;
    MVC::Application& app;
    Server::FPSCharacter playerCharacter;
    FPSCharacterController playerCharacterController;
  };

  class ControllerFactory : public MVC::ControllerFactory
  {
  public:
    ControllerFactory(View& view, MVC::Application& app);
    virtual MVC::ControllerPtr CreateInstance();

  private:
    View& view;
    MVC::Application& app;
  };
} // namespace Menu
} // namespace Client

#endif // MENU_CONTROLLER_HPP
