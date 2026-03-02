# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES  ProductVoidVariant.hpp
                                  ProductCart.hpp
                                  ProductConfiguration.hpp
                                  ProductFeature.hpp
                                  ProductInventory.hpp
                                  ProductMaker.hpp
                                  ProductOption.hpp
                                  ProductOrder.hpp
                                  ProductProcessDispatch.hpp
                                  ProductProcessing.hpp
                                  ProductSpecification.hpp
                                  PRQProduct.hpp )
 
# Only add if requested
if(PSMRTS_BUILD_TESTS)
  add_subdirectory( tests )
endif(PSMRTS_BUILD_TESTS)
