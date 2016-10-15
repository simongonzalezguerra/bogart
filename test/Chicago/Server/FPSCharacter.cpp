#include "FPSCharacter.hpp"
#include "Math.hpp"
#include "Log.hpp"

#include <iomanip>
#include <cmath>

namespace Server
{
  namespace
  {
    float CapPitch(float pitch)
    {
      return (pitch > 90.0f) ? 90.0f : ((pitch < -90.0f) ? -90.0f : pitch);
    }
  } // Anonymous namespace

  FPSCharacter::FPSCharacter() :
    position(glm::vec3(0.0f)),
    pitch(0.0f),
    yaw(0.0f),
    velocity(1.0f),
    movingForward(false),
    movingBackward(false),
    strafingRight(false),
    strafingLeft(false)
  {

  }

  FPSCharacter::FPSCharacter(const glm::vec3 position, float pitch, float yaw, float velocity) :
    position(position),
    pitch(CapPitch(pitch)),
    yaw(yaw),
    velocity(velocity),
    movingForward(false),
    movingBackward(false),
    strafingRight(false),
    strafingLeft(false)
  {

  }

  glm::vec3 FPSCharacter::GetPosition()
  {
    return position;
  }

  glm::vec3 FPSCharacter::GetForward()
  {
    // Result of putting the vector (0, 0, -1) through an extrinsic rotation of pitch degrees
    // around X and yaw around Y
    return glm::vec3(-sinf(Math::DegToRad(yaw)), sinf(Math::DegToRad(pitch)), -cosf(Math::DegToRad(yaw)));
  }

  glm::vec3 FPSCharacter::GetRight()
  {
    // Result of putting the vector (1, 0, 0) through a rotation of yaw degrees
    // around Y
    return glm::vec3(-sinf(Math::DegToRad(yaw - 90)), 0.0f, -cosf(Math::DegToRad(yaw - 90)));
  }

  float FPSCharacter::GetPitch()
  {
    return pitch;
  }

  float FPSCharacter::GetYaw()
  {
    return yaw;
  }

  void FPSCharacter::LogStatus()
  {
    std::ostringstream os;
    glm::vec3 position = GetPosition();
    float pitch = GetPitch();
    float yaw = GetYaw();
    os << std::setprecision(2) << std::fixed << "Position: " << position.x << ", " << position.y << ", " << position.z
       << ", pitch: " << pitch << ", yaw: " << yaw;
    Log::Debug(os.str());
  }

  void FPSCharacter::SetVelocity(float v)
  {
    velocity = v;
  }

  void FPSCharacter::MoveForward()
  {
    movingForward = true;
    movingBackward = false;
  }

  void FPSCharacter::MoveBackward()
  {
    movingForward = false;
    movingBackward = true;
  }

  void FPSCharacter::StrafeRight()
  {
    strafingRight = true;
    strafingLeft = false;
  }

  void FPSCharacter::StrafeLeft()
  {
    strafingRight = false;
    strafingLeft = true;
  }

  void FPSCharacter::StopForwardBack()
  {
    movingForward = movingBackward = false;
  }

  void FPSCharacter::StopStrafing()
  {
    strafingLeft = strafingRight = false;
  }

  void FPSCharacter::SetPitch(float p)
  {
    // Loop up/down but only in a limited range
    pitch = CapPitch(p);
  }

  void FPSCharacter::SetYaw(float y)
  {
    yaw = y;
  }

  void FPSCharacter::Update(float deltaT)
  {
    if (movingForward)
    {
      position += GetForward() * velocity * deltaT;
    }

    if (movingBackward)
    {
      position -= GetForward() * velocity * deltaT;
    }

    if (strafingRight)
    {
      position += GetRight() * velocity * deltaT;
    }

    if (strafingLeft)
    {
      position -= GetRight() * velocity * deltaT;
    }
  }
} // namespace Server
