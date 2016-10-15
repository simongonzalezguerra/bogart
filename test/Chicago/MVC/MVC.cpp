#include "MVC.hpp"
#include "Log.hpp"

#include <cstdlib>

namespace MVC
{
  namespace
  {
    const std::string OPTION_FULLSCREEN         = "-fullscreen";
    const std::string OPTION_WIDTH              = "-width";
    const std::string OPTION_HEIGHT             = "-height";
    const std::string OPTION_DEBUG              = "-debug";
  } // Anonymous namespace

  ResourceDefinition::ResourceDefinition(ResourceID id, ResourceType type, const std::string& path) :
    id(id), type(type), path(path)
  {

  }

  ResourceID ResourceDefinition::GetResourceID()
  {
    return id;
  }

  ResourceType ResourceDefinition::GetResourceType()
  {
    return type;
  }

  std::string ResourceDefinition::GetPath()
  {
    return path;
  }


  Application::Application(Util::CommandLineArguments& args) :
    args(args),
    mustSetVideoMode(true),
    newWindowWidth(args.HasOption(OPTION_WIDTH)? Util::ParseInt(args.GetOptionValue(OPTION_WIDTH)) : 1600),
    newWindowHeight(args.HasOption(OPTION_HEIGHT)? Util::ParseInt(args.GetOptionValue(OPTION_HEIGHT)) : 900),
    newFullscreen(args.HasOption(OPTION_FULLSCREEN)),
    lastWindowWidth(640),
    lastWindowHeight(480),
    lastFullscreen(false),
    mustFocusIn(false),
    mustFocusOut(false),
    mustOpen(false),
    mustClose(false),
    good(true)
  {

  }

  Application::~Application()
  {

  }

  void Application::DefineResource(ResourceID id, ResourceType type, const std::string& path)
  {
    // Check there isn't already a resource definition with the same id
    for (ResourceDefinitionIterator it = resourceDefinitions.begin(); it != resourceDefinitions.end(); it++) {
      if (it->GetResourceID() == id) {
        return;
      }
    }

    resourceDefinitions.push_back(ResourceDefinition(id, type, path));
  }

  ResourceDefinitionVector Application::GetResourceDefinitions() const
  {
    return resourceDefinitions;
  }

  bool Application::IsGood()
  {
    return good;
  }

  bool Application::IsActive()
  {
    return (!controllerStack.empty() || mustSetVideoMode || mustFocusIn || mustFocusOut || mustOpen || mustClose);
  }

  void Application::RegisterControllerFactory(const std::string& name, ControllerFactory& factory)
  {
    if (controllerFactories.find(name) == controllerFactories.end()) {
      controllerFactories.insert(ControllerFactoryMap::value_type(name, factory));
    }
  }

  void Application::Call(const std::string& viewControllerName)
  {
    mustFocusOut = true;
    mustOpen = true;
    mustFocusIn = true;
    mustClose = false;
    nameToPush = viewControllerName;
  }

  void Application::Return()
  {
    mustClose = true;
    mustFocusIn = true;
    mustOpen = false;
    mustFocusOut = false;
  }

  void Application::CheckOpenWindow()
  {
    // Check if we should open the window
    if (mustSetVideoMode) {
      mustSetVideoMode = false;

      if (OpenWindow(newWindowWidth, newWindowHeight, newFullscreen)) {
        lastWindowWidth = newWindowWidth;
        lastWindowHeight = newWindowHeight;
        lastFullscreen = newFullscreen;

        // Load resources from disk
        LoadResources();
  
        StartClock();      
      } else {
        Log::Debug("Could not set window mode, returning to previous configuration");
        if (!OpenWindow(lastWindowWidth, lastWindowHeight, lastFullscreen)) {
          Log::Error("Could not set last window mode, exiting");
          good = false;
        }
      }
    }

    MakeCurrent();
  }

  void Application::CheckCloseWindow()
  {
    // Check if we should close the window
    if (mustSetVideoMode) {
      StopClock();

      // Release resources
      UnloadResources();

      ClearEvents();

      CloseWindow();
    }
  }

  void Application::CheckOpen() {
    if (mustOpen) {
      mustOpen = false;
      // Find the ControllerFactory by name, create an instance, FocusIn and push it
      if (controllerFactories.find(nameToPush) != controllerFactories.end()) {
        ControllerPtr controller = controllerFactories.at(nameToPush).get().CreateInstance();
        controllerStack.push_back(std::move(controller));
      } else {
        Log::Error("Could not find controller by name \"" + nameToPush + "\", exiting");
        good = false;
      }
    }
  }

  void Application::CheckClose() {
    if (mustClose) {
      mustClose = false;
      // Delete the current top and pop it off the stack
      if (!controllerStack.empty()) {
        controllerStack.back()->FocusOut();
        controllerStack.pop_back();
      }
    }
  }

  void Application::CheckFocusIn() {
    if (mustFocusIn) {
      mustFocusIn = false;
      // FocusIn to the previous top
      if (!controllerStack.empty()) {
        controllerStack.back()->FocusIn();
      }
    }
  }

  void Application::CheckFocusOut() {
    if (mustFocusOut) {
      mustFocusOut = false;
      // FocusOut the current top Controller
      if (!controllerStack.empty()) {
        controllerStack.back()->FocusOut();
      }
    }
  }

  void Application::UpdateTop(float deltaT) {
    // Update the controller on the top
    if (!controllerStack.empty()) {
      controllerStack.back()->Update(deltaT);
    }
  }

  void Application::RenderTop() {
    // Render the view on the top
    if (!controllerStack.empty()) {
      controllerStack.back()->Render();
    }
  }

  void Application::SetVideoMode(unsigned int width, unsigned int height, bool isFullscreen)
  {
    newWindowWidth = width;
    newWindowHeight = height;
    newFullscreen = isFullscreen;
    mustSetVideoMode = true;

    mustFocusOut = true;
    mustFocusIn = true;
    mustOpen = false;
    mustClose = false;
  }

  unsigned int Application::GetVideoModeWidth()
  {
    return lastWindowWidth;
  }

  unsigned int Application::GetVideoModeHeight()
  {
    return lastWindowHeight;
  }

  bool Application::GetVideoModeFullscreen()
  {
    return lastFullscreen;
  }

  void Application::Frame()
  {
    CheckFocusOut();
    CheckClose();
    RenderTop();
    SwapBuffers();
    CheckCloseWindow();
    CheckOpenWindow();
    CheckOpen();
    CheckFocusIn();
    UpdateTop(GetDeltaT());
  }
}
