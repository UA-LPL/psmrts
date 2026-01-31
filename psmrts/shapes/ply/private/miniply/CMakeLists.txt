# Add sources current directory to include path
target_sources(psmrts PRIVATE miniply.cpp miniply.h )

if(BUILD_COVERAGE)
  set(_mini_sources "miniply.cpp;miniply.h")
  list(TRANSFORM _mini_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
  psmrts_register_code_coverage(EXCLUDES "${_mini_sources}")
  unset(_mini_sources)
endif()
