#ifndef AllOptionConversions_hpp
#define AllOptionConversions_hpp


#include <psmrts/algorithms/conversions/AllConversionsVisitors.hpp>
#include <psmrts/core/ProductOption.hpp>

namespace psmrts { 

  using namespace psmrts::algorithms::conversions;

  using OptionDoublesExtractor  = Extractor<ProductOption, DoublesVisitor>;
  using OptionStringsExtractor  = Extractor<ProductOption, StringsVisitor>;
  // using OptionSizetsExtractor  = Extractor<ProductOption, SizetsVisitor>;
  using OptionIntegersExtractor = Extractor<ProductOption, IntegersVisitor>;
  using OptionBoolsExtractor    = Extractor<ProductOption, BoolsVisitor>;
  // using OptionJsonExtractor    = Extractor<ProductOption, JsonVisitor>;
  
  using OptionStringsComparator  = Comparator<ProductOption, StringsVisitor>;
  using OptionDoublesComparator  = Comparator<ProductOption, DoublesVisitor>;
  // using OptionSizetsComparator = Comparator<ProductOption, SizetsVisitor>;
  using OptionIntegersComparator = Comparator<ProductOption, IntegersVisitor>;
  using OptionBoolsComparator    = Comparator<ProductOption, BoolsVisitor>;
  // using OptionJsonComparator   = Comparator<ProductOption, JsonVisitor>;

} // namspace psmrts

#endif
