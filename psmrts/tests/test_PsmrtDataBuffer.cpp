#include <psmrts_catch2_environment.hpp>

#define PSMRTS_BOUNDS_CHECK 1
#include <PsmrtsDataBuffer.hpp>


TEST_CASE( "PsmrtsDataBuffer Default Test", "[databuffer][default]") {

  psmrts::PsmrtsDataBuffer<> p_model);
  CHECK( p_model.size()  == 0 );
  CHECK_THROWS( p_model.at( 0 ) );
  CHECK_THROWS( p_model( 0 ) );
}

TEST_CASE( "PsmrtsDataBuffer (double) Double Test", "[databuffer][double]") {

  typedef psmrts::PsmrtsDataBuffer<double> ObjVectorData;

  const size_t n_data = 100;
  auto p_model  = ObjVectorData( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK_NOTHROW( p_model.at( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

  CHECK( p_model.distance( 0 ) == 0 );
  CHECK( p_model.distance( 1 ) == 3 );
  CHECK( p_model.distance( 10 ) == 30 );
  CHECK( p_model.distance( n_data - 1 ) == ( ( n_data - 1 ) * p_model.data_size() ) );

}

TEST_CASE( "PsmrtsDataBuffer (int) Integer Test", "[databuffer][integer]") {

  typedef psmrts::PsmrtsDataBuffer<int> ObjIndexData;

  const size_t n_data = 100;
  auto p_model  = ObjIndexData( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.data_size()       == 3 );
  CHECK( p_model.total_allocated() == (  n_data * p_model.data_size() ) );


  CHECK( p_model.scalar_size()     == 4 );
  CHECK( p_model.scalar_size()     == sizeof( int ) );
  CHECK( p_model.scalar_size()     == sizeof( ObjIndexData::value_type) );

  CHECK_NOTHROW( p_model.at( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}


TEST_CASE( "PsmrtsDataBuffer (float) Float Test", "[databuffer][float]") {

  typedef psmrts::PsmrtsDataBuffer<float> ObjIndexData;

  const size_t n_data = 100;
  auto p_model  = ObjIndexData( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.data_size()       == 3 );
  CHECK( p_model.total_allocated() == (  n_data * p_model.data_size() ) );


  CHECK( p_model.scalar_size()     == 4 );
  CHECK( p_model.scalar_size()     == sizeof( float ) );
  CHECK( p_model.scalar_size()     == sizeof( ObjIndexData::value_type) );

  CHECK_NOTHROW( p_model.at( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}

TEST_CASE( "PsmrtsDataBuffer (unsigned char) Byte Test", "[datamodel][buffer][byte]") {

  typedef unsigned char  UCharType;

  const size_t n_data = 100;
  auto p_model  = psmrts::PsmrtsDataBuffer<UCharType>( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.data_size()       == 3 );
  CHECK( p_model.total_allocated() == (  n_data * p_model.data_size() ) );


  CHECK( p_model.scalar_size()     == 1 );
  CHECK( p_model.scalar_size()     == sizeof( unsigned char ) );
  CHECK( p_model.scalar_size()     == sizeof( UCharType) );
  CHECK( p_model.scalar_size()     == sizeof( psmrts::PsmrtsDataBuffer<UCharType>::value_type) );

  CHECK_NOTHROW( p_model.at( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}


TEST_CASE( "PsmrtsDataBuffer (double) Data Values Test", "[datamodel][buffer][double][values]") {

  typedef psmrts::PsmrtsDataBuffer<double> ObjVectorData;

  const size_t n_data = 10;
  auto p_model  = ObjVectorData( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.data_size()       == 3 );
  CHECK( p_model.total_allocated() == (  n_data * p_model.data_size() ) );

  double value = 1.0;
  for ( int n = 0 ; n < p_model.size() ; n++ ) {
    ObjVectorData::data_reference data_t = p_model( n ); 
    for ( int v = 0 ; v < data_t.size() ; v++ ) {
      data_t[v] = value++; 
    }
  }

  ObjVectorData::data_reference data_0 = p_model( 0 ); 
  CHECK( data_0[0] == 1.0 ); 
  CHECK( data_0[1] == 2.0 ); 
  CHECK( data_0[2] == 3.0 );
  // test at using same process
  
  ObjVectorData::vector_type at_0 = p_model.at( 0 ); 
  CHECK( data_0[0] == at_0[0]); 
  CHECK( data_0[1] == at_0[1] ); 
  CHECK( data_0[2] == 3.0 );
 // Look up Eigen::Map compared to Eigen::Vector3d 

  ObjVectorData::data_reference data_1 = p_model( 1 ); 
  CHECK( data_1[0] == 4.0 ); 
  CHECK( data_1[1] == 5.0 ); 
  CHECK( data_1[2] == 6.0 ); 
  // test at using same process

  ObjVectorData::data_reference data_n = p_model( n_data - 1 ); 
  CHECK( data_n[0] == 28.0 ); 
  CHECK( data_n[1] == 29.0 ); 
  CHECK( data_n[2] == 30.0 ); 

  auto p_model_c = ObjVectorData( data_1.data(), 2);
  ObjVectorData::data_reference data_c = p_model_c( 0 );
  CHECK( data_c[0] == 4.0 ); 
  CHECK( data_c[1] == 5.0 ); 
  CHECK( data_c[2] == 6.0 ); 

  ObjVectorData::data_reference data_c1 = p_model_c( 1 );
  CHECK( data_c1[0] == 7.0 ); 
  CHECK( data_c1[1] == 8.0 ); 
  CHECK( data_c1[2] == 9.0 ); 

  CHECK_THROWS( p_model_c(3) );
  CHECK_THROWS ( p_model_c.at(3) );
}


TEST_CASE( "PsmrtsDataBuffer Slice / Deep Copy Test", "[datamodel][buffer][slice][copy]") {
    
  typedef psmrts::PsmrtsDataBuffer<int> ObjIndexData;

  const size_t n_data = 10;
  auto p_model  = ObjIndexData( n_data );
  auto p_copy = p_model.deep_copy();

  CHECK ( &p_model != &p_copy );
  CHECK ( p_model.at(0) == p_copy.at(0) );
  CHECK ( p_model.at(9) == p_copy.at(9) );
  
  auto p_slice = p_model.slice(2, 8);

  CHECK ( p_model.size() != p_slice.size() );
  CHECK ( p_model.at(2) == p_slice.at(0) );
  
}