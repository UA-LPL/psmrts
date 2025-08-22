# Add sources current directory to include path
target_sources( psmrts PUBLIC PsmrtsMeshData.hpp
                              PsmrtsStridingBuffer.hpp
                              ProductParameter.hpp
                              PsmrtsParameter.hpp
                              ProductProcessDispatch.hpp
                              PsmrtsPriorityTracer.hpp
                              PsmrtsUtilities.hpp
                              ProductRequest.hpp
                              PsmrtsProduct.hpp
                              PsmrtsVector3.hpp
                              ProductSpecification.hpp
                              PsmrtsRayTrace.hpp
                              PsmrtsBuffer.hpp
                              PsmrtsRequest.hpp
                              PsmrtsBufferData.hpp )

# Only add if requested
if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
