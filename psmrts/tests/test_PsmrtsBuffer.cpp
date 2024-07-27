#include <psmrts_catch2_environment.hpp>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBuffer.hpp>


TEST_CASE( "PsmrtsBuffer Default Test", "[databuffer][default]") {

  psmrts::PsmrtsBuffer<double> p_model;
  CHECK( p_model.size()  == 0 );
  CHECK_THROWS( p_model( 0 ) );
}

TEST_CASE( "PsmrtsBuffer (double) Double Test", "[databuffer][double]") {

  typedef psmrts::PsmrtsBuffer<double> ObjVectorData;

  const size_t n_data = 100;
  auto p_model  = ObjVectorData( n_data );

  CHECK( p_model.size()   == n_data );

  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );


}

TEST_CASE( "PsmrtsBuffer (int) Integer Test", "[databuffer][integer]") {

  typedef psmrts::PsmrtsBuffer<int> ObjIndexData;

  const size_t n_data = 100;
  auto p_model  = ObjIndexData( n_data );

  CHECK( p_model.size()        == n_data );
  CHECK( p_model.volume_size() == (  n_data * p_model.stride_size() ) );

  CHECK( p_model.stride_size()     == 4 );
  CHECK( p_model.stride_size()     == sizeof( int ) );
  CHECK( p_model.stride_size()     == sizeof( ObjIndexData::value_type) );

  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}


TEST_CASE( "PsmrtsBuffer (float) Float Test", "[databuffer][float]") {

  typedef psmrts::PsmrtsBuffer<float> ObjIndexData;

  const size_t n_data = 100;
  auto p_model  = ObjIndexData( n_data );

  CHECK( p_model.size()        == n_data );
  CHECK( p_model.volume_size() == (  n_data * p_model.stride_size() ) );


  CHECK( p_model.stride_size()     == 4 );
  CHECK( p_model.stride_size()     == sizeof( float ) );
  CHECK( p_model.stride_size()     == sizeof( ObjIndexData::value_type) );

  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}

TEST_CASE( "PsmrtsBuffer (unsigned char) Byte Test", "[datamodel][buffer][byte]") {

  typedef unsigned char  UCharType;

  const size_t n_data = 100;
  auto p_model  = psmrts::PsmrtsBuffer<UCharType>( n_data );

  CHECK( p_model.size()        == n_data );
  CHECK( p_model.volume_size() == (  n_data * p_model.stride_size() ) );


  CHECK( p_model.stride_size() == 1 );
  CHECK( p_model.stride_size() == sizeof( unsigned char ) );
  CHECK( p_model.stride_size() == sizeof( UCharType) );
  CHECK( p_model.stride_size() == sizeof( psmrts::PsmrtsBuffer<UCharType>::value_type) );

  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}


TEST_CASE( "PsmrtsBuffer (double) Data Values Test", "[datamodel][buffer][double][values]") {

  typedef psmrts::PsmrtsBuffer<double> ObjVectorData;

  const size_t n_data = 10;
  auto p_model  = ObjVectorData( n_data );

  CHECK( p_model.size()        == n_data );
  CHECK( p_model.stride_size() == 8 );
  CHECK( p_model.volume_size() == (  n_data * p_model.stride_size() ) );

  double value = 1.0;
  for ( int n = 0 ; n < p_model.size() ; n++ ) {
    ObjVectorData::reference data_t = p_model( n ); 
    data_t = value++; 
  }

  ObjVectorData::value_type data_0 = p_model( 0 ); 
  CHECK( data_0 == 1.0 ); 
  
  ObjVectorData::value_type data_1 = p_model( 1 ); 
  CHECK( data_1 == 2.0 ); 
  
  ObjVectorData::value_type data_5 = p_model( 5 ); 
  CHECK( data_5 == 6.0 ); 

  ObjVectorData::value_type data_8 = p_model( 8 ); 
  CHECK( data_8 == 9.0 ); 

  ObjVectorData::value_type data_n = p_model( n_data - 1 ); 
  CHECK( data_n == 10.0 ); 

  auto p_copy = p_model.deep_copy();
  CHECK( p_model.size()        == p_copy.size() );
  CHECK( p_model.stride_size() == p_copy.stride_size() );
  CHECK( p_model.volume_size() == p_copy.volume_size() );

}


TEST_CASE( "PsmrtsBuffer Slice / Deep Copy Test", "[datamodel][buffer][slice][copy]") {
    
  typedef psmrts::PsmrtsBuffer<int> ObjIndexData;

  const size_t n_data = 10;
  auto p_model  = ObjIndexData( n_data );
  auto p_copy = p_model.deep_copy();

  CHECK ( p_model(0) == p_copy(0) );
  CHECK ( p_model(9) == p_copy(9) );
  
  auto p_slice = p_model.slice(1, 8);

  CHECK ( p_model.size() != p_slice.size() );
  CHECK ( p_model(1) == p_slice(0) );
  
}