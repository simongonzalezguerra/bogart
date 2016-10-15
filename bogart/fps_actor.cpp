#include "bogart/fps_actor.hpp"
#include "bogart/log/log.hpp"

#include <iomanip>
#include <cmath>

namespace bogart
{
  //------------------------------------------------------------------------------------------------
  //! Internal helper functions and types.
  //------------------------------------------------------------------------------------------------
  namespace
  {
    float cap_pitch(float pitch)
    {
      return (pitch > 90.0f) ? 90.0f : ((pitch < -90.0f) ? -90.0f : pitch);
    }

    float deg_to_rad(float degrees)
    {
      return degrees * 0.017453292f; // = degrees * Pi / 180
    }
  } // Anonymous namespace

  class fps_actor::fps_actor_impl
  {
  public:
    fps_actor_impl() :
      m_position(glm::vec3(0.0f)),
      m_pitch(0.0f),
      m_yaw(0.0f),
      m_velocity(1.0f),
      m_moving_forward(false),
      m_moving_backward(false),
      m_strafing_right(false),
      m_strafing_left(false)
    {

    }

    fps_actor_impl(const glm::vec3 position, float pitch, float yaw, float velocity) :
      m_position(position),
      m_pitch(cap_pitch(pitch)),
      m_yaw(yaw),
      m_velocity(velocity),
      m_moving_forward(false),
      m_moving_backward(false),
      m_strafing_right(false),
      m_strafing_left(false)
    {

    }

    //----------------------------------------------------------------------------------------------
    //! Member variables
    //----------------------------------------------------------------------------------------------
    glm::vec3 m_position;
    float m_pitch;         // In degrees
    float m_yaw;           // In degrees
    float m_velocity;
    bool m_moving_forward;
    bool m_moving_backward;
    bool m_strafing_right;
    bool m_strafing_left;
  }; // class fps_actor::fps_actor_impl

  //------------------------------------------------------------------------------------------------
  //! Public member functions.
  //------------------------------------------------------------------------------------------------
  fps_actor::fps_actor() :
    impl(std::make_unique<fps_actor::fps_actor_impl>())
  {

  }

  fps_actor::fps_actor(const glm::vec3& position, float pitch, float yaw, float velocity) :
    impl(std::make_unique<fps_actor::fps_actor_impl>(position, pitch, yaw, velocity))
  {

  }

  fps_actor::~fps_actor()
  {

  }

  glm::vec3 fps_actor::get_position()
  {
    return impl->m_position;
  }

  glm::vec3 fps_actor::get_forward()
  {
    // Result of putting the vector (0, 0, -1) through an extrinsic rotation of pitch degrees
    // around X and m_yaw around Y
    return glm::vec3(-sinf(deg_to_rad(impl->m_yaw)), sinf(deg_to_rad(impl->m_pitch)), -cosf(deg_to_rad(impl->m_yaw)));
  }

  glm::vec3 fps_actor::get_right()
  {
    // Result of putting the vector (1, 0, 0) through a rotation of m_yaw degrees
    // around Y
    return glm::vec3(-sinf(deg_to_rad(impl->m_yaw - 90)), 0.0f, -cosf(deg_to_rad(impl->m_yaw - 90)));
  }

  float fps_actor::get_pitch()
  {
    return impl->m_pitch;
  }

  float fps_actor::get_yaw()
  {
    return impl->m_yaw;
  }

  void fps_actor::log_status()
  {
    std::ostringstream os;
    glm::vec3 position = get_position();
    os << std::setprecision(2) << std::fixed << "Position: "
       << position.x << ", " << position.y << ", " << position.z
       << ", pitch: " << impl->m_pitch << ", yaw: " << impl->m_yaw;
    log::debug(os.str());
  }

  void fps_actor::set_velocity(float v)
  {
    impl->m_velocity = v;
  }

  void fps_actor::start_moving_forward()
  {
    log::debug("fps_actor: start_moving_forward");
    impl->m_moving_forward = true;
    impl->m_moving_backward = false;
  }

  void fps_actor::start_moving_backward()
  {
    log::debug("fps_actor: start_moving_backward");
    impl->m_moving_forward = false;
    impl->m_moving_backward = true;
  }

  void fps_actor::start_strafing_right()
  {
    log::debug("fps_actor: start_strafing_right");
    impl->m_strafing_right = true;
    impl->m_strafing_left = false;
  }

  void fps_actor::start_strafing_left()
  {
    log::debug("fps_actor: start_strafing_left");
    impl->m_strafing_right = false;
    impl->m_strafing_left = true;
  }

  void fps_actor::stop_moving_forward_back()
  {
    log::debug("fps_actor: stop_moving_forward_back");
    impl->m_moving_forward = false;
    impl->m_moving_backward = false;
  }

  void fps_actor::stop_strafing()
  {
    log::debug("fps_actor: stop_strafing");
    impl->m_strafing_left = false;
    impl->m_strafing_right = false;
  }

  void fps_actor::set_position(const glm::vec3& position)
  {
    impl->m_position = position;
  }

  void fps_actor::set_pitch(float p)
  {
    // Loop up/down but only in a limited range
    impl->m_pitch = cap_pitch(p);
  }

  void fps_actor::set_yaw(float y)
  {
    impl->m_yaw = y;
  }

  void fps_actor::update(float dt)
  {
    if (impl->m_moving_forward)
    {
      impl->m_position += get_forward() * impl->m_velocity * dt;
    }

    if (impl->m_moving_backward)
    {
      impl->m_position -= get_forward() * impl->m_velocity * dt;
    }

    if (impl->m_strafing_right)
    {
      impl->m_position += get_right() * impl->m_velocity * dt;
    }

    if (impl->m_strafing_left)
    {
      impl->m_position -= get_right() * impl->m_velocity * dt;
    }
  }
} // namespace bogart
