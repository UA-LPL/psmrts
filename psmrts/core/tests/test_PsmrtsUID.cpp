#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUID.hpp>

TEST_CASE( "PSMRTS UID Test", "[product][uid]" ) {

  CHECK( psmrts::PsmrtsUID::null_uid() == 0 );
  
  auto uid = psmrts::PsmrtsUID::get_uid();
  CHECK( psmrts::PsmrtsUID::is_valid_uid( uid ) == true );

  auto uid_s = psmrts::PsmrtsUID::to_string( uid );
  CHECK(uid_s == std::to_string( uid ) );

  CHECK( psmrts::PsmrtsUID::from_string( uid_s ) == uid );

  CHECK( psmrts::PsmrtsUID::get_uid() != uid );
  CHECK( psmrts::PsmrtsUID::is_valid_uid( psmrts::PsmrtsUID::get_uid() ) == true );
  CHECK( psmrts::PsmrtsUID::get_uid() != psmrts::PsmrtsUID::null_uid() );

}
