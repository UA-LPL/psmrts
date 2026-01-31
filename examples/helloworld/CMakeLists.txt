# helloworld/CMakeLists.txt

cmake_minimum_required( VERSION 3.24 )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
set( CMAKE_C_STANDARD 17 )

project(helloworld VERSION 1.0 LANGUAGES CXX)

# Find the PSMRTS dependencies and then PSMRTS configuration
find_package( nlohmann_json CONFIG REQUIRED )
find_package( Eigen3        CONFIG REQUIRED )
find_package( cspice        CONFIG REQUIRED)
find_package( Bullet CONFIGS BulletConfig-float64.cmake REQUIRED)
find_package(psmrts         CONFIG REQUIRED)

# Add the C API sources and build specifics
add_executable( helloworld  helloworld.cpp )
target_link_libraries( helloworld PRIVATE psmrts::psmrts )
