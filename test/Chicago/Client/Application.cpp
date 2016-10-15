#include "ChicagoDefs.hpp"
#include "Application.hpp"
#include "Horde3DUtils.h"

namespace Client
{
  namespace
  {
    typedef std::map<MVC::ResourceType, int> ResourceTypeMap;
    typedef ResourceTypeMap::iterator ResourceTypeIterator;

    ResourceTypeMap resourceTypeMap;

    void FillResourceTypeMap()
    {
      resourceTypeMap[MVC::SCENEGRAPH]     = H3DResTypes::SceneGraph;
      resourceTypeMap[MVC::GEOMETRY]       = H3DResTypes::Geometry;
      resourceTypeMap[MVC::ANIMATION]      = H3DResTypes::Animation;
      resourceTypeMap[MVC::MATERIAL]       = H3DResTypes::Material;
      resourceTypeMap[MVC::CODE]           = H3DResTypes::Code;
      resourceTypeMap[MVC::SHADER]         = H3DResTypes::Shader;
      resourceTypeMap[MVC::TEXTURE]        = H3DResTypes::Texture;
      resourceTypeMap[MVC::PARTICLEEFFECT] = H3DResTypes::ParticleEffect;
      resourceTypeMap[MVC::PIPELINE]       = H3DResTypes::Pipeline;
    }

    int MapResourceTypeToHorde3DResourceType(MVC::ResourceType type) {
      int ret = 0;
      if (resourceTypeMap.empty()) {
        FillResourceTypeMap();
      }

      if (resourceTypeMap.find(type) != resourceTypeMap.end()) {
        ret = resourceTypeMap[type];
      }

      return ret;
    }
  }

  Application::Application(Util::CommandLineArguments& args, Server::Server& server, const std::string& contentDir) :
    MVC::Application(args),
    glfwh3dWindow(*this, "Chicago"),
    gameView(*this),
    gameControllerFactory(gameView, *this, server),
    menuView(*this),
    menuControllerFactory(menuView, *this),
    contentDir(contentDir)
  {
    RegisterControllerFactory(CONTROLLER_GAME, gameControllerFactory);
    RegisterControllerFactory(CONTROLLER_MENU, menuControllerFactory);
    Call(CONTROLLER_GAME);
  }

  Application::~Application()
  {

  }

  void Application::LoadResources()
  {
    MVC::ResourceDefinitionVector resourceDefinitions = GetResourceDefinitions();
    for (MVC::ResourceDefinitionIterator it = resourceDefinitions.begin(); it != resourceDefinitions.end(); it++) {
      int horde3DResourceType = MapResourceTypeToHorde3DResourceType(it->GetResourceType());
      resourceMap[it->GetResourceID()] = h3dAddResource(horde3DResourceType, it->GetPath().c_str(), 0);
    }

    h3dutLoadResourcesFromDisk(contentDir.c_str());
  }

  void Application::UnloadResources()
  {

  }

  int Application::FindResource(MVC::ResourceID id)
  {
    int ret = 0;
    if (resourceMap.find(id) != resourceMap.end()) {
      ret = resourceMap[id];
    }

    return ret;
  }

  bool Application::OpenWindow(int width, int height, bool fullscreen)
  {
    return glfwh3dWindow.Open(width, height, fullscreen);
  }

  void Application::MakeCurrent()
  {
    // Left empty, in this implementation we only manage one window at the same time so we don't
    // need to make the OpenGL context current on every frame, it's enough to do it when the window
    // is created.
  }

  void Application::CloseWindow()
  {
    glfwh3dWindow.Close();
  }

  void Application::SwapBuffers()
  {
    glfwh3dWindow.SwapBuffers();
  }

  float Application::GetDeltaT()
  {
    return GLFW::GetDeltaT();
  }

  void Application::StartClock()
  {
    GLFW::StartClock();
  }

  void Application::StopClock()
  {
    GLFW::StopClock();
  }
}
