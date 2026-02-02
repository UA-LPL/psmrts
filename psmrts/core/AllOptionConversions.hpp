/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

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
