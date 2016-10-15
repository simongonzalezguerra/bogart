#include "bogart/view_resources.hpp"
#include "bogart/log/log.hpp"
#include "bogart/view.hpp"
#include "Horde3DUtils.h"
#include "Horde3D.h"

#include <sstream>
#include <string>

namespace bogart
{
  //----------------------------------------------------------------------------------------------
  //! Constants
  //----------------------------------------------------------------------------------------------
  const std::string OPTION_CONTENT_DIR        = "-content-dir";

  //------------------------------------------------------------------------------------------------
  //! Internal helper functions and types.
  //------------------------------------------------------------------------------------------------
  namespace
  {
    enum view_state
    {
      STATE_OPEN_WAIT = 0,
      STATE_SET_UP_WAIT,
      STATE_RENDER,
      STATE_FAILED
    };

    //----------------------------------------------------------------------------------------------
    //! Functions
    //----------------------------------------------------------------------------------------------
    std::string format_settings(const view_settings& settings)
    {
      std::ostringstream os;
      os << settings.window_width << " x "
         << settings.window_height
         << (settings.fullscreen? " fullscreen" : " windowed");
      return os.str();
    }

  } // Anonymous namespace

  class view::view_impl
  {
  public:
    view_impl(async::message_queue& render_queue,
              async::message_queue& logic_queue,
              service::system& system,
              service::cmd_line_args& cmd_args) :
      render_queue(render_queue),
      logic_queue(logic_queue),
      system(system),
      cmd_args(cmd_args),
      camera(0),
      skybox(0),
      light(0),
      terrain(0),
      settings(),
      stats_enabled(false),
      state(STATE_OPEN_WAIT)
    {

    }

    bool open_window(const view_settings& target)
    {
      if (system.open_window(target.window_width, target.window_height, target.fullscreen)) {
        settings = target;
        return true;
      }

      if (settings.window_width != 0) {
        return system.open_window(settings.window_width, settings.window_height, settings.fullscreen);
      }

      return false;
    }

    void open(open_args_ptr args)
    {
      log::debug("view:: open");
      if (state == STATE_OPEN_WAIT) {
        // Open the window
        if (!open_window(args->settings)) {
          log::debug(std::string("Could not set video mode to ") + format_settings(args->settings));
          state = STATE_OPEN_WAIT;
          logic_queue.post(async::make_callable([args = std::move(args)]() {
            args->handler->on_open(false, view_settings());
          }));
          return;
        }
        log::debug(std::string("Successfully set video mode to ") + format_settings(args->settings));

        // Initialize the renderer
        if (!h3dInit()) {
          h3dutDumpMessages();
          system.close_window();
          state = STATE_OPEN_WAIT;
          logic_queue.post(async::make_callable([args = std::move(args)]() {
            args->handler->on_open(false, view_settings());
          }));
          return;
        }
        log::debug("Successfully initialized Horde3D");

        // Set options
        h3dSetOption(H3DOptions::LoadTextures, 1);
        h3dSetOption(H3DOptions::TexCompression, 0);
        h3dSetOption(H3DOptions::MaxAnisotropy, 4);
        h3dSetOption(H3DOptions::ShadowMapSize, 2048);
        h3dSetOption(H3DOptions::FastAnimation, 0);

        // Load resources
        if (!load_resources(cmd_args.get_option_value(OPTION_CONTENT_DIR, ""))) {
          log::error("There were some errors loading resources from disk. See log for details. "
                     "Continuing view initializaton anyway");
        }
        log::debug("Successfully loaded view resources");

        // Finish successfully
        state = STATE_SET_UP_WAIT;
        logic_queue.post(async::make_callable([s = settings, args = std::move(args)]() {
          args->handler->on_open(true, s);
        }));
      }
    }

    void set_up()
    {
      log::debug("view:: set_up");
      if (state == STATE_SET_UP_WAIT) {
        // Create camera
        H3DRes forward_pipe_res = get_resource(RESOURCE_FORWARD_PIPELINE);
        camera = h3dAddCameraNode(H3DRootNode, "GameCamera", forward_pipe_res);
        h3dSetNodeParamI(camera, H3DCamera::ViewportXI, 0);
        h3dSetNodeParamI(camera, H3DCamera::ViewportYI, 0);
        h3dSetNodeParamI(camera, H3DCamera::ViewportWidthI, settings.window_width);
        h3dSetNodeParamI(camera, H3DCamera::ViewportHeightI, settings.window_height);
        h3dSetupCameraView(camera, 70.0f, (float) settings.window_width / settings.window_height, 0.1f, 1000.0f);
        h3dResizePipelineBuffers(forward_pipe_res, settings.window_width, settings.window_height);

        // Create Skybox
        H3DRes skybox_res = get_resource(RESOURCE_SKYBOX);
        skybox = h3dAddNodes(H3DRootNode, skybox_res);
        h3dSetNodeTransform(skybox, 0, 0, 0, 0, 0, 0, 210, 50, 210);
        h3dSetNodeFlags(skybox, H3DNodeFlags::NoCastShadow, true);

        // Create light
        H3DRes light_source_res = get_resource(RESOURCE_LIGHT_MATERIAL);
        light = h3dAddLightNode(H3DRootNode, "Light1", light_source_res, "LIGHTING", "SHADOWMAP");
        h3dSetNodeTransform(light, 0.0f, 18.0f, 0.0f, -90, 0, 0, 1, 1, 1);
        h3dSetNodeParamF(light, H3DLight::RadiusF, 0, 200);
        h3dSetNodeParamF(light, H3DLight::FovF, 0, 90);
        h3dSetNodeParamI(light, H3DLight::ShadowMapCountI, 3);
        h3dSetNodeParamF(light, H3DLight::ShadowSplitLambdaF, 0, 0.9f);
        h3dSetNodeParamF(light, H3DLight::ShadowMapBiasF, 0, 0.001f);
        h3dSetNodeParamF(light, H3DLight::ColorF3, 0, 0.9f);
        h3dSetNodeParamF(light, H3DLight::ColorF3, 1, 0.7f);
        h3dSetNodeParamF(light, H3DLight::ColorF3, 2, 0.75f);

        // Create terrain
        H3DRes sponza_res = get_resource(RESOURCE_SPONZA_MODEL);
        terrain = h3dAddNodes(H3DRootNode, sponza_res);
        h3dSetNodeTransform(terrain, 0, 0, 0, 0, 0, 0, 1, 1, 1);

        // Start rendering loop
        state = STATE_RENDER;
        render_queue.post(async::make_callable([=]() { render(); }));
      }
    }

    void render()
    {
      if (state == STATE_RENDER) {
        // Show stats if enabled
        if (stats_enabled) {
          H3DRes font_mat_res = get_resource(RESOURCE_FONT_MATERIAL);
          H3DRes panel_mat_res = get_resource(RESOURCE_PANEL_MATERIAL);
          if (font_mat_res && panel_mat_res) {
            h3dutShowFrameStats(font_mat_res, panel_mat_res, H3DUTMaxStatMode);
          }
        }

        // Render the scene from the camera
        h3dRender(camera);
        // Tell the engine that we have finished rendering the frame (used for stats generation)
        h3dFinalizeFrame();
        // Remove all overlays
        h3dClearOverlays();
        // Display rendered frame
        system.swap_buffers();
        // Write all messages to log file
        h3dutDumpMessages();

        // Post events to controller
        if (m_event_handler) {
          auto e = system.poll_events();
          // Lambdas are immutable by default. We need to make the lambda mutable to be able to
          // move the unique_ptr we have captured inside it to set_up's parameter.
          logic_queue.post(async::make_callable([h = m_event_handler, e = std::move(e)]() mutable {
            h(std::move(e));
          }));
        }

        // Explicitly stay in STATE_RENDER and schedule next rendering loop
        state = STATE_RENDER;
        render_queue.post(async::make_callable([=]() { render(); }));
      }
    }

    void tear_down()
    {
      if (state == STATE_RENDER) {
        h3dRemoveNode(terrain);
        h3dRemoveNode(light);
        h3dRemoveNode(skybox);
        h3dRemoveNode(camera);

        state = STATE_SET_UP_WAIT;
      }
    }

    void close()
    {
      if (state == STATE_SET_UP_WAIT)
      {
        h3dRelease();
        log::debug("Successfully finalized Horde3D");
        system.close_window();
        log::debug("Successfully closed window");
        state = STATE_OPEN_WAIT;
      }
    }

    void update_camera(float x, float y, float z, float rx, float ry)
    {
      if (camera) {
        h3dSetNodeTransform(camera, x, y, z, rx ,ry, 0, 1, 1, 1 );
      }
    }

    void set_stats_enabled(bool enabled)
    {
      stats_enabled = enabled;
    }

    void subscribe_to_events(const event_handler& handler)
    {
      m_event_handler = handler;
    }

    //----------------------------------------------------------------------------------------------
    //! Member variables
    //----------------------------------------------------------------------------------------------
    async::message_queue& render_queue;
    async::message_queue& logic_queue;
    service::system& system;
    service::cmd_line_args& cmd_args;
    H3DNode camera;
    H3DNode skybox;
    H3DNode light;
    H3DNode terrain;
    view_settings settings; // last view settings that were succesfully set, if any
    bool stats_enabled;
    event_handler m_event_handler;
    view_state state;
  }; // class view::view_impl

  //------------------------------------------------------------------------------------------------
  //! Public member functions.
  //------------------------------------------------------------------------------------------------
  view::view(
    async::message_queue& render_queue,
    async::message_queue& logic_queue,
    service::system& system,
    service::cmd_line_args& args) :
    impl(std::make_unique<view::view_impl>(render_queue, logic_queue, system, args)) {

  }

  view::~view() {

  }

  void view::async_open(open_args_ptr args)
  {
    // Lambdas are immutable by default. We need to make the lambda mutable to be able to move the
    // unique_ptr we have captured inside it to set_up's parameter.
    impl->render_queue.post(async::make_callable([=, args = std::move(args)]() mutable {
      impl->open(std::move(args));
    }));
  }

  void view::async_set_up()
  {
    impl->render_queue.post(async::make_callable([=](){ impl->set_up(); }));
  }

  void view::async_tear_down()
  {
    impl->render_queue.post(async::make_callable([=]() { impl->tear_down(); }));
  }

  void view::async_close()
  {
    impl->render_queue.post(async::make_callable([=]() { impl->close(); }));
  }

  void view::async_update_camera(float x, float y, float z, float rx, float ry)
  {
    impl->render_queue.post(async::make_callable([=]() {
      impl->update_camera(x, y, z, rx, ry);
    }));
  }

  void view::async_set_stats_enabled(bool enable)
  {
    impl->render_queue.post(async::make_callable([=]() { impl->set_stats_enabled(enable); }));
  }

  void view::async_subscribe_to_events(const event_handler& handler)
  {
    impl->render_queue.post(async::make_callable([=]() {
      impl->subscribe_to_events(handler);
    }));
  }
} // namespace bogart
