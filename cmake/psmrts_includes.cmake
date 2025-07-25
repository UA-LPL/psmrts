set( PSMRTS_BASE_HEADER_DIR ${PSMRTS_DIR}/src/ )

set( PSMRTS_INCLUDE_DIRS ${PSMRTS_BASE_HEADER_DIR})

set( PSMRTS_HEADER_FILES ${PSMRTS_BASE_HEADER_DIR}ProductParameter.hpp
                         ${PSMRTS_BASE_HEADER_DIR}ProductProcessDispatch.hpp
                         ${PSMRTS_BASE_HEADER_DIR}ProductRequest.hpp
                         ${PSMRTS_BASE_HEADER_DIR}ProductSpecification.hpp
                         ${PSMRTS_BASE_HEADER_DIR}psmrts_version.h
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsBuffer.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsBufferData.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsMeshData.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsParameters.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsProduct.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsRayTrace.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsRequest.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsTracer.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsShapeTracerAdapter.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsStridingBuffer.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsTracerModel.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsUtilities.hpp
                         ${PSMRTS_BASE_HEADER_DIR}PsmrtsVector3.hpp )

include(psmrts_shapes)
include(psmrts_tracers)

set( PSMRTS_INCLUDES "${PSMRTS_INCLUDE_DIRS}" )
# message( "PsmrtsIncludes: ${PSMRTS_INCLUDES}" )
