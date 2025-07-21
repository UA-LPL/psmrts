if(NOT PSMRTS_PROJECT_DIR)
  message(FATAL_ERROR "PSMRTS_PROJECT_DIR must be set prior to this init")
endif()

set(PSMRTS_INCLUDE_DIRS "${PSMRTS_INCLUDE_DIRS}" 
                         ${PSMRTS_SHAPES_DIR}/dsk/src
                         ${PSMRTS_SHAPES_DIR}/obj/src
                         ${PSMRTS_SHAPES_DIR}/ply/src )
# message("ShapesIncludes: ${PSMRTS_INCLUDE_DIRS}")
