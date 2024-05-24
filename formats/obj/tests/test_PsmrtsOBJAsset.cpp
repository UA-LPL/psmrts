
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsOBJAsset.hpp>

// Test Default constructor for PsmrtsOBJAsset
TEST_CASE ( "OBJ FORMAT Asset Test - Default Constructor", "[obj][default]"){
    psmrts::PsmrtsOBJAsset t_loader;

    CHECK( t_loader.isValid()           == false );
    CHECK( t_loader.obj_source()        == "" );
    CHECK( t_loader.nVertexes()         == 0 );
    CHECK( t_loader.nShapes()           == 0 );
    CHECK( t_loader.shape_facet_count() == 0 );
    CHECK( t_loader.nMaterials()        == 0 );
}

TEST_CASE ( "OBJ FORMAT Asset Test - Basic Load/Init Tests", "[obj][shape][bennu]" ) {
    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    
    psmrts::PsmrtsOBJAsset t_loader( objfile );
    CHECK_NOTHROW( t_loader.check_obj_errors() ); 
    CHECK( t_loader.isValid()           == true );

    CHECK( t_loader.obj_source()        == objfile );
    CHECK( t_loader.nShapes()           == 1 );
    CHECK( t_loader.nVertexes()         == 20 );
    CHECK( t_loader.shape_facet_count() == 36 );
    CHECK( t_loader.nMaterials()        == 0 );
}

TEST_CASE ( "OBJ FORMAT Asset Test - Load Fail Tests", "[obj][shape][failures]" ) {
    std::string objfile = psmrts_formats_path( "obj/data/NOT_FOUND.obj" );
    CHECK_THROWS( psmrts::PsmrtsOBJAsset( objfile ) ); 
}

#if 0
TEST_CASE ( "OBJ FORMAT Asset Test - Huge OBJ Tests", "[obj][shape][hugeone]" ) {
    std::string objfile = "/opt/isis3/data/osirisrex/kernels/dsk/l_00050mm_alt_ptm_5595n04217_v020.obj";

    psmrts::PsmrtsOBJAsset t_loader( objfile );
    CHECK_NOTHROW( t_loader.check_obj_errors() ); 
    CHECK( t_loader.isValid()           == true );

    CHECK( t_loader.obj_source()        == objfile );
    CHECK( t_loader.nShapes()           == 1 );
    CHECK( t_loader.nVertexes()         == 2148456 );
    CHECK( t_loader.nIndexes()          == 4289659 );
    CHECK( t_loader.shape_facet_count() == 4289659 );
    CHECK( t_loader.nMaterials()        == 0 );
}
#endif