#include <psmrts_catch2_environment.hpp>

#include <PsmrtsDSKFormat.hpp>


TEST_CASE( "DSK FORMAT Base Test", "[format][dsk][default]") {
    std::string dsk_file = psmrts_formats_path( "dsk/data/bennu_20facets.bds" );

    psmrts::PsmrtsDSKFormat d_loader( dsk_file );

    CHECK( d_loader.get_mesh().isValid() == true );
    CHECK( d_loader.get_mesh().nvectors() == 20 );
}
