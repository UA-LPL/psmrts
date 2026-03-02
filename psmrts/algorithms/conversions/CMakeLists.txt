# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers
                           TYPE HEADERS 
                           FILES AllConversionsVisitors.hpp
                                 BoolsVisitor.hpp
                                 Comparator.hpp
                                 ConversionTraits.hpp
                                 DoublesVisitor.hpp
                                 Extractor.hpp                           
                                 IntegersVisitor.hpp
                                 SizetsVisitor.hpp
                                 StringsVisitor.hpp                                 
              )

# Only add if requested
if(PSMRTS_BUILD_TESTS)
  add_subdirectory( tests )
endif(PSMRTS_BUILD_TESTS)
