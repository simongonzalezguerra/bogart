#include "ChicagoDefs.hpp"
#include "MenuView.hpp"

#include <string>

namespace Client
{
namespace Menu
{
  namespace
  {

  }

  View::View(MVC::Application& app) :
    app(app),
    haveResources(false),
    forwardPipeRes(0),
    hdrPipeRes(0),
    cam(0),
    env(0),
    knight(0),
    particleSys(0),
    light(0),
    weight(1.0f),
    animTime(0.0f)
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
      hdrPipeRes     = app.FindResource(HDR_PIPELINE);
      fontMatRes     = app.FindResource(FONT_MATERIAL);
      panelMatRes    = app.FindResource(PANEL_MATERIAL);
      logoMatRes     = app.FindResource(LOGO_MATERIAL);
      envRes         = app.FindResource(SPHERE_MODEL);
      knightRes      = app.FindResource(KNIGHT_MODEL);
      knightAnim1Res = app.FindResource(KNIGHT_ORDER_ANIMATION);
      knightAnim2Res = app.FindResource(KNIGHT_ATTACK_ANIMATION);
      particleSysRes = app.FindResource(PARTICLE_SYSTEM);
    }

    // Create camera
    cam = h3dAddCameraNode(H3DRootNode, "MenuCamera", hdrPipeRes);
    // Resize viewport
    h3dSetNodeParamI(cam, H3DCamera::ViewportXI, 0 );
    h3dSetNodeParamI(cam, H3DCamera::ViewportYI, 0 );
    h3dSetNodeParamI(cam, H3DCamera::ViewportWidthI, videoModeWidth);
    h3dSetNodeParamI(cam, H3DCamera::ViewportHeightI, videoModeHeight);
    
    // Set virtual camera parameters
    h3dSetupCameraView(cam, 45.0f, (float) videoModeWidth / videoModeHeight, 0.1f, 1000.0f );
    h3dResizePipelineBuffers(hdrPipeRes, videoModeWidth, videoModeHeight );

    env = h3dAddNodes( H3DRootNode, envRes );
    h3dSetNodeTransform( env, 0, -20, 0, 0, 0, 0, 20, 20, 20 );
    // Add knight
    knight = h3dAddNodes( H3DRootNode, knightRes );
    h3dSetNodeTransform( knight, 0, 0, 0, 0, 180, 0, 0.1f, 0.1f, 0.1f );
    h3dSetupModelAnimStage( knight, 0, knightAnim1Res, 0, "", false );
    h3dSetupModelAnimStage( knight, 1, knightAnim2Res, 0, "", false );
    // Attach particle system to hand joint
    h3dFindNodes( knight, "Bip01_R_Hand", H3DNodeTypes::Joint );
    H3DNode hand = h3dGetNodeFindResult( 0 );
    particleSys = h3dAddNodes( hand, particleSysRes );
    h3dSetNodeTransform( particleSys, 0, 40, 0, 90, 0, 0, 1, 1, 1 );

    // Add light source
    light = h3dAddLightNode( H3DRootNode, "Light1", 0, "LIGHTING", "SHADOWMAP" );
    h3dSetNodeTransform( light, 0, 15, 10, -60, 0, 0, 1, 1, 1 );
    h3dSetNodeParamF( light, H3DLight::RadiusF, 0, 30 );
    h3dSetNodeParamF( light, H3DLight::FovF, 0, 90 );
    h3dSetNodeParamI( light, H3DLight::ShadowMapCountI, 1 );
    h3dSetNodeParamF( light, H3DLight::ShadowMapBiasF, 0, 0.01f );
    h3dSetNodeParamF( light, H3DLight::ColorF3, 0, 1.0f );
    h3dSetNodeParamF( light, H3DLight::ColorF3, 1, 0.8f );
    h3dSetNodeParamF( light, H3DLight::ColorF3, 2, 0.7f );
    h3dSetNodeParamF( light, H3DLight::ColorMultiplierF, 0, 1.0f );

    // Customize post processing effects
    H3DNode matRes = h3dFindResource( H3DResTypes::Material, "pipelines/postHDR.material.xml" );
    h3dSetMaterialUniform( matRes, "hdrExposure", 2.5f, 0, 0, 0 );
    h3dSetMaterialUniform( matRes, "hdrBrightThres", 0.5f, 0, 0, 0 );
    h3dSetMaterialUniform( matRes, "hdrBrightOffset", 0.08f, 0, 0, 0 );
  }

  void View::TearDown()
  {
    h3dRemoveNode(light);
    h3dRemoveNode(particleSys);
    h3dRemoveNode(knight);
    h3dRemoveNode(env);
    h3dRemoveNode(cam);
  }

  void View::Render()
  {
    h3dRender( cam );
    // Finish rendering of frame
    h3dFinalizeFrame();
  }

  void View::UpdateCamera(float x, float y, float z, float rx, float ry)
  {
    // Set camera parameters
    h3dSetNodeTransform(cam, x, y, z, rx ,ry, 0, 1, 1, 1);
  }

  void View::UpdateKnightAnimation(float deltaT)
  {
    animTime += deltaT;

    // Do animation blending
    h3dSetModelAnimParams( knight, 0, animTime * 24.0f, weight );
    h3dSetModelAnimParams( knight, 1, animTime * 24.0f, 1.0f - weight );
    h3dUpdateModel( knight, H3DModelUpdateFlags::Animation | H3DModelUpdateFlags::Geometry );

    // Animate particle systems (several emitters in a group node)
    unsigned int cnt = h3dFindNodes( particleSys, "", H3DNodeTypes::Emitter );
    for( unsigned int i = 0; i < cnt; ++i )
    {
      h3dUpdateEmitter( h3dGetNodeFindResult( i ), deltaT);
    }
  }
} // namespace Menu
} // namespace Client
