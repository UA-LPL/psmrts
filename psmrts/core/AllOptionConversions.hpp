#ifndef AllOptionConversions_hpp
#define AllOptionConversions_hpp


#include <psmrts/algorithms/conversions/AllConversionsVisitors.hpp>
#include <psmrts/core/ProductOption.hpp>

namespace psmrts { 

  namespace optvis = psmrts::algorithms::conversions;

  using OptionDoublesExtractor  = optvis::Extractor<ProductOption, optvis::DoublesVisitor>;
  using OptionBoolsExtractor    = optvis::Extractor<ProductOption, optvis::BoolsVisitor>;
  using OptionIntegersExtractor = optvis::Extractor<ProductOption, optvis::IntegersVisitor>;
  using OptionSizetsExtractor   = optvis::Extractor<ProductOption, optvis::SizetsVisitor>;
  using OptionStringsExtractor  = optvis::Extractor<ProductOption, optvis::StringsVisitor>;
  // using OptionJsonExtractor    = optvis::Extractor<ProductOption, optvis:: JsonVisitor>;
  
  using OptionStringsComparator  = optvis::Comparator<ProductOption, optvis::StringsVisitor>;
  using OptionDoublesComparator  = optvis::Comparator<ProductOption, optvis::DoublesVisitor>;
  using OptionSizetsComparator   = optvis::Comparator<ProductOption, optvis::SizetsVisitor>;
  using OptionIntegersComparator = optvis::Comparator<ProductOption, optvis::IntegersVisitor>;
  using OptionBoolsComparator    = optvis::Comparator<ProductOption, optvis::BoolsVisitor>;
  // using OptionJsonComparator   = optvis::Comparator<ProductOption, optvis::JsonVisitor>;

} // namespace psmrts

#endif
