#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "GameController.hpp"
#include "MenuController.hpp"
#include "GLFWH3DWindow.hpp"
#include "GameView.hpp"
#include "MenuView.hpp"
#include "Server.hpp"
#include "Horde3D.h"
#include "GLFW.hpp"
#include "MVC.hpp"

#include <map>

namespace Client
{
  typedef std::map<MVC::ResourceID, H3DRes> ResourceMap;
  typedef ResourceMap::iterator ResourceIterator;

  class Application : public MVC::Application
  {
  public:
    Application(Util::CommandLineArguments& args, Server::Server& server, const std::string& contentDir);
    ~Application();
    // MVC::Application Implementation
    virtual void LoadResources();
    virtual void UnloadResources();
    virtual int FindResource(MVC::ResourceID id);
    virtual bool OpenWindow(int width, int height, bool fullscreen);
    virtual void MakeCurrent();
    virtual void CloseWindow();
    virtual void SwapBuffers();
    virtual float GetDeltaT();
    virtual void StartClock();
    virtual void StopClock();
    Server::Server& GetServer();

  private:
    GLFW::H3DWindow glfwh3dWindow;
    Client::Game::View gameView;
    Client::Game::ControllerFactory gameControllerFactory;
    Client::Menu::View menuView;
    Client::Menu::ControllerFactory menuControllerFactory;
    ResourceMap resourceMap;
    std::string contentDir;
  };
}

#endif // APPLICATION_HPP