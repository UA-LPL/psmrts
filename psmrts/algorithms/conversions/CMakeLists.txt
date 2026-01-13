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
                                 StringsVisitor.hpp                                 
              )

# Only add if requested
if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
