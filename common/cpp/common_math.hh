#ifndef _DEMO_COMMON_MATH_HH_
#define _DEMO_COMMON_MATH_HH_

#include "common_core.hh"

//
// Degrees/Radians
//

constexpr f32 Pi = 3.1415926535;

static inline f32 ToRadians(f32 degrees)
{
  return degrees * Pi / 180.0f;
}

static inline f32 ToDegrees(f32 radians)
{
  return radians * 180.0f / Pi;
}

//
// 2D vectors
//

class Vec2
{
public:
  f32 x;
  f32 y;
public:
  Vec2() : Vec2(0.0f, 0.0f)
  {
  }

  Vec2(f32 x, f32 y)
    : x(x), y(y)
  {
  }

  Vec2 operator+(const Vec2& rhs)
  {
    return Vec2(x + rhs.x, y + rhs.y);
  }

  Vec2 operator+=(const Vec2& rhs)
  {
    *this = *this + rhs;
    return *this;
  }

  Vec2 operator-(const Vec2& rhs)
  {
    return Vec2(x - rhs.x, y - rhs.y);
  }

  Vec2 operator-=(const Vec2& rhs)
  {
    *this = *this - rhs;
    return *this;
  }

  Vec2 operator*(f32 scalar)
  {
    return Vec2(x * scalar, y * scalar);
  }

  Vec2 operator*=(f32 scalar)
  {
    *this = *this * scalar;
    return *this;
  }

  Vec2 operator/(f32 scalar)
  {
    return Vec2(x / scalar, y / scalar);
  }

  Vec2 operator/=(f32 scalar)
  {
    *this = *this / scalar;
    return *this;
  }

  f32 LengthSq()
  {
    return x * x + y * y;
  }

  f32 Length()
  {
    return std::sqrtf(LengthSq());
  }

  Vec2 Normalize()
  {
    const f32 len = Length();
    if (len > 0) {
      return Vec2(x / len, y / len);
    } else {
      return *this;
    }
  }

  f32 ToEuler()
  {
    return ToDegrees(std::atan2f(x, y));
  }

public:
  static Vec2 FromEuler(f32 degrees)
  {
    const f32 r = ToRadians(degrees);
    return Vec2(std::sinf(r), std::cosf(r));
  };
};

#endif // _DEMO_COMMON_MATH_HH_
