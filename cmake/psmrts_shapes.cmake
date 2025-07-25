set( PSMRTS_DSK_HEADER_DIR ${PSMRTS_SHAPES_DIR}/dsk/src/ )
set( PSMRTS_OBJ_HEADER_DIR ${PSMRTS_SHAPES_DIR}/obj/src/ )
set( PSMRTS_PLY_HEADER_DIR ${PSMRTS_SHAPES_DIR}/ply/src/ )

set( PSMRTS_INCLUDE_DIRS  ${PSMRTS_INCLUDE_DIRS} 
                          ${PSMRTS_DSK_HEADER_DIR}
                          ${PSMRTS_OBJ_HEADER_DIR}
                          ${PSMRTS_PLY_HEADER_DIR} )

set( PSMRTS_HEADER_FILES ${PSMRTS_HEADER_FILES}
                         ${PSMRTS_DSK_HEADER_DIR}DskShape.hpp
                         ${PSMRTS_DSK_HEADER_DIR}PsmrtsDSKFormat.hpp )

set( PSMRTS_HEADER_FILES ${PSMRTS_HEADER_FILES}
                         ${PSMRTS_OBJ_HEADER_DIR}ObjShape.hpp
                         ${PSMRTS_OBJ_HEADER_DIR}PsmrtsOBJFormat.hpp
                         ${PSMRTS_OBJ_HEADER_DIR}tiny_obj_loader.h )

set( PSMRTS_HEADER_FILES ${PSMRTS_HEADER_FILES}
                         ${PSMRTS_PLY_HEADER_DIR}PlyShape.hpp
                         ${PSMRTS_PLY_HEADER_DIR}PsmrtsPLYFormat.hpp
                         ${PSMRTS_PLY_HEADER_DIR}miniply.h )
# message("ShapesIncludes: ${PSMRTS_INCLUDE_DIRS}")
