# Add sources current directory to include path
target_sources( psmrts PUBLIC ProductConfiguration.hpp
                              PsmrtsMeshData.hpp
                              PsmrtsStridingBuffer.hpp
                              ProductFeature.hpp
                              ProductOption.hpp
                              ProductProcessDispatch.hpp
                              PsmrtsUtilities.hpp
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
