# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

# Add option to build tests and tools
option(BUILD_TESTING   "Add PSMRTS unit tests"     OFF  )
option(BUILD_EXTRAS    "Add PSMRTS tools/extras"   OFF  )

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

# See https://github.com/Microsoft/vcpkg/issues/4497 as to why this
# is needed. Its a conflict with proj and nlohmann_json.
set(other_flags "${other_flags} -frtti -fvisibility-inlines-hidden")
set(other_flags "${other_flags} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${other_flags}")

# This is PROJECT_SOURCE_DIR=psmrts
project(psmrts)

# Set psmrts structs directory
set(PSMRTS_BUILD_DIR    ${CMAKE_BINARY_DIR})

find_package(Eigen3 CONFIG REQUIRED)
find_package(args CONFIG REQUIRED)
find_library(SPICE_LIB cspice)

# Add psmrts application
# add_executable(psmrts src/psmrts.cpp)

target_link_libraries(psmrts PRIVATE taywee::args)
target_link_libraries(psmrts PRIVATE "${SPICE_LIB}")
target_link_libraries(psmrts PRIVATE Eigen3::Eigen)

# Optionally add unit tests of psmrts classes
if(BUILD_TESTING)
  add_subdirectory( tests )
  enable_testing()
endif(BUILD_TESTING)

# Optionally add build of extra programs
if(BUILD_EXTRAS)
  add_subdirectory( extras )
endif(BUILD_EXTRAS)
