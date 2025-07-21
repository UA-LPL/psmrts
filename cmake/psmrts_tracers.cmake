if(NOT PSMRTS_PROJECT_DIR)
  message(FATAL_ERROR "PSMRTS_PROJECT_DIR must be set prior to this init")
endif()

set(PSMRTS_INCLUDE_DIRS "${PSMRTS_INCLUDE_DIRS}" 
                         ${PSMRTS_TRACERS_DIR}/bullet/src
                         ${PSMRTS_TRACERS_DIR}/ellipsoid/src
                         ${PSMRTS_TRACERS_DIR}/naifdsk/src )
# message("TracersIncludes: ${PSMRTS_INCLUDE_DIRS}")
