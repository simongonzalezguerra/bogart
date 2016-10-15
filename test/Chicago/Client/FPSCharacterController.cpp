#include "FPSCharacterController.hpp"
#include "Log.hpp"

#include <sstream>

namespace Client
{
  FPSCharacterController::FPSCharacterController(Server::FPSCharacter& fpsCharacter, MVC::Application& app) :
    fpsCharacter(fpsCharacter), app(app), lastMouseX(0.0f), lastMouseY(0.0f)
  {

  }

  void FPSCharacterController::FocusIn()
  {
    // Mouse has been recentered, update internal variables to calculate mouse motion
    lastMouseX = app.GetVideoModeWidth() / 2.0f;
    lastMouseY = app.GetVideoModeHeight() / 2.0f;

  }

  void FPSCharacterController::CheckEvents()
  {
    Util::EventList events = app.GetEvents();
    for (auto& event : events) {
      // Check keyboard events
      if (event.eventType == Util::KEY_PRESS && event.value == 'W') {
        Log::Debug("W press");
        fpsCharacter.MoveForward();
      }

      if (event.eventType == Util::KEY_RELEASE && event.value == 'W') {
        Log::Debug("W release");
        fpsCharacter.StopForwardBack();
        fpsCharacter.LogStatus();
      }

      if (event.eventType == Util::KEY_PRESS && event.value == 'S') {
        fpsCharacter.MoveBackward();
      }

      if (event.eventType == Util::KEY_RELEASE && event.value == 'S') {
        fpsCharacter.StopForwardBack();
        fpsCharacter.LogStatus();
      }

      if (event.eventType == Util::KEY_PRESS && event.value == 'A') {
        fpsCharacter.StrafeLeft();
      }

      if (event.eventType == Util::KEY_RELEASE && event.value == 'A') {
        fpsCharacter.StopStrafing();
        fpsCharacter.LogStatus();
      }

      if (event.eventType == Util::KEY_PRESS && event.value == 'D') {
        fpsCharacter.StrafeRight();
      }

      if (event.eventType == Util::KEY_RELEASE && event.value == 'D') {
        fpsCharacter.StopStrafing();
        fpsCharacter.LogStatus();
      }

      // Check keyboard events
      if (event.eventType == Util::MOUSE_MOVE) {
        Server::FPSCharacter& character = fpsCharacter;

        // Yaw rotates the camera around the Y axis counter-clockwise. Mouse X coordinates increase
        // to the right, so we use mouse motion to substract from yaw.
        float deltaYaw = 0.1f * (event.mouseX - lastMouseX);
        lastMouseX = event.mouseX;
        character.SetYaw(character.GetYaw() - deltaYaw);

        // Pitch rotates the camera around the X axis counter-clockwise. Mouse Y coordinates
        // increase down, so we use mouse motion to add to yaw.
        float deltaPitch = 0.1f * (lastMouseY - event.mouseY);
        lastMouseY = event.mouseY;
        character.SetPitch(character.GetPitch() + deltaPitch);

        character.LogStatus();
      }
    }
  }
} // namespace Client
