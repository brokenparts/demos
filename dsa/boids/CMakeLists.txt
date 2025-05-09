cmake_minimum_required(VERSION 3.15)
project(boids)

set(DEMO_NEEDS_SDL TRUE)
include("../../common/cpp/CMakeLists.txt")

add_executable(boids
  "boids.cc"
)
target_link_libraries(boids PRIVATE
  demo-common
)
