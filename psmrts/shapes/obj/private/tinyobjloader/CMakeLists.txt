# Add sources current directory to include path
target_sources(psmrts PRIVATE tinyobjloader.cpp 
                         PUBLIC
                           FILE_SET psmrts_headers 
                             TYPE HEADERS 
                             FILES tiny_obj_loader.h
              )

if(BUILD_COVERAGE)
  set(_tiny_sources "tinyobjloader.cpp;tiny_obj_loader.h")
  list(TRANSFORM _tiny_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
  psmrts_register_code_coverage(EXCLUDES "${_tiny_sources}")
  unset(_tiny_sources)
endif()
