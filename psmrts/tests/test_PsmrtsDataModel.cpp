#define CATCH_CONFIG_MAIN
#include <psmrts_catch2_environment.hpp>

#define PSMRTS_BOUNDS_CHECK 1
#include <PsmrtsDataModel.hpp>

TEST_CASE( "PsmrtsDataModel Default Test", "[datamodel][buffer][default]") {

  psmrts::PsmrtsDataModel p_model  = psmrts::PsmrtsDataModel();

  CHECK( p_model.size()            == 0 );
  CHECK( p_model.total_allocated() == 0 );

  CHECK( p_model.data_size()       == 3 );

  CHECK( p_model.scalar_size()     == 8 );
  CHECK( p_model.scalar_size()     == sizeof( double ) );
  CHECK( p_model.scalar_size()     == sizeof( psmrts::PsmrtsDataModel<>::Scalar ) );
  CHECK( p_model.scalar_size()     == sizeof( psmrts::PsmrtsDataModel<>::value_type) );

  CHECK_THROWS( p_model.at( 0 ) );
  CHECK_THROWS( p_model( 0 ) );
}

TEST_CASE( "PsmrtsDataModel Protected API Default Test", "[datamodel][buffer][protected]") {

  class PsmrtsTestDataBuffer : psmrts::PsmrtsDataModel<> {
    public:
      PsmrtsTestDataBuffer() { }
      ~PsmrtsTestDataBuffer() { }

      inline void validate_t( const size_t index ) const {
        psmrts::PsmrtsDataModel<>::validate( index );
      }

      inline size_t data_index_t( const size_t index ) const {
        return ( psmrts::PsmrtsDataModel<>::validate( index ) );
      }

      inline value_type *data_t( const size_t index ) const {
        return ( psmrts::PsmrtsDataModel<>::data( index ) );
      }
      inline value_type *data_t( const size_t index ) {
        return ( psmrts::PsmrtsDataModel<>::data( index ) );
      }

      inline void init() {
        psmrts::PsmrtsDataModel<>::init();
      }

      inline void allocate_t( const size_t n_data ) {
        psmrts::PsmrtsDataModel<>::allocate( n_data );
      }

  };


  psmrts::PsmrtsTestDataBuffer p_model_t = psmrts::PsmrtsTestDataBuffer();

  CHECK( p_model_t.size()            == 0 );
  CHECK( p_model_t.total_allocated() == 0 );

  CHECK( p_model_t.data_size()       == 3 );

  CHECK( p_model_t.scalar_size()     == 8 );
  CHECK( p_model_t.scalar_size()     == sizeof( double ) );
  CHECK( p_model_t.scalar_size()     == sizeof( psmrts::PsmrtsTestDataBuffer::psmrts::PsmrtsDataModel<>::Scalar ) );
  CHECK( p_model_t.scalar_size()     == sizeof( psmrts::PsmrtsTestDataBuffer::psmrts::PsmrtsDataModel<>::value_type) );

  CHECK_THROWS( p_model_t.at( 0 ) );
  CHECK_THROWS( p_model_t( 0 ) );

  // Now check the protected API
  CHECK_THROWS( p_model_t.validate_t( 0 ) );
  CHECK_THROWS( p_model_t.data_index( 0 ) );
  CHECK_THROWS( p_model_t.data_t( 0 ) );
  CHECK_THROWS( p_model_t.data_t( 0 ) );

  // Lets allocate a small buffer
  size_t n_data  = 10;
  CHECK_NOTHROW( p_model_t.allocate( n_data ) );
  CHECK( p_model_t.size()            ==  n_data );
  CHECK( p_model_t.data_size()       == 3 );
  CHECK( p_model_t.total_allocated() == ( n_data * p_model_t.data_size() ) );

  CHECK_NOTHROW( p_model_t.at( 0 ) );
  CHECK_NOTHROW( p_model_t( 0 ) );
  CHECK_NOTHROW( p_model_t( n_data - 1 ) );
  CHECK_THROWS( p_model_t( n_data ) );
}

TEST_CASE( "PsmrtsDataModel (unsigned char) Byte Test", "[datamodel][buffer][byte]") {

  typedef Eigen::Vector<unsigned char, 3>   UCharType;

  const size_t n_data = 100;
  auto p_model  = psmrts::PsmrtsDataModel<UCharType>( n_data );

  CHECK( p_model.size()            == n_data );
  CHECK( p_model.data_size()       == 3 );
  CHECK( p_model.total_allocated() == (  n_data * p_model_t.data_size() )) );


  CHECK( p_model.scalar_size()     == 1 );
  CHECK( p_model.scalar_size()     == sizeof( unsigned char ) );
  CHECK( p_model.scalar_size()     == sizeof( UCharType::Scalar ) );
  CHECK( p_model.scalar_size()     == sizeof( UCharType::value_type ) );
  CHECK( p_model.scalar_size()     == sizeof( psmrts::PsmrtsDataModel<UCharType>::Scalar ) );
  CHECK( p_model.scalar_size()     == sizeof( psmrts::PsmrtsDataModel<UCharType>::value_type) );

  CHECK_NOTHROW( p_model.at( 0 ) );
  CHECK_NOTHROW( p_model( 0 ) );
  CHECK_NOTHROW( p_model( n_data - 1 ) );

}