# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES  AllOptionConversions.hpp
                                  PsmrtsBuffer.hpp
                                  PsmrtsBufferData.hpp
                                  PsmrtsCache.hpp
                                  PsmrtsContainer.hpp
                                  PsmrtsFactory.hpp
                                  PsmrtsInventory.hpp
                                  PsmrtsInvoice.hpp
                                  PsmrtsJson.hpp
                                  PsmrtsMeshData.hpp
                                  PsmrtsProduct.hpp
                                  PsmrtsRayTrace.hpp
                                  PsmrtsRequest.hpp
                                  PsmrtsStridingBuffer.hpp
                                  PsmrtsTranslations.hpp
                                  PsmrtsUtilities.hpp
                                  PsmrtsVector3.hpp
                                  psmrts_version.h )

add_subdirectory( products )

# Only add if requested
if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
