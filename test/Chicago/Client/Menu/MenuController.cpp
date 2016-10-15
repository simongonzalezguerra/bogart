#include "MenuController.hpp"
#include "Log.hpp"

#include <memory>

namespace Client
{
namespace Menu
{
  Controller::Controller(View& view, MVC::Application& app) :
    view(view),
    app(app),
    playerCharacter(glm::vec3(5.0f, 3.0f, 19.0f), 7.0f, 15.0f, 10.0f),
    playerCharacterController(playerCharacter, app)
  {
    Log::Debug("MenuController initialization");
  }

  Controller::~Controller()
  {
    Log::Debug("MenuController finalization");
  }
  
  void Controller::Update(float deltaT)
  {
    Util::EventList events = app.GetEvents();
    for (auto& event : events) {
      if (event.eventType == Util::KEY_PRESS && event.value == GLFW_KEY_ESCAPE) {
        app.Return();
        break;
      }
    }

    playerCharacterController.CheckEvents();

    // Update the player
    playerCharacter.Update(deltaT);

    glm::vec3 position = playerCharacter.GetPosition();
    view.UpdateCamera(position.x, position.y, position.z, playerCharacter.GetPitch(), playerCharacter.GetYaw());

    view.UpdateKnightAnimation(deltaT);
  }

  void Controller::Render()
  {
    view.Render();
  }

  void Controller::FocusIn()
  {
    Log::Debug("MenuController::FocusIn");
    view.SetUp(app.GetVideoModeWidth(), app.GetVideoModeHeight());
    playerCharacterController.FocusIn();
  }

  void Controller::FocusOut()
  {
    Log::Debug("MenuController::FocusOut");
    view.TearDown();
  }

  ControllerFactory::ControllerFactory(View& view, MVC::Application& app) :
    view(view), app(app)
  {

  }

  MVC::ControllerPtr ControllerFactory::CreateInstance()
  {
    return std::make_unique<Controller>(view, app);
  } 
} // namespace Menu
} // namespace Client