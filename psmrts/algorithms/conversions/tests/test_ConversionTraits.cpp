#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/algorithms/conversions/AllConversionsVisitors.hpp>

// Extractor tyoes
using DoublesVisitor   = psmrts::algorithms::conversions::DoublesVisitor;
using StringsVisitor   = psmrts::algorithms::conversions::StringsVisitor;
using SizetsVisitor    = psmrts::algorithms::conversions::SizetsVisitor;

using DoublesExtractor = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, DoublesVisitor>;
using StringsExtractor = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, StringsVisitor>;
using SizetsExtractor  = psmrts::algorithms::conversions::Extractor<psmrts::ProductOption, SizetsVisitor>;

using DoublesComparator = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, DoublesVisitor>;
using StringsComparator = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, StringsVisitor>;
using SizetsComparator  = psmrts::algorithms::conversions::Comparator<psmrts::ProductOption, SizetsVisitor>;


TEST_CASE("ConversionTraits Basic Test", "[conversions][option][traits]") {

  psmrts::ProductOption option_int( "integer", 100 );
  DoublesExtractor xtr_d( option_int );

  std::vector<double> data;
  auto v   = xtr_d.create_visitor( data, option_int, xtr_d.traits() );
  auto cvp = v.parameters();

  CHECK( option_int.size() == 1 );
  CHECK( data.capacity()   == 1 );

  CHECK( cvp.index()       == 0 );
  CHECK( cvp.count()       == 1 );  

  CHECK( cvp.traits().digits()    == 9 );  
  CHECK( cvp.traits().tolerance() == 1.0e-9 );  
  CHECK( cvp.traits().spaces()    == -1 );

  CHECK( xtr_d.get()   == 100.0 );
  CHECK( xtr_d.front() == 100.0 );
  CHECK( xtr_d.back()  == 100.0 );

}

TEST_CASE("ConversionParamaters Indexing Test", "[conversions][option][traits][indexing]") {

  auto d = psmrts::ProductOption("d", 1.0);
  CHECK( d.size() == 1 );
  
  auto xtr = StringsExtractor(d, "null" );
  CHECK( xtr.size() == 1 );

  std::vector<std::string> data;
  auto v   = xtr.create_visitor( data, d, xtr.traits(), "null" );
  auto cvp = xtr.compute_range( 0, 3, d.size() );

  CHECK( cvp.index() == 0 );
  CHECK( cvp.count() == 3 );

  CHECK( cvp.done( 0, d.size() )      == false );
  CHECK( cvp.done( 1, d.size() )      == false );
  CHECK( cvp.done( 2, d.size() )      == false );
  CHECK( cvp.done( 3, d.size() )      == true );
  CHECK( cvp.done( 4, d.size() )      == true );
  CHECK( cvp.done( 10, d.size() )     == true );
  CHECK( cvp.done( 1000, d.size() )   == true );

  CHECK( cvp.add_valid_value(0, d.size() )    == true );
  CHECK( cvp.add_valid_value(1, d.size() )    == false );
  CHECK( cvp.add_valid_value(2, d.size() )    == false );
  CHECK( cvp.add_valid_value(3, d.size() )    == false );
  CHECK( cvp.add_valid_value(4, d.size() )    == false );
  CHECK( cvp.add_valid_value(10, d.size() )   == false );
  CHECK( cvp.add_valid_value(1000, d.size() ) == false );

  CHECK( cvp.dataset_index(0, d.size() )    == 0 );
  CHECK( cvp.dataset_index(1, d.size() )    == 0 );
  CHECK( cvp.dataset_index(2, d.size() )    == 0 );
  CHECK( cvp.dataset_index(3, d.size() )    == 0 );
  CHECK( cvp.dataset_index(4, d.size() )    == 0 );
  CHECK( cvp.dataset_index(10, d.size() )   == 0 );
  CHECK( cvp.dataset_index(1000, d.size() ) == 0 );  

  CHECK( cvp.traits().digits()     == 9 );
  CHECK( cvp.traits().tolerance()  == 1.0e-9 );
  CHECK( cvp.traits().spaces()     == -1 );

  CHECK( xtr.get()   == "1.000000000" );
  CHECK( xtr.front() == "1.000000000" );
  CHECK( xtr.back()  == "1.000000000" );

  CHECK( xtr.size() == 1 );
  CHECK( xtr.default_value() == "null" );
  CHECK( xtr.get_all( 1, 2) == std::vector<std::string>( { "null", "null"} ) );
  CHECK( xtr.get(1) == "null" );
  
  auto e = psmrts::ProductOption("e", xtr.get_all(1, 10) );
  CHECK( e.size() == 10 );
  CHECK( e.to_string(0) == "null" );
}
