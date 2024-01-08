#define CATCH_CONFIG_MAIN
#include <psmrts_catch2_environment.hpp>

#define PSMRTS_BOUNDS_CHECK 1
#include <PsmrtsDataModel.hpp>

TEST_CASE( "PsmrtsDataModel Default Test", "[datamodel][buffer][default]") {

  psmrts::PsmrtsDataModel<> p_model  = psmrts::PsmrtsDataModel<>();

  CHECK( p_model.size()            == 0 );
  CHECK( p_model.total_allocated() == 0 );

  CHECK( p_model.data_size()       == 3 );

  CHECK( p_model.scalar_size()     == 8 );
  CHECK( p_model.scalar_size()     == sizeof( double ) );
  CHECK( p_model.scalar_size()     == sizeof( psmrts::PsmrtsDataModel<>::value_type) );

  CHECK_THROWS( p_model.at( 0 ) );
  CHECK_THROWS( p_model( 0 ) );
}

TEST_CASE( "PsmrtsDataModel Protected API Default Test", "[datamodel][buffer][protected]") {

  /** Create derived class to tests protected methods */
  class PsmrtsTestDataBuffer : public psmrts::PsmrtsDataModel<> {
    public:
      typedef PsmrtsDataModel<>::data_type       data_type;
      typedef PsmrtsDataModel<>::value_type      value_type;

      PsmrtsTestDataBuffer() { }
      ~PsmrtsTestDataBuffer() { }

      inline void validate_t( const int index ) const {
        this->validate( index );
      }

      inline int data_index_t( const int index ) const {
        return ( this->data_index( index ) );
      }

      inline const value_type *get_data_t( const int index ) const {
        return ( this->get_data_ref( index ) );
      }
      inline value_type *get_data_t( const int index ) {
        return ( this->get_data_ref( index ) );
      }

      inline void init_t() {
        this->init();
      }

      inline void allocate_t( const size_t n_data ) {
        this->allocate( n_data );
      }

  };


  //  Get an instance of the derived class for testing
  PsmrtsTestDataBuffer p_model_t = PsmrtsTestDataBuffer();

  CHECK( p_model_t.size()            == 0 );
  CHECK( p_model_t.total_allocated() == 0 );

  CHECK( p_model_t.data_size()       == 3 );

  CHECK( p_model_t.scalar_size()     == 8 );
  CHECK( p_model_t.scalar_size()     == sizeof( double ) );
  CHECK( p_model_t.scalar_size()     == sizeof( PsmrtsTestDataBuffer::value_type) );

  CHECK_THROWS( p_model_t.at( 0 ) );
  CHECK_THROWS( p_model_t( 0 ) );

  // Now check the protected API
  CHECK_THROWS( p_model_t.validate_t( 0 ) );
  CHECK_THROWS( p_model_t.get_data_t( 0 ) );
  CHECK_THROWS( p_model_t.get_data_t( 0 ) );

  // Lets allocate a small buffer
  size_t n_data  = 10;
  CHECK_NOTHROW( p_model_t.allocate_t( n_data ) );
  CHECK( p_model_t.size()            ==  n_data );
  CHECK( p_model_t.data_size()       == 3 );
  CHECK( p_model_t.total_allocated() == ( n_data * p_model_t.data_size() ) );

  CHECK_NOTHROW( p_model_t.at( 0 ) );
  CHECK_NOTHROW( p_model_t( 0 ) );
  CHECK_NOTHROW( p_model_t( n_data - 1 ) );
  CHECK_THROWS( p_model_t( n_data ) );

  CHECK_NOTHROW( p_model_t.init_t() );
  CHECK( p_model_t.size()            == 0 );
  CHECK( p_model_t.total_allocated() == 0 );

}

TEST_CASE( "PsmrtsDataModel (double) Double Test", "[datamodel][buffer][double]") {

  typedef psmrts::PsmrtsDataModel<Eigen::Vector3d> ObjVectorData;

  const size_t n_data = 100;
  auto p_model  = ObjVectorData( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.data_size()       == 3 );
  CHECK( p_model.total_allocated() == (  n_data * p_model.data_size() ) );


  CHECK( p_model.scalar_size()     == 8 );
  CHECK( p_model.scalar_size()     == sizeof( double ) );
  CHECK( p_model.scalar_size()     == sizeof( ObjVectorData::value_type) );

  CHECK_NOTHROW( p_model.at( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}

TEST_CASE( "PsmrtsDataModel (int) Integer Test", "[datamodel][buffer][integer]") {

  typedef psmrts::PsmrtsDataModel<Eigen::Vector3i> ObjIndexData;

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

TEST_CASE( "PsmrtsDataModel (unsigned char) Byte Test", "[datamodel][buffer][byte]") {

  typedef Eigen::Vector<unsigned char, 3>   UCharType;

  const size_t n_data = 100;
  auto p_model  = psmrts::PsmrtsDataModel<UCharType>( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.data_size()       == 3 );
  CHECK( p_model.total_allocated() == (  n_data * p_model.data_size() ) );


  CHECK( p_model.scalar_size()     == 1 );
  CHECK( p_model.scalar_size()     == sizeof( unsigned char ) );
  CHECK( p_model.scalar_size()     == sizeof( UCharType::value_type ) );
  CHECK( p_model.scalar_size()     == sizeof( psmrts::PsmrtsDataModel<UCharType>::value_type) );

  CHECK_NOTHROW( p_model.at( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}

TEST_CASE( "PsmrtsDataModel (double) Data Values Test", "[datamodel][buffer][double][values]") {

  typedef psmrts::PsmrtsDataModel<Eigen::Vector3d> ObjVectorData;

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
  
  ObjVectorData::data_reference data_1 = p_model( 1 ); 
  CHECK( data_1[0] == 4.0 ); 
  CHECK( data_1[1] == 5.0 ); 
  CHECK( data_1[2] == 6.0 ); 

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
}
