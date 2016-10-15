#include "ChicagoDefs.hpp"
#include "Application.hpp"
#include "GameView.hpp"

#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime>

namespace Client
{
namespace Game
{
  ParticleView::ParticleView(const Server::Particle& model, H3DRes particleResource, H3DRes animResource) :
    node(0)
  {
    // Add character to scene and apply animation
    node = h3dAddNodes(H3DRootNode, particleResource);
    h3dSetupModelAnimStage(node, 0, animResource, 0, "", false);
    h3dSetNodeTransform(node, model.px, 0.02f, model.pz, 0, 0, 0, 1, 1, 1);
  }

  ParticleView::~ParticleView()
  {
    h3dRemoveNode(node);
  }

  void ParticleView::Update(const Server::Particle& model)
  {
    // Get rotation from orientation
    float ry = 0;
    if (model.oz != 0.0f) {
      ry = atan2(model.ox, model.oz);
    }
    ry *= 180 / 3.1415f;  // Convert from radians to degrees

    // Update character scene node position
    h3dSetNodeTransform(node, model.px, 0.02f, model.pz, 0, ry, 0, 1, 1, 1 );

    // Update animation
    h3dSetModelAnimParams(node, 0, model.animTime, 1.0f);
    h3dUpdateModel(node, H3DModelUpdateFlags::Animation | H3DModelUpdateFlags::Geometry);
  }

  Skybox::Skybox(H3DRes skyboxRes)
  {
    node = h3dAddNodes(H3DRootNode, skyboxRes);
    h3dSetNodeTransform(node, 0, 0, 0, 0, 0, 0, 210, 50, 210);
    h3dSetNodeFlags(node, H3DNodeFlags::NoCastShadow, true);
  }

  Skybox::~Skybox()
  {
    h3dRemoveNode(node);
  }

  LightSource::LightSource(H3DRes lightSourceRes)
  {
    node = h3dAddLightNode(H3DRootNode, "Light1", lightSourceRes, "LIGHTING", "SHADOWMAP");
    h3dSetNodeTransform(node, 0.0f, 18.0f, 0.0f, -90, 0, 0, 1, 1, 1);
    h3dSetNodeParamF(node, H3DLight::RadiusF, 0, 200);
    h3dSetNodeParamF(node, H3DLight::FovF, 0, 90);
    h3dSetNodeParamI(node, H3DLight::ShadowMapCountI, 3);
    h3dSetNodeParamF(node, H3DLight::ShadowSplitLambdaF, 0, 0.9f);
    h3dSetNodeParamF(node, H3DLight::ShadowMapBiasF, 0, 0.001f);
    h3dSetNodeParamF(node, H3DLight::ColorF3, 0, 0.9f);
    h3dSetNodeParamF(node, H3DLight::ColorF3, 1, 0.7f);
    h3dSetNodeParamF(node, H3DLight::ColorF3, 2, 0.75f);
  }

  LightSource::~LightSource()
  {
    h3dRemoveNode(node);
  }

  View::View(Client::Application& app) :
    app(app),
    haveResources(false),
    forwardPipeRes(0),
    deferredPipeRes(0),
    skyBoxRes(0),
    lightMatRes(0),
    fontMatRes(0),
    panelMatRes(0),
    characterRes(0),
    characterWalkRes(0),
    logoMaterialRes(0),
    sponzaModelRes(0),
    sponzaModelNode(0)
  {

  }

  View::~View()
  {

  }

  void View::SetUp(unsigned int videoModeWidth, unsigned int videoModeHeight)
  {
    // If it's the first time we are invoked get resource handles
    if (!haveResources) {
      haveResources = true;

      forwardPipeRes = app.FindResource(FORWARD_PIPELINE);
      deferredPipeRes = app.FindResource(DEFERRED_PIPELINE);
      skyBoxRes = app.FindResource(SKYBOX);
      lightMatRes = app.FindResource(LIGHT_MATERIAL);
      fontMatRes = app.FindResource(FONT_MATERIAL);
      panelMatRes = app.FindResource(PANEL_MATERIAL);
      characterRes = app.FindResource(MAN_MODEL);
      characterWalkRes = app.FindResource(MAN_ANIMATION);
      logoMaterialRes = app.FindResource(LOGO_MATERIAL);
      sponzaModelRes = app.FindResource(SPONZA_MODEL);
    }

    CreateCamera(videoModeWidth, videoModeHeight);
    CreateSkybox();
    CreateLightSource();
    sponzaModelNode = h3dAddNodes(H3DRootNode, sponzaModelRes);
    h3dSetNodeTransform(sponzaModelNode, 0, 0, 0, 0, 0, 0, 1, 1, 1);
  }

  void View::TearDown()
  {
    h3dRemoveNode(sponzaModelNode);
    DestroyLightSource();
    DestroySkybox();
  }

  void View::Render()
  {
    // Show stats
    h3dutShowFrameStats(fontMatRes, panelMatRes, statMode);
    if (statMode > 0 && camera.get()) {
      if (camera->GetPipelineResource() == forwardPipeRes) {
        h3dutShowText("Pipeline: forward", 0.03f, 0.24f, 0.026f, 1, 1, 1, fontMatRes);
      } else {
        h3dutShowText("Pipeline: deferred", 0.03f, 0.24f, 0.026f, 1, 1, 1, fontMatRes);
      }
    }

    // Show logo
    if (camera.get()) {
      const float ww = camera->GetViewportWidth() / camera->GetViewportHeight();
      const float ovLogo[] = { ww-0.4f, 0.8f, 0, 1,  ww-0.4f, 1, 0, 0,  ww, 1, 1, 0,  ww, 0.8f, 1, 1 };
      h3dShowOverlays( ovLogo, 4, 1.f, 1.f, 1.f, 1.f, logoMaterialRes, 0 );
    }

    if (camera.get()) {
      camera->Render();
    }

    // Finish rendering of frame
    h3dFinalizeFrame();

    // Remove all overlays
    h3dClearOverlays();

    // Write all messages to log file
    h3dutDumpMessages();
  }

  ParticleView& View::CreateParticleView(const Server::Particle& model)
  {
    ParticleViewPtr ptr = std::make_unique<ParticleView>(model, characterRes, characterWalkRes);
    ParticleView& ret = *ptr;
    particleViews.push_back(std::move(ptr));

    return ret;
  }

  void View::DeleteParticleView(ParticleView& view)
  {
    for (ParticleViewIterator it = particleViews.begin(); it != particleViews.end(); it++) {
      if (&view == it->get()) {
        particleViews.erase(it);
        break;
      }
    }
  }

  Camera& View::CreateCamera(unsigned int windowWidth, unsigned int windowHeight)
  {
    camera = std::make_unique<Camera>(forwardPipeRes, deferredPipeRes, windowWidth, windowHeight);
    return *camera;
  }

  void View::DestroyCamera()
  {
    camera.reset();
  }

  void View::UpdateCamera(float x, float y, float z, float rx, float ry)
  {
    if (camera.get()) {
      camera->Update(x, y, z, rx, ry);
    }
  }

  Skybox& View::CreateSkybox()
  {
    skybox = std::make_unique<Skybox>(skyBoxRes);
    return *skybox;
  }

  void View::DestroySkybox()
  {
    skybox.reset();
  }

  LightSource& View::CreateLightSource()
  {
    lightSource = std::make_unique<LightSource>(lightMatRes);
    return *lightSource;
  }

  void View::DestroyLightSource()
  {
    lightSource.reset();
  }

  void View::SetDebugViewMode(bool debugViewMode)
  {
    h3dSetOption(H3DOptions::DebugViewMode, debugViewMode? 1.0f : 0.0f);
  }

  void View::SetWireFrameMode(bool wireframeMode)
  {
    h3dSetOption(H3DOptions::WireframeMode, wireframeMode ? 1.0f : 0.0f);
  }

  void View::SetStatMode(unsigned int mode)
  {
    statMode = mode;
  }

  void View::TogglePipeline()
  {
    if (camera.get()) {
      camera->TogglePipeline();
    }
  }


  Camera::Camera(H3DRes forwardPipeRes, H3DRes deferredPipeRes, unsigned int windowWidth, unsigned int windowHeight) :
    cam(0), forwardPipeRes(forwardPipeRes), deferredPipeRes(deferredPipeRes)
  {
    // Create camera
    cam = h3dAddCameraNode(H3DRootNode, "GameCamera", forwardPipeRes);

    // set viewport size
    h3dSetNodeParamI(cam, H3DCamera::ViewportXI, 0);
    h3dSetNodeParamI(cam, H3DCamera::ViewportYI, 0);
    h3dSetNodeParamI(cam, H3DCamera::ViewportWidthI, windowWidth);
    h3dSetNodeParamI(cam, H3DCamera::ViewportHeightI, windowHeight);

    // Set virtual camera parameters
    h3dSetupCameraView(cam, 70.0f, (float) windowWidth / windowHeight, 0.1f, 1000.0f);
    h3dResizePipelineBuffers(deferredPipeRes, windowWidth, windowHeight);
    h3dResizePipelineBuffers(forwardPipeRes, windowWidth, windowHeight);
  }

  Camera::~Camera()
  {
    h3dRemoveNode(cam);
  }

  float Camera::GetViewportWidth()
  {
    return (float) h3dGetNodeParamI(cam, H3DCamera::ViewportWidthI);
  }

  float Camera::GetViewportHeight()
  {
    return (float) h3dGetNodeParamI(cam, H3DCamera::ViewportHeightI);
  }

  H3DRes Camera::GetPipelineResource()
  {
    return h3dGetNodeParamI(cam, H3DCamera::PipeResI);
  }

  void Camera::Update(float x, float y, float z, float rx, float ry)
  {
    // Set camera parameters
    h3dSetNodeTransform(cam, x, y, z, rx ,ry, 0, 1, 1, 1 );
  }

  void Camera::Render()
  {
    // Render scene
    h3dRender(cam);
  }

  void Camera::TogglePipeline()
  {
    if (h3dGetNodeParamI(cam, H3DCamera::PipeResI) == forwardPipeRes) {
      h3dSetNodeParamI(cam, H3DCamera::PipeResI, deferredPipeRes);
    } else {
      h3dSetNodeParamI(cam, H3DCamera::PipeResI, forwardPipeRes);
    }
  }
} // namespace Game
} // namespace Client
