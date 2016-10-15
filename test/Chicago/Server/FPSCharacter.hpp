#ifndef FPS_CHARACTER_HPP
#define FPS_CHARACTER_HPP

#include <glm/vec3.hpp>

namespace Server
{
  class FPSCharacter
  {
  public:
    // Constructors
    FPSCharacter();
    // Pitch and yaw are degrees of rotation with respect to the default orientaion which is
    // looking down the nagative Z axis (forward = (0, 0, -1), right = (1, 0, 0))
    FPSCharacter(const glm::vec3 position, float pitch, float yaw, float velocity);

    // Accessors
    glm::vec3 GetPosition();
    glm::vec3 GetForward();
    glm::vec3 GetRight();
    float GetPitch();
    float GetYaw();
    void LogStatus();

    // Modifiers
    void SetVelocity(float velocity);
    void MoveForward();
    void MoveBackward();
    void StrafeRight();
    void StrafeLeft();
    void StopForwardBack();
    void StopStrafing();
    void SetPitch(float pitch);
    void SetYaw(float yaw);
    void Update(float deltaT);

  private:
    glm::vec3 position;
    float pitch;         // In degrees
    float yaw;           // In degrees
    float velocity;
    bool movingForward;
    bool movingBackward;
    bool strafingRight;
    bool strafingLeft;
  };
} // namespace Server

#endif // FPS_CHARACTER_HPP
