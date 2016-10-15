#include "Math.hpp"

#include <cmath>

namespace Math
{
  float DegToRad(float degrees)
  {
    return degrees * 0.017453292f; // = degrees * Pi / 180
  }
} // namespace Math