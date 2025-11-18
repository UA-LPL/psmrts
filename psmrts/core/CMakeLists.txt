# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES  ProductConfiguration.hpp
                                  ProductFeature.hpp
                                  ProductInventory.hpp
                                  ProductOption.hpp
                                  ProductProcessDispatch.hpp
                                  ProductSpecification.hpp
                                  PRQProduct.hpp
                                  PsmrtsBuffer.hpp
                                  PsmrtsBufferData.hpp
                                  PsmrtsCache.hpp
                                  PsmrtsFactory.hpp
                                  PsmrtsInventory.hpp
                                  PsmrtsJson.hpp
                                  PsmrtsMeshData.hpp
                                  PsmrtsProduct.hpp
                                  PsmrtsRayTrace.hpp
                                  PsmrtsRequest.hpp
                                  PsmrtsStridingBuffer.hpp
                                  PsmrtsUtilities.hpp
                                  PsmrtsVector3.hpp
              )

# Only add if requested
if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
