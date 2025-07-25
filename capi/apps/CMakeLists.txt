cmake_minimum_required( VERSION 3.20 )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

project( psmrts_app )

find_package( psmrts CONFIG REQUIRED )
find_package( spdlog CONFIG REQUIRED )
find_package( nlohmann_json CONFIG REQUIRED )
find_package( Eigen3 CONFIG REQUIRED )
find_package( Bullet CONFIGS BulletConfig-float64.cmake REQUIRED )
find_library( SPICE_LIB cspice )

# Add the C API sources and build specifics
add_executable( psmrts_capi_features  psmrts_capi_features.cpp )
target_link_libraries( psmrts_capi_features PRIVATE psmrts::psmrts_c )