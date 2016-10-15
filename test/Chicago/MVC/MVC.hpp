#ifndef MVC_HPP
#define MVC_HPP

#include "Util.hpp"

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>

namespace MVC
{
  class Controller
  {
  public:
    Controller() {}
    virtual ~Controller() {}

    // Frame logic. This method is responsible for event processing and updating view state. It is
    // not responsible for rendering the scene nor swapping buffers, this is done in the
    // View implementation associated to the controller. This method is called on every frame that
    // this controller is on the top.
    virtual void Update(float deltaT) = 0;

    // Renders this controllers's view
    virtual void Render() = 0;

    // Called before coming back to this controller from another one
    virtual void FocusIn() {}

    // Called before leaving this controller to open a new one
    virtual void FocusOut() {}
  };

  typedef std::unique_ptr<Controller> ControllerPtr;
  typedef std::list<ControllerPtr> ControllerStack;

  class ControllerFactory
  {
  public:
    ControllerFactory() {}
    virtual ~ControllerFactory() {}
    virtual ControllerPtr CreateInstance() = 0;
  };

  typedef std::map<std::string, std::reference_wrapper<ControllerFactory>> ControllerFactoryMap;

  typedef unsigned int ResourceID;

  enum ResourceType
  {
    SCENEGRAPH = 0,
    GEOMETRY,
    ANIMATION,
    MATERIAL,
    CODE,
    SHADER,
    TEXTURE,
    PARTICLEEFFECT,
    PIPELINE
  };

  class ResourceDefinition
  {
  public:
    ResourceDefinition(ResourceID id, ResourceType type, const std::string& path);
    ResourceID GetResourceID();
    ResourceType GetResourceType();
    std::string GetPath();

  private:
    ResourceID id;
    ResourceType type;
    std::string path;
  };

  typedef std::vector<ResourceDefinition> ResourceDefinitionVector;
  typedef ResourceDefinitionVector::iterator ResourceDefinitionIterator;

  class Application : public Util::EventCache
  {
  public:
    Application(Util::CommandLineArguments& args);
    virtual ~Application();

    // Resource management methods
    void DefineResource(ResourceID id, ResourceType type, const std::string& path);
    ResourceDefinitionVector GetResourceDefinitions() const;
    virtual void LoadResources() = 0;
    virtual void UnloadResources() = 0;
    virtual int FindResource(ResourceID id) = 0;

    // Flow control methods
    void RegisterControllerFactory(const std::string& name, ControllerFactory& factory);
    // This method can't be called from a Controller constructor, a Controller destructor, FocusIn() or FocusOut()
    void Call(const std::string& viewControllerName);
    // This method can't be called from a Controller constructor, a Controller destructor, FocusIn() or FocusOut()
    void Return();
    bool IsGood();
    bool IsActive();
    
    // Window management methods
    virtual bool OpenWindow(int width, int height, bool fullscreen) = 0;
    // Called on every frame to set this Applications's OpenGL context as current. This method is
    // important in order to support rendering to several Application instances sequentially. If you
    // are only going to manage one window at the same time, you can make the window's context
    // current in OpenWindow and leave this method empty.
    virtual void MakeCurrent() = 0;
    virtual void CloseWindow() = 0;
    virtual void SwapBuffers() = 0;

    // Clock management methods
    virtual float GetDeltaT() = 0;
    virtual void StartClock() = 0;
    virtual void StopClock() = 0;

    // Frame processing methods
    void CheckOpenWindow();
    void CheckCloseWindow();
    void CheckOpen();
    void CheckClose();
    void CheckFocusIn();
    void CheckFocusOut();
    void UpdateTop(float deltaT);
    void RenderTop();
    virtual void Frame();
    void SetVideoMode(unsigned int width, unsigned int height, bool fullscreen);
    unsigned int GetVideoModeWidth();
    unsigned int GetVideoModeHeight();
    bool GetVideoModeFullscreen();

  private:
    Util::CommandLineArguments& args;
    bool mustSetVideoMode;
    unsigned int newWindowWidth;
    unsigned int newWindowHeight;
    bool newFullscreen;
    unsigned int lastWindowWidth;
    unsigned int lastWindowHeight;
    bool lastFullscreen;
    bool mustFocusIn;
    bool mustFocusOut;
    bool mustOpen;
    bool mustClose;
    bool good;
    std::string nameToPush;
    ControllerFactoryMap controllerFactories;
    ControllerStack controllerStack;
    ResourceDefinitionVector resourceDefinitions;
  };
}

#endif // MVC_HPP
