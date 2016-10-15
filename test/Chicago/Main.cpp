#include "GameController.hpp"
#include "Application.hpp"
#include "ChicagoDefs.hpp"
#include "Server.hpp"
#include "GLFW.hpp"
#include "Util.hpp"
#include "Log.hpp"

#include <string>

const std::string OPTION_DEBUG = "-debug";

// Known issues:
//   - When changing between windowed video modes, spurious mouse move events can be generated due the change in the mouse cursor position in the screen. I'm not aware of a way to avoid this with GLFW

int main(int argc, char** argv) {
  GLFW::Initialize();

  Util::CommandLineArguments args(argc, argv);
  if (args.HasOption(OPTION_DEBUG)) {
    Log::SetLogLevel(Log::DEBUG);
  }

  // Create the server
  Server::Server server;

  // Create a client application for the server
  Client::Application app(args, server, "../../../resources/");
  // Define resources
  app.DefineResource(FORWARD_PIPELINE       , MVC::PIPELINE,   "pipelines/forward.pipeline.xml");
  app.DefineResource(DEFERRED_PIPELINE      , MVC::PIPELINE,   "pipelines/deferred.pipeline.xml");
  app.DefineResource(FONT_MATERIAL          , MVC::MATERIAL,   "overlays/font.material.xml");
  app.DefineResource(PANEL_MATERIAL         , MVC::MATERIAL,   "overlays/panel.material.xml");
  app.DefineResource(LOGO_MATERIAL          , MVC::MATERIAL,   "overlays/logo.material.xml");
  app.DefineResource(LIGHT_MATERIAL         , MVC::MATERIAL,   "materials/light.material.xml");
  app.DefineResource(SKYBOX                 , MVC::SCENEGRAPH, "models/skybox/skybox.scene.xml");
  app.DefineResource(MAN_MODEL              , MVC::SCENEGRAPH, "models/man/man.scene.xml");
  app.DefineResource(MAN_ANIMATION          , MVC::ANIMATION,  "animations/man.anim");
  app.DefineResource(HDR_PIPELINE           , MVC::PIPELINE,   "pipelines/hdr.pipeline.xml");
  app.DefineResource(SPHERE_MODEL           , MVC::SCENEGRAPH, "models/sphere/sphere.scene.xml");
  app.DefineResource(KNIGHT_MODEL           , MVC::SCENEGRAPH, "models/knight/knight.scene.xml");
  app.DefineResource(KNIGHT_ORDER_ANIMATION , MVC::ANIMATION,  "animations/knight_order.anim");
  app.DefineResource(KNIGHT_ATTACK_ANIMATION, MVC::ANIMATION,  "animations/knight_attack.anim");
  app.DefineResource(PARTICLE_SYSTEM        , MVC::SCENEGRAPH, "particles/particleSys1/particleSys1.scene.xml");
  app.DefineResource(SPONZA_MODEL           , MVC::SCENEGRAPH, "models/sponza/sponza.scene.xml");

  // Main loop
  while (app.IsGood() && app.IsActive()) {
    app.Frame();
    GLFW::GenerateEvents();
  }

  GLFW::Finalize();

  return 0;
}
