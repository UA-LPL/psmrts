#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/algorithms/conversions/Extractor.hpp>
#include <psmrts/algorithms/conversions/AllConversionsVisitors.hpp>

// Visitor andExtractor types
namespace optvis = psmrts::algorithms::conversions;

using OptionDoublesExtractor  = optvis::Extractor<psmrts::ProductOption, optvis::DoublesVisitor>;
using OptionBoolsExtractor    = optvis::Extractor<psmrts::ProductOption, optvis::BoolsVisitor>;
using OptionIntegersExtractor = optvis::Extractor<psmrts::ProductOption, optvis::IntegersVisitor>;
using OptionSizetsExtractor   = optvis::Extractor<psmrts::ProductOption, optvis::SizetsVisitor>;
using OptionStringsExtractor  = optvis::Extractor<psmrts::ProductOption, optvis::StringsVisitor>;

TEST_CASE("Extractor Default Test", "[extractor][default]") {

  // default comparator constructors
  OptionBoolsExtractor    boolXtr;
  OptionDoublesExtractor  doublesXtr;
  OptionIntegersExtractor intsXtr;
  OptionSizetsExtractor   sizetsXtr;
  OptionStringsExtractor  stringsXtr;
  
  // validate sizes
  CHECK( boolXtr.size()    == 1 );
  CHECK( doublesXtr.size() == 1 );
  CHECK( intsXtr.size()    == 1 );
  CHECK( sizetsXtr.size()  == 1 );
  CHECK( stringsXtr.size() == 1 );

  // validate default ProductOption values
  CHECK( boolXtr.default_value()    == false );
  CHECK( psmrts::isnull( doublesXtr.default_value() ) );
  CHECK( intsXtr.default_value()    == std::numeric_limits<int>::min() );
  CHECK( sizetsXtr.default_value()  == std::numeric_limits<size_t>::max() );
  CHECK( stringsXtr.default_value() == "" );
}

TEST_CASE("Extractor Doubles Test", "[extractor][doubles]") {

  psmrts::ProductOption option_d1( "double", 1.2345678 );
  psmrts::ProductOption option_d2( "double", 3.14159154 );
  psmrts::ProductOption option_string( "double", "527.987" );

  CHECK( OptionDoublesExtractor( option_d1 ).get()     == 1.2345678 );
  CHECK( OptionDoublesExtractor( option_d1 ).get(0)    == 1.2345678 );
  CHECK( psmrts::isnull( OptionDoublesExtractor( option_d1 ).get(1) ) );
  CHECK( OptionDoublesExtractor( option_d1 ).get_all() == std::vector<double> { 1.2345678 } );

  CHECK( OptionDoublesExtractor( option_d2 ).get()     == 3.14159154 );
  CHECK( OptionDoublesExtractor( option_d2 ).get(0)    == 3.14159154 );
  CHECK( psmrts::isnull( OptionDoublesExtractor( option_d2 ).get(1) ) );
  CHECK( OptionDoublesExtractor( option_d2 ).get_all() == std::vector<double> { 3.14159154} );

  CHECK( OptionDoublesExtractor( option_string ).get()  == 527.987 );
  CHECK( OptionDoublesExtractor( option_string ).get(0) == 527.987 );
  CHECK( psmrts::isnull( OptionDoublesExtractor( option_string ).get(1) ) );
  CHECK( OptionDoublesExtractor( option_string ).get_all() == std::vector<double> { 527.987 } );

  psmrts::ProductOption bool_t( "bool", true );
  CHECK ( bool_t.type() == psmrts::ProductOption::PsmrtsBoolean );
  CHECK( OptionDoublesExtractor( bool_t ).get()  == 1.0 );
  CHECK( OptionDoublesExtractor( bool_t ).get_all() == std::vector<double> { 1.0 } );

  psmrts::ProductOption int_t( "integer", 4 );
  CHECK ( int_t.type() == psmrts::ProductOption::PsmrtsInteger );
  CHECK( OptionDoublesExtractor( int_t ).get()  == 4.0 );
  CHECK( OptionDoublesExtractor( int_t ).get_all() == std::vector<double> { 4.0 } );

  size_t s_t = 1011;
  psmrts::ProductOption sizet_t( "size_t", s_t );
  CHECK ( sizet_t.type() == psmrts::ProductOption::PsmrtsSizeT );
  CHECK( OptionDoublesExtractor( sizet_t ).get()  == 1011.0 );
  CHECK( OptionDoublesExtractor( sizet_t ).get_all() == std::vector<double> { 1011.0 } );

  char j_obj[] = R"({"jarray":[5.0, 4, "3.2", true, false, "badone"]})";
  psmrts::ProductOption json_t( psmrts::json_utils::parse_json_string( j_obj ) );
  OptionDoublesExtractor json_xtr( json_t );
  CHECK ( json_xtr.name() == "jarray" );
  CHECK ( psmrts::isnull( json_xtr.default_value() ) );
  CHECK ( json_xtr.size() == 6 );
  CHECK ( json_xtr.get(0) == 5.0 );
  CHECK ( json_xtr.get(0) == json_xtr.front() );
  CHECK ( json_xtr.get(1) == 4.0 );
  CHECK ( json_xtr.get(2) == 3.2 );
  CHECK ( json_xtr.get(3) == 1.0 );
  CHECK ( json_xtr.get(4) == 0.0 );
  CHECK ( psmrts::isnull( json_xtr.get(5) ) );
  CHECK ( psmrts::isnull( json_xtr.back() ) );
  CHECK ( psmrts::isnull( json_xtr.get(6) ) );

  psmrts::ProductOption d_array( "darray", { 1.0, 2.0, 3.0, 4.0, 5.0 } );
  OptionDoublesExtractor darray_xtr( d_array );
  CHECK( darray_xtr.size() == 5 );
  CHECK( darray_xtr.name() == "darray" );
  CHECK( darray_xtr.get()  == 1.0 );
  CHECK( darray_xtr.get(0) == 1.0 );
  CHECK( darray_xtr.get()  == darray_xtr.get(0) );
  CHECK( darray_xtr.get(1) == 2.0 );
  CHECK( darray_xtr.get(2) == 3.0 );
  CHECK( darray_xtr.get(3) == 4.0 );
  CHECK( darray_xtr.get(4) == 5.0 );
  CHECK( psmrts::isnull( darray_xtr.get(5) ) );

  std::vector<double> d_t;
  CHECK( darray_xtr.get_all( d_t )  == std::vector<double>{ 1.0, 2.0, 3.0, 4.0, 5.0 } );
  CHECK( darray_xtr.get_all( 0, 3 ) == std::vector<double>{ 1.0, 2.0, 3.0  } );
  CHECK( darray_xtr.get_all( 1, 2 ) == std::vector<double>{ 2.0, 3.0  } );
  CHECK( darray_xtr.get_all( 3, 2 ) == std::vector<double>{ 4.0, 5.0 } );
  CHECK( darray_xtr.get_all( 4, 1 ) == std::vector<double>{ 5.0 } );

  // Try constant initialization
  psmrts::ProductOption init_t( "init_t", "2.0" );
  OptionDoublesExtractor init_xtr( init_t, 10.0 );
  CHECK( init_xtr.size()             == 1 );
  CHECK( init_xtr.container().type() == psmrts::ProductOption::PsmrtsString );
  CHECK( init_xtr.get( 0 )           == 2.0 );
  CHECK( init_xtr.get_all( 1, 5 )    == std::vector<double>{ 10.0, 10.0, 10.0, 10.0, 10.0 } );
}

TEST_CASE("Extractor Visitor/Parameters/Traits Test", "[extractor][vistor][parameters][traits]") {

  psmrts::ProductOption s_array( "sarray", { "one", "two", "three" } );
  OptionStringsExtractor::TypeVector v_array; 
  optvis::StringsVisitor svis = OptionStringsExtractor::create_visitor( v_array, s_array );
  optvis::ConversionParameters cparms = svis.parameters();

  CHECK( s_array.size() == 3 );
  CHECK( cparms.count() == s_array.size() );
  CHECK( cparms.count() == 3 );
  CHECK( cparms.index() == 0 );

  CHECK( cparms.done(0, v_array.size() ) == false );
  CHECK( cparms.done(1, v_array.size() ) == false );
  CHECK( cparms.done(2, v_array.size() ) == false );
  CHECK( cparms.done(3, v_array.size() ) == true );
  CHECK( cparms.done(100, v_array.size() ) == true );

  optvis::ConversionTraits ctraits = svis.traits();
  CHECK( ctraits.digits()    == 9 );
  CHECK( ctraits.tolerance() == 1.0e-9 );
  CHECK( ctraits.spaces()    == -1 );

  s_array.visit( svis );
  CHECK( s_array.size() == v_array.size() );
  CHECK( svis.data()    == v_array );
  CHECK( v_array        == std::vector<std::string>( { "one", "two", "three" } ) );
}