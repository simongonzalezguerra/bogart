#include "GameController.hpp"
#include "ChicagoDefs.hpp"
#include "Log.hpp"

namespace Client
{
namespace Game
{
  ParticleController::ParticleController(ParticleView& view, Server::Particle& model) :
    view(view), model(model)
  {

  }

  void ParticleController::Update()
  {
    view.Update(model);
  }

  ParticleView& ParticleController::GetView()
  {
    return view;
  }

  Server::Particle& ParticleController::GetParticle()
  {
    return model;
  }


  CrowdController::CrowdController(View& view, Server::Game& game) : view(view), game(game)
  {
  }

  void CrowdController::FocusIn()
  {
    Server::ParticleReferenceVector particles = game.GetParticles();
    for (auto& particle : particles) {
      particleControllers.push_back(std::make_unique<ParticleController>(view.CreateParticleView(particle), particle.get()));
    }
  }

  void CrowdController::FocusOut()
  {
    for (auto& particleController : particleControllers) {
      view.DeleteParticleView(particleController->GetView());
    }
    particleControllers.clear();
  }

  void CrowdController::RemoveParticle(Server::Particle& particleToRemove)
  {
    for (ParticleControllerIterator it = particleControllers.begin(); it != particleControllers.end(); it++) {
      ParticleControllerPtr& current = *it;
      if (&(current->GetParticle()) == &particleToRemove) {
        view.DeleteParticleView(current->GetView());
        particleControllers.erase(it);
        break;
      }
    }
  }

  void CrowdController::UpdateView(Server::ParticleReferenceVector& toDelete, Server::ParticleReferenceVector& toAdd)
  {
    // If particles were removed, remove their views
    for (auto& particleToDelete : toDelete) {
      RemoveParticle(particleToDelete);
    }

    // If particles were added, add their views
    for (auto& particle : toAdd) {
      particleControllers.push_back(std::make_unique<ParticleController>(view.CreateParticleView(particle), particle.get()));
    }

    // For the particles that still exist, update their views
    for (auto& particle: particleControllers) {
      particle->Update();
    }
  }

  FPSCameraController::FPSCameraController(View& view, Server::Game& game, MVC::Application& app) :
    view(view), game(game), app(app)
  {

  }

  void FPSCameraController::UpdateView(float)
  {
    // Make the camera reflect the position and orientation of the player
    if (!game.IsPaused()) {
      glm::vec3 position = game.GetCharacter().GetPosition();
      view.UpdateCamera(position.x, position.y, position.z, game.GetCharacter().GetPitch(), game.GetCharacter().GetYaw());
    }
  }

  SettingsController::SettingsController(View& view, MVC::Application& app) :
    view(view), app(app), debugViewMode(false), wireframeMode(false), statMode(1)
  {

  }

  void SettingsController::CheckEvents()
  {
    // Process events and udpdate state variables
    Util::EventList events = app.GetEvents();
    for (auto& event : events) {
      if (event.eventType == Util::KEY_PRESS && event.value == 260) {
        // F3
        view.TogglePipeline();
      }

      if (event.eventType == Util::KEY_PRESS && event.value == 263) {
        // F6
        statMode++;
        if (statMode > H3DUTMaxStatMode) {
          statMode = 0;
        }
      }

      if (event.eventType == Util::KEY_PRESS && event.value == 264) {
        // F7
        debugViewMode = !debugViewMode;
      }

      if (event.eventType == Util::KEY_PRESS && event.value == 265) {
        // F8
        wireframeMode = !wireframeMode;
      }
    }

    // Read state and update view
    view.SetStatMode(statMode);
    view.SetDebugViewMode(debugViewMode);
    view.SetWireFrameMode(wireframeMode);
  }

  PauseController::PauseController(Server::Game& game, MVC::Application& app) :
    game(game), app(app)
  {

  }

  void PauseController::CheckEvents()
  {
    Util::EventList events = app.GetEvents();
    for (auto& event : events) {
      if (event.eventType == Util::KEY_PRESS && event.value == 32) {
        // SPACE
        game.SetPaused(!game.IsPaused());
      }
    }
  }

  EndController::EndController(Server::Game& game, MVC::Application& app) :
    game(game), app(app)
  {

  }

  void EndController::CheckEvents()
  {
    Util::EventList events = app.GetEvents();
    for (auto& event : events) {
      if (event.eventType == Util::KEY_PRESS && event.value == GLFW_KEY_ESCAPE) {
        game.SetEnded(true);
        break;
      }

      if (event.eventType == Util::KEY_PRESS && event.value == GLFW_KEY_ENTER) {
        app.Call(CONTROLLER_MENU);
        break;
      }
    }

    if (game.IsEnded()) {
      app.Return();
    }
  }

  VideoModeController::VideoModeController(MVC::Application& app) : app(app)
  {

  }

  void VideoModeController::CheckEvents()
  {
    Util::EventList events = app.GetEvents();
    for (auto& event : events) {
      if (event.eventType == Util::KEY_PRESS && event.value == GLFW_KEY_F1) {
        app.SetVideoMode(640, 480, false);
      }

      if (event.eventType == Util::KEY_PRESS && event.value == GLFW_KEY_F2) {
        app.SetVideoMode(1600, 900, false);
      }
    }
  }

  Controller::Controller(View& view, MVC::Application& app, Server::Server& server) :
    view(view),
    app(app),
    game(server.CreateGame()),
    settingsController(view, app),
    cameraController(view, *game, app),
    crowdController(view, *game),
    pauseController(*game, app),
    endController(*game, app),
    fpsCharacterController(game->GetCharacter(), app),
    videoModeController(app)
  {
    Log::Debug("GameController initialization");
  }

  Controller::~Controller()
  {
    Log::Debug("GameController finalization");
  }
  
  void Controller::Update(float deltaT)
  {
    // Check events
    pauseController.CheckEvents();
    endController.CheckEvents();
    settingsController.CheckEvents();
    fpsCharacterController.CheckEvents();
    videoModeController.CheckEvents();

    // Update the model
    Server::ParticleReferenceVector toDelete, toAdd;
    if (!game->IsPaused()) {
      game->Update(deltaT, toDelete, toAdd);
    }

    // Update the view from the model
    crowdController.UpdateView(toDelete, toAdd);
    cameraController.UpdateView(deltaT);
  }

  void Controller::Render()
  {
    view.Render();
  }

  void Controller::FocusIn()
  {
    Log::Debug("FocusIn");
    view.SetUp(app.GetVideoModeWidth(), app.GetVideoModeHeight());
    crowdController.FocusIn();
    fpsCharacterController.FocusIn();
  }

  void Controller::FocusOut()
  {
    Log::Debug("FocusOut");
    crowdController.FocusOut();
    view.TearDown();
  }

  ControllerFactory::ControllerFactory(View& view, MVC::Application& app, Server::Server& server) :
    view(view), app(app), server(server)
  {

  }

  MVC::ControllerPtr ControllerFactory::CreateInstance()
  {
    return std::make_unique<Controller>(view, app, server);
  }
} // namespace Game
} // namespace Client
