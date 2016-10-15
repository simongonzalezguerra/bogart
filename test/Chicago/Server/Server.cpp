#include "Server.hpp"

#include <cmath>

namespace Server
{
  namespace
  {
    void chooseDestination(Particle &p)
    {
      // Choose random destination within a circle
      float ang = ((rand() % 360) / 360.0f) * 6.28f;
      float rad = (rand() % 20) * 1.0f;

      p.dx = sinf( ang ) * rad;
      p.dz = cosf( ang ) * rad;
    }

  }

  Particle::Particle()
  {
    ox = 0; oz = 0;
    animTime = 0;
  }


  Game::Game() :
    player(glm::vec3(12.75f, 2.0f, 0.1f), 12.7f, 88.0f, 7.5f),
    isEnded(false),
    isPaused(false)
  {
    // Init random generator
    srand( 99777 );  // Use fixed value for better performance comparisons

    // Add characters
    for(unsigned int i = 0; i < 10; ++i)
    {
      ParticlePtr p = std::make_unique<Particle>();

      // Characters start in a circle formation
      p->px = sinf( (i / 100.0f) * 6.28f ) * 10.0f;
      p->pz = cosf( (i / 100.0f) * 6.28f ) * 10.0f;

      chooseDestination(*p);

      particles.push_back(std::move(p));
    }
  }

  ParticleReferenceVector Game::GetParticles()
  {
    ParticleReferenceVector ret;
    for (auto& particle : particles) {
      ret.push_back(*particle);
    }

    return ret;
  }

  FPSCharacter& Game::GetCharacter()
  {
    return player;
  }

  void Game::Update(float deltaT, ParticleReferenceVector&, ParticleReferenceVector)
  {
    // Update the crowd
    // Parameters for three repulsion zones
    float d1 = 0.25f, d2 = 2.0f, d3 = 4.5f;
    float f1 = 3.0f, f2 = 1.0f, f3 = 0.1f;

    for(unsigned int i = 0; i < particles.size(); ++i )
    {
      Particle &p = *particles[i];

      // Reset force
      p.fx = 0; p.fz = 0;

      // Calculate distance to destination
      float dist = sqrtf( (p.dx - p.px)*(p.dx - p.px) + (p.dz - p.pz)*(p.dz - p.pz) );

      // If destination not reached walk to destination
      if( dist > 3.0f )
      {
        // Calculate normalized attraction force to destination
        float afx = (p.dx - p.px) / dist;
        float afz = (p.dz - p.pz) / dist;

        p.fx += afx * 0.035f; p.fz += afz * 0.035f;

        // Repulsion forces from other particles
        for( unsigned int j = 0; j < particles.size(); ++j )
        {
          if( j == i ) continue;

          Particle &p2 = *particles[j];

          float dist2 = sqrtf( (p.px - p2.px)*(p.px - p2.px) + (p.pz - p2.pz)*(p.pz - p2.pz) );
          float strength = 0;

          float rfx = (p.px - p2.px) / dist2;
          float rfz = (p.pz - p2.pz) / dist2;

          // Use three zones with different repulsion strengths
          if( dist2 <= d3 && dist2 > d2 )
          {
            float m = (f3 - 0) / (d2 - d3);
            float t = 0 - m * d3;
            strength = m * dist2 + t;
          }
          else if( dist2 <= d2 && dist2 > d1 )
          {
            float m = (f2 - f3) / (d1 - d2);
            float t = f3 - m * d2;
            strength = m * dist2 + t;
          }
          else if( dist2 <= d1 )
          {
            float m = (f1 - f2) / (0 - d1);
            float t = f2 - m * d1;
            strength = m * dist2 + t;
          }

          p.fx += rfx * strength; p.fz += rfz * strength;
        }
      }
      else
      {
        // Choose new destination
        chooseDestination( p );
      }

      // Make movement framerate independent
      p.fx *= (30 * deltaT);
      p.fz *= (30 * deltaT);
      float vel = sqrtf( p.fx * p.fx + p.fz * p.fz );
      // Update animation
      p.animTime += vel * 35.0f;

      // Set new position
      p.px += p.fx; p.pz += p.fz;

      // Calculate orientation
      p.ox = (p.ox + p.fx) / 2;
      p.oz = (p.oz + p.fz) / 2;
    }

    // Update the player
    player.Update(deltaT);
  }

  void Game::DeleteParticles()
  {

  }

  void Game::SetEnded(bool ended)
  {
    isEnded = ended;
  }

  bool Game::IsEnded() const
  {
    return isEnded;
  }

  void Game::SetPaused(bool p)
  {
    isPaused = p;
  }

  bool Game::IsPaused() const
  {
    return isPaused;
  }

  Server::Server()
  {

  }

  Server::~Server()
  {

  }

  GamePtr Server::CreateGame()
  {
    return std::make_unique<Game>();
  }
} // namespace Server
