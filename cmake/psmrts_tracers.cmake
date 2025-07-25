set( PSMRTS_BULLET_HEADER_DIR ${PSMRTS_TRACERS_DIR}/bullet/src/ )
set( PSMRTS_ELLIPSOID_HEADER_DIR ${PSMRTS_TRACERS_DIR}/ellipsoid/src/ )
set( PSMRTS_NAIFDSK_HEADER_DIR ${PSMRTS_TRACERS_DIR}/naifdsk/src/ )

set(PSMRTS_INCLUDE_DIRS  "${PSMRTS_INCLUDE_DIRS}"
                         ${PSMRTS_BULLET_HEADER_DIR}
                         ${PSMRTS_ELLIPSOID_HEADER_DIR}
                         ${PSMRTS_NAIFDSK_HEADER_DIR} )

set( PSMRTS_HEADER_FILES ${PSMRTS_HEADER_FILES}
                         ${PSMRTS_BULLET_HEADER_DIR}BulletTracer.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}BulletSystemModel.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}BulletTracerModel.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletAllHitsRayCallback.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletClosestRayCallback.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletFactory.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletMeshMap.hpp
                         ${PSMRTS_BULLET_HEADER_DIR}PsmrtsBulletWorldModel.hpp )

set( PSMRTS_HEADER_FILES ${PSMRTS_HEADER_FILES}
                         ${PSMRTS_ELLIPSOID_HEADER_DIR}EllipsoidTracer.hpp
                         ${PSMRTS_ELLIPSOID_HEADER_DIR}EllipsoidTracerModel.hpp )

set( PSMRTS_HEADER_FILES ${PSMRTS_HEADER_FILES}
                         ${PSMRTS_NAIFDSK_HEADER_DIR}DskKernelModel.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}DskSegment.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}KernelFileSystem.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}NaifDskTracer.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}NaifDskTracerModel.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}NaifEllipsoidShape.hpp
                         ${PSMRTS_NAIFDSK_HEADER_DIR}NaifUtilities.hpp )
# message("TracersIncludes: ${PSMRTS_INCLUDE_DIRS}")
