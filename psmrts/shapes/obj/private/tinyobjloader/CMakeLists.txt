# Add sources current directory to include path
target_sources(psmrts PRIVATE tinyobjloader.cpp 
                         PUBLIC
                           FILE_SET psmrts_headers 
                             TYPE HEADERS 
                             FILES tiny_obj_loader.h
              )

if(BUILD_COVERAGE)
  psmrts_register_code_coverage(EXCLUDES tinyobjloader.cpp tiny_obj_loader.h)
endif()              