#ifndef MENU_VIEW_HPP
#define MENU_VIEW_HPP

#include "GLFWH3DWindow.hpp"
#include "Horde3DUtils.h"
#include "Horde3D.h"
#include "MVC.hpp"

namespace Client
{
namespace Menu
{
  class View
  {
  public:
    View(MVC::Application& app);
    virtual ~View();
    void SetUp(unsigned int videoModeWidth, unsigned int videoModeHeight);
    void TearDown();
    void Render();
    void UpdateCamera(float x, float y, float z, float rx, float ry);
    void UpdateKnightAnimation(float deltaT);

  private:
    MVC::Application& app;
    bool haveResources;
    H3DRes forwardPipeRes;
    H3DRes hdrPipeRes;
    H3DRes fontMatRes;
    H3DRes panelMatRes;
    H3DRes logoMatRes;
    H3DRes envRes;
    H3DRes knightRes;
    H3DRes knightAnim1Res;
    H3DRes knightAnim2Res;
    H3DRes particleSysRes;
    H3DNode cam;
    H3DNode env;
    H3DNode knight;
    H3DNode particleSys;
    H3DNode light;
    float weight;
    float animTime;
  };
} // namespace Menu
} // namespace Client

#endif // MENU_VIEW_HPP
