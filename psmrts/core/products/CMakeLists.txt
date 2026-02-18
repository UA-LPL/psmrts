# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES  AllOptionConversions.hpp
                                  ProductVoidVariant.hpp
                                  ProductCart.hpp
                                  ProductConfiguration.hpp
                                  ProductFeature.hpp
                                  ProductInventory.hpp
                                  ProductOption.hpp
                                  ProductOrder.hpp
                                  ProductProcessDispatch.hpp
                                  ProductProcessing.hpp
                                  ProductSpecification.hpp
                                  PRQProduct.hpp
                                  PsmrtsProduct.hpp )
 
# Only add if requested
if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
