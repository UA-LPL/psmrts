set( PSMRTS_BULLET_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tracers/bullet/src/ )
set( PSMRTS_ELLIPSOID_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tracers/ellipsoid/src/ )
set( PSMRTS_NAIFDSK_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tracers/naifdsk/src/ )

set(PSMRTS_INCLUDE_DIRS  "${PSMRTS_INCLUDE_DIRS}"
                         ${PSMRTS_BULLET_HEADER_DIR}
                         ${PSMRTS_ELLIPSOID_HEADER_DIR}
                         ${PSMRTS_NAIFDSK_HEADER_DIR} )

set( PSMRTS_HEADER_FILES ${PSMRTS_HEADER_FILES}
                         ${PSMRTS_BULLET_HEADER_DIR}BulletShapeTracer.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}BulletSystemModel.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}BulletTracerModel.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletAllHitsRayCallback.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletClosestRayCallback.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletFactory.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletMeshMap.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletWorldModel.hpp )

set( PSMRTS_HEADER_FILES ${PSMRTS_HEADER_FILES}
                         ${PSMRTS_ELLIPSOID_HEADER_DIR}EllipsoidShapeTracer.hpp
                         ${PSMRTS_ELLIPSOID_HEADER_DIR}EllipsoidTracerModel.hpp )

set( PSMRTS_HEADER_FILES ${PSMRTS_HEADER_FILES}
                         ${PSMRTS_NAIFDSK_HEADER_DIR}DskKernelModel.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}DskSegment.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}KernelFileSystem.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}NaifDskShapeTracer.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}NaifDskTracerModel.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}NaifEllipsoidShape.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}NaifUtilities.hpp )
# message("TracersIncludes: ${PSMRTS_INCLUDE_DIRS}")
