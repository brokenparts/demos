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
