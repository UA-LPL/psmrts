#include <psmrts_catch2_environment.hpp>

#include <PsmrtsDSKFormat.hpp>

TEST_CASE( "DSK FORMAT Asset Test - Default Constructor", "[format][dsk][default]") {
    std::string no_file = psmrts_formats_path( "dsk/data/bad_path.bds" );
    psmrts::PsmrtsDSKFormat d_loader;

    CHECK( d_loader.isValid()             == false );
    CHECK( d_loader.dsk_source()          == "" );
    CHECK( d_loader.format_model_source() == "" );
    CHECK( d_loader.nVertexes()           == 0 );
    CHECK( d_loader.nIndexes()            == 0 );

    //CHECK_THROWS( d_loader.load_dsk_file( no_file ) );

    CHECK( d_loader.get_mesh().isValid()        == false );
    CHECK( d_loader.get_float_vectors().size()  == 0 );
    CHECK( d_loader.get_double_vectors().size() == 0 );
    CHECK( d_loader.get_indexes().size()        == 0 );

}


TEST_CASE( "DSK FORMAT Basic Load/Innit Test", "[format][dsk][shape][bennu]") {
    std::string dsk_file = psmrts_formats_path( "dsk/data/bennu_20facets.bds" );
    psmrts::PsmrtsDSKFormat d_loader( dsk_file );

    CHECK( d_loader.isValid()                   == true );
    CHECK( d_loader.dsk_source()                == psmrts_formats_path( "dsk/data/bennu_20facets.bds" ) );
    CHECK( d_loader.format_model_source()       == psmrts_formats_path( "dsk/data/bennu_20facets.bds" ) );
    CHECK( d_loader.nVertexes()                 == 20 );
    CHECK( d_loader.nIndexes()                  == 36 );
    CHECK( d_loader.get_mesh().isValid()        == true );
    CHECK( d_loader.get_float_vectors().size()  == 20 );
    CHECK( d_loader.get_double_vectors().size() == 20 );
    CHECK( d_loader.get_indexes().size()        == 36 );
}
