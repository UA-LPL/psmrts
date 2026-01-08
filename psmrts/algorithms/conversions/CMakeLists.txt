# Add sources current directory to include path
target_sources( psmrts PUBLIC
                         FILE_SET psmrts_headers
                           TYPE HEADERS 
                           FILES AllConversionsVisitors.hpp
                                 Comparator.hpp
                                 ConversionTraits.hpp
                                 DoublesVisitor.hpp
                                 Extractor.hpp                           
                                 OptionBools.hpp
                                 OptionStrings.hpp
                                 OptionDoubles.hpp
                                 OptionInts.hpp
                                 StringsVisitor.hpp                                 
              )

# Only add if requested
if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
