if(NOT PSMRTS_PROJECT_DIR)
  message(FATAL_ERROR "PSMRTS_PROJECT_DIR must be set prior to this init")
endif()

set(PSMRTS_INCLUDE_DIRS "${PSMRTS_INCLUDE_DIRS}" 
                         ${PSMRTS_FORMAT_DIR}/obj/src
                         ${PSMRTS_FORMAT_DIR}/ply/src )
# message("FormatsIncludes: ${PSMRTS_INCLUDE_DIRS}")
