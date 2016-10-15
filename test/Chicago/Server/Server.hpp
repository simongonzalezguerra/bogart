#ifndef SERVER_HPP
#define SERVER_HPP

#include "FPSCharacter.hpp"

#include <glm/vec3.hpp>

#include <vector>
#include <memory>

namespace Server
{
  struct Particle
  {
    Particle();

    float    px, pz;  // Current postition
    float    dx, dz;  // Destination position
    float    fx, fz;  // Force on particle
    float    ox, oz;  // Orientation vector
    float    animTime;
  };

  typedef std::vector<std::reference_wrapper<Particle>> ParticleReferenceVector;
  typedef std::unique_ptr<Particle> ParticlePtr;
  typedef std::vector<ParticlePtr> ParticleVector;
  typedef ParticleVector::iterator ParticleIterator;

  class Game
  {
  public:
    Game();
    ParticleReferenceVector GetParticles();
    FPSCharacter& GetCharacter();
    void Update(float deltaT, ParticleReferenceVector& toDelete, ParticleReferenceVector toAdd);
    void DeleteParticles();
    void SetEnded(bool ended);
    bool IsEnded() const;
    void SetPaused(bool paused);
    bool IsPaused() const;

  private:
    ParticleVector particles;
    ParticleVector toDelete;
    FPSCharacter player;
    bool isEnded;
    bool isPaused;
  };

  typedef std::unique_ptr<Game> GamePtr;

  class Server
  {
  public:
    Server();
    ~Server();
    GamePtr CreateGame();
  };
} // namespace Server

#endif // SERVER_HPP
