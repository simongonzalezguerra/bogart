#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include "GLFWH3DWindow.hpp"
#include "Horde3DUtils.h"
#include "Server.hpp"
#include "Horde3D.h"
#include "Math.hpp"

#include <functional>
#include <vector>

namespace Client
{
  class Application;
namespace Game
{
  class ParticleView
  {
  public:
    ParticleView(const Server::Particle& model, H3DRes particleResource, H3DRes animResource);
    ~ParticleView();
    void Update(const Server::Particle& model);

  private:
    H3DNode node;
  };

  typedef std::unique_ptr<ParticleView> ParticleViewPtr;
  typedef std::vector<ParticleViewPtr> ParticleViewVector;
  typedef ParticleViewVector::iterator ParticleViewIterator;

  class Camera
  {
  public:
    Camera(H3DRes forwardPipeRes, H3DRes deferredPipeRes, unsigned int windowWidth, unsigned int windowHeight);
    ~Camera();
    void Update(float x, float y, float z, float rx, float ry);
    float GetViewportWidth();
    float GetViewportHeight();
    H3DRes GetPipelineResource();
    void Render();
    void TogglePipeline();

  private:
    H3DNode cam;
    H3DRes forwardPipeRes;
    H3DRes deferredPipeRes;
  };

  typedef std::unique_ptr<Camera> CameraPtr;


  class Skybox
  {
  public:
    Skybox(H3DRes skyboxRes);
    ~Skybox();

  private:
    H3DNode node;
  };

  typedef std::unique_ptr<Skybox> SkyboxPtr;

  class LightSource
  {
  public:
    LightSource(H3DRes lightSourceRes);
    ~LightSource();

  private:
    H3DNode node;
  };

  typedef std::unique_ptr<LightSource> LightSourcePtr;

  class View
  {
  public:
    View(Client::Application& app);
    virtual ~View();
    void SetUp(unsigned int videoModeWidth, unsigned int videoModeHeight);
    void TearDown();
    void Render();
    ParticleView& CreateParticleView(const Server::Particle& model);
    void DeleteParticleView(ParticleView& view);
    Camera& CreateCamera(unsigned int windowWidth, unsigned int windowHeight);
    void DestroyCamera();
    void UpdateCamera(float x, float y, float z, float rx, float ry);
    Skybox& CreateSkybox();
    void DestroySkybox();
    LightSource& CreateLightSource();
    void DestroyLightSource();
    void SetDebugViewMode(bool debugViewMode);
    void SetWireFrameMode(bool wireframeMode);
    void SetStatMode(unsigned int statMode);
    void TogglePipeline();

  private:
    Client::Application& app;
    ParticleViewVector particleViews;
    CameraPtr camera;
    SkyboxPtr skybox;
    LightSourcePtr lightSource;
    unsigned int statMode;
    bool haveResources;
    H3DRes forwardPipeRes;
    H3DRes deferredPipeRes;
    H3DRes skyBoxRes;
    H3DRes lightMatRes;
    H3DRes fontMatRes;
    H3DRes panelMatRes;
    H3DRes characterRes;
    H3DRes characterWalkRes;
    H3DRes logoMaterialRes;
    H3DRes sponzaModelRes;
    H3DNode sponzaModelNode;
  };
} // namespace Game
} // namespace Client

#endif // GAME_VIEW_HPP
