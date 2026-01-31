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

TEST_CASE("Extractor Integers Test", "[extractor][integers]") {

  psmrts::ProductOption option_d1( "integer", -123 );
  psmrts::ProductOption option_d2( "integer", 42 );
  psmrts::ProductOption option_string( "integer", "527" );

  CHECK( OptionIntegersExtractor( option_d1 ).get()     == -123 );
  CHECK( OptionIntegersExtractor( option_d1 ).get(0)    == -123 );
  CHECK( OptionIntegersExtractor( option_d1 ).get(1)    == std::numeric_limits<int>::min() );
  CHECK( OptionIntegersExtractor( option_d1 ).get_all() == std::vector<int> { -123 } );

  CHECK( OptionIntegersExtractor( option_d2 ).get()     == 42 );
  CHECK( OptionIntegersExtractor( option_d2 ).get(0)    == 42 );
  CHECK( OptionIntegersExtractor( option_d2 ).get(1)    == std::numeric_limits<int>::min() );
  CHECK( OptionIntegersExtractor( option_d2 ).get_all() == std::vector<int> { 42 } );

  CHECK( OptionIntegersExtractor( option_string ).get()  == 527 );
  CHECK( OptionIntegersExtractor( option_string ).get(0) == 527 );
  CHECK( OptionIntegersExtractor( option_string ).get(1) == std::numeric_limits<int>::min() );
  CHECK( OptionIntegersExtractor( option_string ).get_all() == std::vector<int> { 527 } );

  psmrts::ProductOption bool_t( "bool", true );
  CHECK ( bool_t.type() == psmrts::ProductOption::PsmrtsBoolean );
  CHECK( OptionIntegersExtractor( bool_t ).get()  == 1 );
  CHECK( OptionIntegersExtractor( bool_t ).get_all() == std::vector<int> { 1 } );

  psmrts::ProductOption dbl_t( "double", 42.360 );
  CHECK ( dbl_t.type() == psmrts::ProductOption::PsmrtsDouble );
  CHECK( OptionIntegersExtractor( dbl_t ).get()  == 42 );
  CHECK( OptionIntegersExtractor( dbl_t ).get_all() == std::vector<int> { 42 } );

  size_t s_t = 1011;
  psmrts::ProductOption sizet_t( "size_t", s_t );
  CHECK ( sizet_t.type() == psmrts::ProductOption::PsmrtsSizeT );
  CHECK( OptionIntegersExtractor( sizet_t ).get()     == 1011 );
  CHECK( OptionIntegersExtractor( sizet_t ).get_all() == std::vector<int> { 1011 } );

  char j_obj[] = R"({"jarray":[5.0, 4, "3.2", true, false, "badone"]})";
  psmrts::ProductOption json_t( psmrts::json_utils::parse_json_string( j_obj ) );
  OptionIntegersExtractor json_xtr( json_t );
  CHECK ( json_xtr.name() == "jarray" );
  CHECK ( json_xtr.default_value() == std::numeric_limits<int>::min() );
  CHECK ( json_xtr.size() == 6 );
  CHECK ( json_xtr.get(0) == 5 );
  CHECK ( json_xtr.get(0) == json_xtr.front() );
  CHECK ( json_xtr.get(1) == 4 );
  CHECK ( json_xtr.get(2) == 3 );
  CHECK ( json_xtr.get(3) == 1 ); // min()
  CHECK ( json_xtr.get(4) == 0 ); // min()
  CHECK ( json_xtr.get(5) == std::numeric_limits<int>::min() );
  CHECK ( json_xtr.back() == std::numeric_limits<int>::min() );
  CHECK ( json_xtr.get(6) == std::numeric_limits<int>::min() );

  psmrts::ProductOption i_array( "darray", { 1, 2, 3, 4, 5 } );
  OptionIntegersExtractor iarray_xtr( i_array );
  CHECK( iarray_xtr.size() == 5 );
  CHECK( iarray_xtr.name() == "darray" );
  CHECK( iarray_xtr.get()  == 1 );
  CHECK( iarray_xtr.get(0) == 1 );
  CHECK( iarray_xtr.get()  == iarray_xtr.get(0) );
  CHECK( iarray_xtr.get(1) == 2 );
  CHECK( iarray_xtr.get(2) == 3 );
  CHECK( iarray_xtr.get(3) == 4 );
  CHECK( iarray_xtr.get(4) == 5 );
  CHECK( iarray_xtr.get(5) == std::numeric_limits<int>::min() );

  std::vector<int> i_t;
  CHECK( iarray_xtr.get_all( i_t )  == std::vector<int>{ 1, 2, 3, 4, 5 } );
  CHECK( iarray_xtr.get_all( 0, 3 ) == std::vector<int>{ 1, 2, 3  } );
  CHECK( iarray_xtr.get_all( 1, 2 ) == std::vector<int>{ 2, 3  } );
  CHECK( iarray_xtr.get_all( 3, 2 ) == std::vector<int>{ 4, 5 } );
  CHECK( iarray_xtr.get_all( 4, 1 ) == std::vector<int>{ 5 } );

  // Try constant initialization
  psmrts::ProductOption init_t( "init_t", "2" );
  OptionIntegersExtractor init_xtr( init_t, 10 );
  CHECK( init_xtr.size()             == 1 );
  CHECK( init_xtr.container().type() == psmrts::ProductOption::PsmrtsString );
  CHECK( init_xtr.get( 0 )           == 2 );
  CHECK( init_xtr.get_all( 1, 5 )    == std::vector<int>{ 10, 10, 10, 10, 10 } );
}

TEST_CASE("Extractor Bools Test", "[extractor][bools]") {
  psmrts::ProductOption option_d1( "bool", true );
  psmrts::ProductOption option_d2( "bool", false );
  psmrts::ProductOption option_string( "bool", "true" );

  CHECK( OptionBoolsExtractor( option_d1 ).get()     == true );
  CHECK( OptionBoolsExtractor( option_d1 ).get(0)    == true );
  CHECK( OptionBoolsExtractor( option_d1 ).get(1)    == false );
  CHECK( OptionBoolsExtractor( option_d1 ).get_all() == std::vector<bool> { true } );

  CHECK( OptionBoolsExtractor( option_d2 ).get()     == false );
  CHECK( OptionBoolsExtractor( option_d2 ).get(0)    == false );
  CHECK( OptionBoolsExtractor( option_d2 ).get(1)    == false );
  CHECK( OptionBoolsExtractor( option_d2 ).get_all() == std::vector<bool> { false } );

  CHECK( OptionBoolsExtractor( option_string ).get()     == true );
  CHECK( OptionBoolsExtractor( option_string ).get(0)    == true );
  CHECK( OptionBoolsExtractor( option_string ).get(1)    == false );
  CHECK( OptionBoolsExtractor( option_string ).get_all() == std::vector<bool> { true } );

  psmrts::ProductOption dbl_t( "double", 42.123 );
  CHECK ( dbl_t.type() == psmrts::ProductOption::PsmrtsDouble );
  CHECK( OptionBoolsExtractor( dbl_t ).get()     == true );
  CHECK( OptionBoolsExtractor( dbl_t ).get_all() == std::vector<bool> { true } );

  psmrts::ProductOption int_t( "integer", 4 );
  CHECK ( int_t.type() == psmrts::ProductOption::PsmrtsInteger );
  CHECK( OptionBoolsExtractor( int_t ).get()     == true );
  CHECK( OptionBoolsExtractor( int_t ).get_all() == std::vector<bool> { true } );

  size_t s_t = 1011;
  psmrts::ProductOption sizet_t( "size_t", s_t );
  CHECK ( sizet_t.type() == psmrts::ProductOption::PsmrtsSizeT );
  CHECK( OptionBoolsExtractor( sizet_t ).get()     == true );
  CHECK( OptionBoolsExtractor( sizet_t ).get_all() == std::vector<bool> { true } );

  char j_obj[] = R"({"jarray":[5.0, 4, "3.2", true, false, 0, "badone"]})";
  psmrts::ProductOption json_t( psmrts::json_utils::parse_json_string( j_obj ) );
  OptionBoolsExtractor json_xtr( json_t );
  CHECK ( json_xtr.name() == "jarray" );
  CHECK ( json_xtr.default_value() == false );
  CHECK ( json_xtr.size() == 7 );
  CHECK ( json_xtr.get(0) == true  ); // 5.0
  CHECK ( json_xtr.get(0) == json_xtr.front() ); // 5.0
  CHECK ( json_xtr.get(1) == true  ); // 4
  CHECK ( json_xtr.get(2) == false ); // "3.2", psmrts Bools conversion does not auto-convert numerical strings
  CHECK ( json_xtr.get(3) == true  ); // true
  CHECK ( json_xtr.get(4) == false ); // false
  CHECK ( json_xtr.get(5) == false ); // 0
  CHECK ( json_xtr.get(6) == false ); // "badone"
  CHECK ( json_xtr.back() == false );
  CHECK ( json_xtr.get(7) == false );

  psmrts::ProductOption b_array( "barray", { true, true, false, true, true } );
  OptionBoolsExtractor barray_xtr( b_array );
  CHECK( barray_xtr.size() == 5 );
  CHECK( barray_xtr.name() == "barray" );
  CHECK( barray_xtr.get()  == true  );
  CHECK( barray_xtr.get(0) == true  );
  CHECK( barray_xtr.get()  == barray_xtr.get(0) );
  CHECK( barray_xtr.get(1) == true  );
  CHECK( barray_xtr.get(2) == false );
  CHECK( barray_xtr.get(3) == true  );
  CHECK( barray_xtr.get(4) == true  );
  CHECK( barray_xtr.get(5) == false );

  std::vector<bool> b_t;
  CHECK( barray_xtr.get_all( b_t )  == std::vector<bool>{ true, true, false, true, true } );
  CHECK( barray_xtr.get_all( 0, 3 ) == std::vector<bool>{ true, true, false } );
  CHECK( barray_xtr.get_all( 1, 2 ) == std::vector<bool>{ true, false } );
  CHECK( barray_xtr.get_all( 3, 2 ) == std::vector<bool>{ true, true } );
  CHECK( barray_xtr.get_all( 4, 1 ) == std::vector<bool>{ true } );

  // Try constant initialization
  psmrts::ProductOption init_t( "init_t", "true" );
  OptionBoolsExtractor init_xtr( init_t, false );
  CHECK( init_xtr.size()             == 1 );
  CHECK( init_xtr.container().type() == psmrts::ProductOption::PsmrtsString );
  CHECK( init_xtr.get( 0 )           == true );
  CHECK( init_xtr.get_all( 1, 5 )    == std::vector<bool>{ false, false, false, false, false } );
}

TEST_CASE("Extractor Size_t Test", "[extractor][size_t]") {

  psmrts::ProductOption option_d1( "size_t", 123 );
  psmrts::ProductOption option_d2( "size_t", 42 );
  psmrts::ProductOption option_string( "size_t", "527" );

  CHECK( OptionSizetsExtractor( option_d1 ).get()     == 123 );
  CHECK( OptionSizetsExtractor( option_d1 ).get(0)    == 123 );
  CHECK( OptionSizetsExtractor( option_d1 ).get(1)    == std::numeric_limits<size_t>::max() );
  CHECK( OptionSizetsExtractor( option_d1 ).get_all() == std::vector<size_t> { 123 } );

  CHECK( OptionSizetsExtractor( option_d2 ).get()     == 42 );
  CHECK( OptionSizetsExtractor( option_d2 ).get(0)    == 42 );
  CHECK( OptionSizetsExtractor( option_d2 ).get(1)    == std::numeric_limits<size_t>::max() );
  CHECK( OptionSizetsExtractor( option_d2 ).get_all() == std::vector<size_t> { 42 } );

  CHECK( OptionSizetsExtractor( option_string ).get()  == 527 );
  CHECK( OptionSizetsExtractor( option_string ).get(0) == 527 );
  CHECK( OptionSizetsExtractor( option_string ).get(1) == std::numeric_limits<size_t>::max() );
  CHECK( OptionSizetsExtractor( option_string ).get_all() == std::vector<size_t> { 527 } );

  psmrts::ProductOption bool_t( "bool", true );
  CHECK ( bool_t.type() == psmrts::ProductOption::PsmrtsBoolean );
  CHECK( OptionSizetsExtractor( bool_t ).get()  == 1 );
  CHECK( OptionSizetsExtractor( bool_t ).get_all() == std::vector<size_t> { 1 } );

  psmrts::ProductOption dbl_t( "double", 42.360 );
  CHECK ( dbl_t.type() == psmrts::ProductOption::PsmrtsDouble );
  CHECK( OptionSizetsExtractor( dbl_t ).get()  == 42 );
  CHECK( OptionSizetsExtractor( dbl_t ).get_all() == std::vector<size_t> { 42 } );

  psmrts::ProductOption i_t( "integer", 1011 );
  CHECK ( i_t.type() == psmrts::ProductOption::PsmrtsInteger );
  CHECK( OptionSizetsExtractor( i_t ).get()     == 1011 );
  CHECK( OptionSizetsExtractor( i_t ).get_all() == std::vector<size_t> { 1011 } );

  char j_obj[] = R"({"jarray":[5.0, 4, "3.2", true, false, "badone"]})";
  psmrts::ProductOption json_t( psmrts::json_utils::parse_json_string( j_obj ) );
  OptionSizetsExtractor json_xtr( json_t );
  CHECK ( json_xtr.name() == "jarray" );
  CHECK ( json_xtr.default_value() == std::numeric_limits<size_t>::max() );
  CHECK ( json_xtr.size() == 6 );
  CHECK ( json_xtr.get(0) == 5 );
  CHECK ( json_xtr.get(0) == json_xtr.front() );
  CHECK ( json_xtr.get(1) == 4 );
  CHECK ( json_xtr.get(2) == 3 );
  CHECK ( json_xtr.get(3) == 1 ); // max
  CHECK ( json_xtr.get(4) == 0 ); // max
  CHECK ( json_xtr.get(5) == std::numeric_limits<size_t>::max() );
  CHECK ( json_xtr.back() == std::numeric_limits<size_t>::max() );
  CHECK ( json_xtr.get(6) == std::numeric_limits<size_t>::max() );

  psmrts::ProductOption st_array( "starray", { 1, 2, 3, 4, 5 } );
  OptionSizetsExtractor starray_xtr( st_array );
  CHECK( starray_xtr.size() == 5 );
  CHECK( starray_xtr.name() == "starray" );
  CHECK( starray_xtr.get()  == 1 );
  CHECK( starray_xtr.get(0) == 1 );
  CHECK( starray_xtr.get()  == starray_xtr.get(0) );
  CHECK( starray_xtr.get(1) == 2 );
  CHECK( starray_xtr.get(2) == 3 );
  CHECK( starray_xtr.get(3) == 4 );
  CHECK( starray_xtr.get(4) == 5 );
  CHECK( starray_xtr.get(5) == std::numeric_limits<size_t>::max() );

  std::vector<size_t> st_t;
  CHECK( starray_xtr.get_all( st_t ) == std::vector<size_t>{ 1, 2, 3, 4, 5 } );
  CHECK( starray_xtr.get_all( 0, 3 ) == std::vector<size_t>{ 1, 2, 3  } );
  CHECK( starray_xtr.get_all( 1, 2 ) == std::vector<size_t>{ 2, 3  } );
  CHECK( starray_xtr.get_all( 3, 2 ) == std::vector<size_t>{ 4, 5 } );
  CHECK( starray_xtr.get_all( 4, 1 ) == std::vector<size_t>{ 5 } );

  // Try constant initialization
  psmrts::ProductOption init_t( "init_t", "2" );
  OptionSizetsExtractor init_xtr( init_t, 10 );
  CHECK( init_xtr.size()             == 1 );
  CHECK( init_xtr.container().type() == psmrts::ProductOption::PsmrtsString );
  CHECK( init_xtr.get( 0 )           == 2 );
  CHECK( init_xtr.get_all( 1, 5 )    == std::vector<size_t>{ 10, 10, 10, 10, 10 } );
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

  OptionStringsExtractor s_xtr( s_array );
  optvis::ConversionParameters sparms = s_xtr.compute_range(1, s_xtr.size(), s_xtr.size() );
  CHECK( sparms.count() == 3 );
  CHECK( sparms.index() == 1 );

  CHECK( sparms.done(0, s_array.size() )            == false );
  CHECK( sparms.add_valid_value(0, s_array.size() ) == true );
  CHECK( sparms.dataset_index(0, s_array.size() )   == 1 );

  CHECK( sparms.done(1, s_array.size() )            == false );
  CHECK( sparms.add_valid_value(1, s_array.size() ) == true );
  CHECK( sparms.dataset_index(1, s_array.size() )   == 2 );

  CHECK( sparms.done(2, s_array.size() )            == false );
  CHECK( sparms.add_valid_value(2, s_array.size() ) == false );
  CHECK( sparms.dataset_index(2, s_array.size() )   == 2 );

  CHECK( sparms.done(3, s_array.size() )            == true );
  CHECK( sparms.add_valid_value(3, s_array.size() ) == false );
  CHECK( sparms.dataset_index(3, s_array.size() )   == 2 );

  CHECK( sparms.done(100, s_array.size() )            == true );
  CHECK( sparms.add_valid_value(100, s_array.size() ) == false );
  CHECK( sparms.dataset_index(100, s_array.size() )   == 2 );
}

TEST_CASE("Extractor Functor Test", "[extractor][parameters][functor]") {

  bool expected_add;
  size_t expected_index;
  auto ftester = [&] (const bool addit, const size_t index ) {
    CHECK( addit == expected_add );
    CHECK( index == expected_index );
  };

  optvis::ConversionParameters parms_t;

  expected_add = false;
  expected_index = 0;
  parms_t.extractor( 1, ftester );

  parms_t = optvis::ConversionParameters( 0, 1 );

  expected_add = true;
  expected_index = 0;
  parms_t.extractor( 1, ftester );  

  parms_t = optvis::ConversionParameters( 1, 1 );
  CHECK( parms_t.index() == 1 );
  CHECK( parms_t.count() == 1 );

  expected_add = false;
  expected_index = 0;
  parms_t.extractor( 1, ftester );  
}
