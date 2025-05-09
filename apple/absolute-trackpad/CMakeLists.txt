cmake_minimum_required(VERSION 3.15)
project(raw-gestures)

include("../../common/cpp/CMakeLists.txt")

add_executable(absolutetrackpad
  "absolutetrackpad.mm"
)
target_link_libraries(absolutetrackpad PRIVATE
  demo-common
  "-framework Cocoa"
  "-F/System/Library/PrivateFrameworks"
  "-framework MultitouchSupport"
)
