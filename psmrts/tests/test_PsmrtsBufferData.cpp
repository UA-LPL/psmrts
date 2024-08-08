
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsBufferData.hpp>

TEST_CASE( "PsmrtsBufferData Default Test", "[data][buffer][default]" ) {
    psmrts::PsmrtsBufferData pb_data;

    CHECK( pb_data.size() == 0 );
    CHECK( pb_data.isValid() == false );
    // CHECK( pb_data.data_get() == nullptr ); invalid index range

}