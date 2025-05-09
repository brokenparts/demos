add_library(demo-common INTERFACE)

# Always generate clangd compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Write executables to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Enable all compiler warnings
if(MSVC)
  target_compile_options(demo-common INTERFACE
    /W3
  )
else()
  # Assume GCC-like
  target_compile_options(demo-common INTERFACE
    -Wall -Wextra -Wpedantic
  )
endif()

# Common headers
target_include_directories(demo-common INTERFACE
  "${CMAKE_CURRENT_LIST_DIR}"
)

# Use C++20
target_compile_features(demo-common INTERFACE cxx_std_20)

#
# Dependency: SDL
# https://www.libsdl.org/
#
if(DEMO_NEEDS_SDL)
  include(FetchContent)
  FetchContent_Declare(
    demo-dep-SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         5ac37a8ffcf89da390404c1016833d56e2d67ae4  # release-3.2.12
  )
  FetchContent_MakeAvailable(demo-dep-SDL3)
  target_link_libraries(demo-common INTERFACE
    SDL3::SDL3-shared
  )
endif()
