#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include "FPSCharacterController.hpp"
#include "GameView.hpp"
#include "Server.hpp"
#include "MVC.hpp"

#include <functional>
#include <vector>

namespace Client
{
namespace Game
{
  class ParticleController
  {
  public:
    ParticleController(ParticleView& view, Server::Particle& model);
    void Update();
    ParticleView& GetView();
    Server::Particle& GetParticle();

  private:
    ParticleView& view;
    Server::Particle& model;
  };

  typedef std::unique_ptr<ParticleController> ParticleControllerPtr;
  typedef std::vector<ParticleControllerPtr> ParticleControllerVector;
  typedef ParticleControllerVector::iterator ParticleControllerIterator;

  class CrowdController
  {
  public:
    CrowdController(View& view, Server::Game& game);
    void FocusIn();
    void FocusOut();
    void RemoveParticle(Server::Particle& particleToRemove);
    void UpdateView(Server::ParticleReferenceVector& toDelete, Server::ParticleReferenceVector& toAdd);

  private:
    View& view;
    Server::Game& game;
    ParticleControllerVector particleControllers;
  };

  class FPSCameraController
  {
  public:
    FPSCameraController(View& view, Server::Game& game, MVC::Application& app);
    void UpdateView(float deltaT);

  private:
    View& view;
    Server::Game& game;
    MVC::Application& app;
  };

  class SettingsController
  {
  public:
    SettingsController(View& view, MVC::Application& app);
    Server::Game& GetGame();
    void CheckEvents();

  private:
    View& view;
    MVC::Application& app;
    bool debugViewMode;
    bool wireframeMode;
    unsigned int statMode;
  };

  class PauseController
  {
  public:
    PauseController(Server::Game& game, MVC::Application& app);
    void CheckEvents();

  private:
    Server::Game& game;
    MVC::Application& app;
  };

  class EndController
  {
  public:
    EndController(Server::Game& game, MVC::Application& app);
    void CheckEvents();

  private:
    Server::Game& game;
    MVC::Application& app;
  };

  typedef std::unique_ptr<EndController> EndControllerPtr;

  class VideoModeController
  {
  public:
    VideoModeController(MVC::Application&);
    void CheckEvents();

  private:
    MVC::Application& app;
  };

  class Controller : public MVC::Controller
  {
  public:
    Controller(View& view, MVC::Application& app, Server::Server& server);
    ~Controller();
    virtual void Update(float deltaT);
    virtual void Render();
    virtual void FocusIn();
    virtual void FocusOut();

  private:
    View& view;
    MVC::Application& app;
    Server::GamePtr game;
    SettingsController settingsController;
    FPSCameraController cameraController;
    CrowdController crowdController;
    PauseController pauseController;
    EndController endController;
    FPSCharacterController fpsCharacterController;
    VideoModeController videoModeController;
  };

  class ControllerFactory : public MVC::ControllerFactory
  {
  public:
    ControllerFactory(View& view, MVC::Application& app, Server::Server& server);
    virtual MVC::ControllerPtr CreateInstance();

  private:
    View& view;
    MVC::Application& app;
    Server::Server& server;
  };
} // namespace Game
} // namespace Client

#endif // GAME_CONTROLLER_HPP
