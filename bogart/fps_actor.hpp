#ifndef FPS_ACTOR_HPP
#define FPS_ACTOR_HPP

#include <glm/vec3.hpp>

#include <memory>

namespace bogart
{
  //------------------------------------------------------------------------------------------------
  //! @class fps_actor
  //! @ingroup bogart
  //!
  //! Thread-safety: all public methods are thread-safe.
  //------------------------------------------------------------------------------------------------
  class fps_actor
  {
  public:
    //----------------------------------------------------------------------------------------------
    //! Types
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    //! Constructor
    //----------------------------------------------------------------------------------------------
    fps_actor();

    //----------------------------------------------------------------------------------------------
    //! Constructor
    //----------------------------------------------------------------------------------------------
    // Pitch and yaw are degrees of rotation with respect to the default orientation which is
    // looking down the nagative Z axis (forward = (0, 0, -1), right = (1, 0, 0))
    fps_actor(const glm::vec3& position, float pitch, float yaw, float velocity);

    //----------------------------------------------------------------------------------------------
    //! Destructor
    //----------------------------------------------------------------------------------------------
    ~fps_actor();

    //----------------------------------------------------------------------------------------------
    //! Member functions
    //----------------------------------------------------------------------------------------------
    // Accessors
    glm::vec3 get_position();
    glm::vec3 get_forward();
    glm::vec3 get_right();
    float get_pitch();
    float get_yaw();
    void log_status();

    // Modifiers
    void set_velocity(float velocity);
    void start_moving_forward();
    void start_moving_backward();
    void start_strafing_right();
    void start_strafing_left();
    void stop_moving_forward_back();
    void stop_strafing();
    void set_position(const glm::vec3& position);
    void set_pitch(float pitch);
    void set_yaw(float yaw);
    void update(float deltat);

  private:
    class fps_actor_impl;                            //!< implementation class (Pimpl idiom)
    std::unique_ptr<fps_actor_impl> impl;            //!< pointer to implementation (Pimpl idiom)
  }; // class fps_actor
} // namespace bogart


#endif // FPS_ACTOR_HPP
