#ifndef _DEMOS_COMMON_CORE_HH_
#define _DEMOS_COMMON_CORE_HH_

//
// Common headers
//

#include <cstdint>
#include <cstdio>

//
// Fixed-size core types
//

using i32 = std::int32_t;

using u32 = std::uint32_t;

using f32 = float;
using f64 = double;

using usize = std::size_t;

//
// Type helpers
//

#ifndef UNUSED
# define UNUSED(x) ((void)(x))
#endif

template <typename T>
static inline T Min(T v1, T v2)
{
  if (v1 < v2) {
    return v1;
  } else {
    return v2;
  }
}

template <typename T>
static inline T Max(T v1, T v2)
{
  if (v1 > v2) {
    return v1;
  } else {
    return v2;
  }
}

template <typename T>
static inline T Clamp(T v, T vmin, T vmax)
{
  return Min(Max(v, vmin), vmax);
}

template <typename T>
static inline T Remap(T v, T vmin1, T vmax1, T vmin2, T vmax2)
{
  return vmin2 + (v - vmin1) * (vmax2 - vmin2) / (vmax1 - vmin1);
}

template <typename T>
static inline T RemapClamp(T v, T vmin1, T vmax1, T vmin2, T vmax2)
{
  return Clamp(Remap(v, vmin1, vmax1, vmin2, vmax2), vmin2, vmax2);
}

#endif // _DEMOS_COMMON_CORE_HH_
