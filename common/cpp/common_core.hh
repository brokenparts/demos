#ifndef _DEMOS_COMMON_CORE_HH_
#define _DEMOS_COMMON_CORE_HH_

//
// Common headers
//

#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

//
// Debugging
//

static inline void _panic_impl(const char *file, int line, const char *func, const char *fmt, ...)
{
  fprintf(stderr, "!!! Panic in function %s(...) !!!\n", func);
  fprintf(stderr, "  Location: %s:%d\n", file, line);
  fprintf(stderr, "  Error:    ");

  va_list va; va_start(va, fmt);
  vfprintf(stderr, fmt, va);
  va_end(va);

  fprintf(stderr, "\n");

  std::abort();
}

#define panic(...)                                        \
  sizeof(printf(__VA_ARGS__));                            \
  _panic_impl(__FILE__, __LINE__, __func__, __VA_ARGS__)

#define trace(...)                \
  printf("trace | " __VA_ARGS__); \
  printf("\n")

//
// Fixed-size core types
//

using i32 = std::int32_t;

using u8  = std::uint8_t;
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

//
// Basic memory allocation
//

template <typename T>
static inline T* MemAlloc(usize count = 1)
{
  void* mem = std::calloc(count, sizeof(T));
  assert(mem);
  return reinterpret_cast<T*>(mem);
}

template <typename T>
static inline void MemFree(T* ptr)
{
  std::free(ptr);
}

//
// XOR shift
//

class XorShift {
private:
  u32 state;
public:
  XorShift()
    : XorShift(123) { }

  XorShift(u32 state)
    : state(state) { }

    f32 NextF32(f32 fmin = 0.0f, f32 fmax = 1.0f)
    {
      return RemapClamp((f32)Shift(), 0.0f, (f32)UINT32_MAX, fmin, fmax);
    }
private:
  u32 Shift()
  {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
  }
};

#endif // _DEMOS_COMMON_CORE_HH_
