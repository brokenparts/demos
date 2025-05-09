#include "common_core.hh"

//
// Using undocumented multitouch API
//
// ref: https://web.archive.org/web/20151012175118/http://steike.com/code/multitouch/
// ref: https://gist.github.com/rmhsilva/61cc45587ed34707da34818a76476e11
//

#include <Cocoa/Cocoa.h>
#include <CoreFoundation/CoreFoundation.h>

struct MTPoint {
  f32 x;
  f32 y;
};

struct MTVector {
  MTPoint position;
  MTPoint velocity;
};

struct MTTouch {
  i32       frame;
  f64       timestamp;
  i32       pathIndex;
  u32       state;
  i32       fingerID;
  i32       handID;
  MTVector  normalizedVector;
};

using MTDeviceRef = CFTypeRef;
using MTFrameCallbackFn = void(*)(MTDeviceRef, MTTouch *, size_t, double, size_t);

extern "C" {

MTDeviceRef MTDeviceCreateDefault();
MTDeviceRef MTDeviceCreateFromDeviceID(int64_t);
OSStatus    MTDeviceStart(MTDeviceRef, int);
void        MTDeviceRelease(MTDeviceRef device);

void MTRegisterContactFrameCallback(MTDeviceRef, MTFrameCallbackFn);

}

static void TouchFrameCallback(MTDeviceRef, MTTouch *touches, size_t num_touches, double, size_t)
{
  const f32 margin = 0.1f;

  NSRect display_bounds = [[NSScreen mainScreen] frame];
  for (size_t i = 0; i < num_touches; ++i) {
    MTPoint pos = touches[i].normalizedVector.position;
    printf("pos: <%.2f, %.2f>\n", pos.x, pos.y);

    pos.x = RemapClamp(pos.x, margin, 1.0f - margin, 0.0f, 1.0f);
    pos.y = 1.0f - RemapClamp(pos.y, margin, 1.0f - margin, 0.0f, 1.0f);
    
    if (!isnan(pos.x) && !isnan(pos.y)) {
      CGPoint display_pos = CGPointMake(
        pos.x * (f32)display_bounds.size.width,
        pos.y * (f32)display_bounds.size.height
      );
      CGWarpMouseCursorPosition(display_pos);
    }
  }
}

int main(int argc, const char **argv)
{
  UNUSED(argc); UNUSED(argv);

  MTDeviceRef trackpad = MTDeviceCreateDefault();
  if (!trackpad) {
    fprintf(stderr, "No multitouch devices found\n");
    return EXIT_FAILURE;
  }

  MTRegisterContactFrameCallback(trackpad, TouchFrameCallback);
  MTDeviceStart(trackpad, 0);

  while (1);
}
