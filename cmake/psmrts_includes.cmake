if(NOT PSMRTS_PROJECT_DIR)
  message(FATAL_ERROR "PSMRTS_PROJECT_DIR must be set prior to init of PSMRTS_INCLUDES")
endif()

set( PSMRTS_DIR ${PSMRTS_PROJECT_DIR}/psmrts )
set( PSMRTS_INCLUDE_DIRS ${PSMRTS_DIR}/src )
include(psmrts_shapes)
include(psmrts_tracers)

set( PSMRTS_INCLUDES "${PSMRTS_INCLUDE_DIRS}" )
# message( "PsmrtsIncludes: ${PSMRTS_INCLUDES}" )
