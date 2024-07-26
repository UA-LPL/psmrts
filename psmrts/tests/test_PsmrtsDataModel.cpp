#include <psmrts_catch2_environment.hpp>

#define PSMRTS_BOUNDS_CHECK 1
#include <PsmrtsVector3.hpp>
#include <PsmrtsDataModel.hpp>


TEST_CASE( "PsmrtsDataModel Default Test", "[datamodel][buffer][default]") {

  psmrts::PsmrtsDataModel<> p_model  = psmrts::PsmrtsDataModel<>();

  CHECK( p_model.size()        == 0 );
  CHECK( p_model.volume_size() == 0 );

  CHECK( p_model.vector_size() == 3 );

  CHECK( p_model.stride_size() == 1 );

  CHECK_THROWS( p_model.value( 0 ) );
  CHECK_THROWS( p_model( 0 ) );
}

TEST_CASE( "PsmrtsDataModel (double) Double Test", "[datamodel][buffer][double]") {

  typedef psmrts::PsmrtsDataModel<double> ObjVectorData;

  const size_t n_data = 100;
  auto p_model  = ObjVectorData( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.vector_size()     == 3 );
  CHECK( p_model.volume_size()     == (  n_data * p_model.stride_size() ) );


  CHECK( p_model.stride_size()     == 24 );
  CHECK( p_model.stride_size()     == 3 * sizeof( double ) );
  CHECK( p_model.stride_size()     == sizeof( ObjVectorData::value_type ) * p_model.vector_size() );

  CHECK_NOTHROW( p_model.value( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

#if 0
  CHECK( p_model.distance( 0 ) == 0 );
  CHECK( p_model.distance( 1 ) == 3 );
  CHECK( p_model.distance( 10 ) == 30 );
  CHECK( p_model.distance( n_data - 1 ) == ( ( n_data - 1 ) * p_model.vector_size() ) );
#endif
}

TEST_CASE( "PsmrtsDataModel (int) Integer Test", "[datamodel][buffer][integer]") {

  typedef psmrts::PsmrtsDataModel<int> ObjIndexData;

  const size_t n_data = 100;
  auto p_model  = ObjIndexData( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.vector_size()     == 3 );
  CHECK( p_model.volume_size()     == (  n_data * p_model.stride_size() ) );


  CHECK( p_model.stride_size()     == 12 );
  CHECK( p_model.stride_size()     == 3 * sizeof( int ) );
  CHECK( p_model.stride_size()     == sizeof( ObjIndexData::value_type ) * p_model.vector_size() );

  CHECK_NOTHROW( p_model.value( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}


TEST_CASE( "PsmrtsDataModel (float) Float Test", "[datamodel][buffer][float]") {

  typedef psmrts::PsmrtsDataModel<float> ObjIndexData;

  const size_t n_data = 100;
  auto p_model  = ObjIndexData( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.vector_size()     == 3 );
  CHECK( p_model.volume_size()     == (  n_data * p_model.stride_size() ) );


  CHECK( p_model.stride_size()     == 12 );
  CHECK( p_model.stride_size()     == 3 * sizeof( float ) );
  CHECK( p_model.stride_size()     == sizeof( ObjIndexData::value_type ) * p_model.vector_size() );

  CHECK_NOTHROW( p_model.value( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}

TEST_CASE( "PsmrtsDataModel (unsigned char) Byte Test", "[datamodel][buffer][byte]") {

  typedef unsigned char  UCharType;

  const size_t n_data = 100;
  auto p_model  = psmrts::PsmrtsDataModel<UCharType>( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.vector_size()     == 3 );
  CHECK( p_model.volume_size()     == ( n_data * p_model.stride_size() ) );


  CHECK( p_model.stride_size()     == 3 );
  CHECK( p_model.stride_size()     == 3 * sizeof( unsigned char ) );
  CHECK( p_model.stride_size()     == 3 * sizeof( UCharType) );
  CHECK( p_model.stride_size()     == sizeof( psmrts::PsmrtsDataModel<UCharType>::value_type ) * p_model.vector_size() );

  CHECK_NOTHROW( p_model.value( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}


TEST_CASE( "PsmrtsDataModel (double) Data Values Test", "[datamodel][buffer][double][values]") {

  typedef psmrts::PsmrtsVector3<double> ObjVectorData;

  const int n_data = 10;
  auto p_model  = ObjVectorData( n_data );

  CHECK( p_model.size()         == n_data );
  CHECK( p_model.vector_size()  == 3 );
  CHECK( p_model.stride_size()  == 24 );
  CHECK( p_model.volume_size()  == ( n_data * p_model.stride_size() ) );

  double value = 1.0;
  for ( int n = 0 ; n < p_model.size() ; n++ ) {
    ObjVectorData::vector_reference data_t = p_model( n ); 
    for ( int v = 0 ; v < data_t.size() ; v++ ) {
      data_t[v] = value++; 
    }
  }

  ObjVectorData::vector_reference data_0 = p_model( 0 ); 
  CHECK( data_0[0] == 1.0 ); 
  CHECK( data_0[1] == 2.0 ); 
  CHECK( data_0[2] == 3.0 );
  // test at using same process
  
  ObjVectorData::vector_type at_0 = p_model.value( 0 ); 
  CHECK( data_0[0] == at_0[0]); 
  CHECK( data_0[1] == at_0[1] ); 
  CHECK( data_0[2] == 3.0 );
 // Look up Eigen::Map compared to Eigen::Vector3d 

  ObjVectorData::vector_reference data_1 = p_model( 1 ); 
  CHECK( data_1[0] == 4.0 ); 
  CHECK( data_1[1] == 5.0 ); 
  CHECK( data_1[2] == 6.0 ); 
  // test at using same process

  ObjVectorData::vector_reference data_n = p_model( n_data - 1 ); 
  CHECK( data_n[0] == 28.0 ); 
  CHECK( data_n[1] == 29.0 ); 
  CHECK( data_n[2] == 30.0 ); 

  auto p_model_c = p_model.slice( 1, 2 );
  CHECK ( p_model_c.size()        == 2 );
  CHECK ( p_model_c.stride_size() == p_model.stride_size() );
  CHECK ( p_model_c.stride_size() == 24 );

  ObjVectorData::vector_reference data_c = p_model_c( 0 );
  CHECK( data_c[0] == 4.0 ); 
  CHECK( data_c[1] == 5.0 ); 
  CHECK( data_c[2] == 6.0 ); 

  ObjVectorData::vector_reference data_c1 = p_model_c( 1 );
  CHECK( data_c1[0] == 7.0 ); 
  CHECK( data_c1[1] == 8.0 ); 
  CHECK( data_c1[2] == 9.0 ); 

  CHECK_THROWS( p_model_c(3) );
  CHECK_THROWS ( p_model_c.value(3) );
}


TEST_CASE( "PsmrtsDataModel Slice / Deep Copy Test", "[datamodel][buffer][slice][copy]") {
    
  // typedef psmrts::PsmrtsDataModel<int> ObjIndexData;
  typedef psmrts::PsmrtsVector3i ObjIndexData;

  const size_t n_data = 10;
  auto p_model  = ObjIndexData( n_data );
  auto p_copy = p_model.deep_copy();

  CHECK ( &p_model != &p_copy );
  CHECK ( p_model.value(0) == p_copy.value(0) );
  CHECK ( p_model.value(9) == p_copy.value(9) );
  
  auto p_slice = p_model.slice(2, 8);

  CHECK ( p_model.size() != p_slice.size() );
  CHECK ( p_model.value(2) == p_slice.value(0) );
  
}