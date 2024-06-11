
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsOBJAsset.hpp>

#include <DskKernelModel.hpp>


// Test Default constructor for PsmrtsOBJAsset
TEST_CASE ( "OBJ FORMAT Asset Test - Default Constructor", "[format][obj][default]"){
    psmrts::PsmrtsOBJAsset t_loader;

    CHECK( t_loader.isValid()           == false );
    CHECK( t_loader.obj_source()        == "" );
    CHECK( t_loader.nVertexes()         == 0 );
    CHECK( t_loader.nShapes()           == 0 );
    CHECK( t_loader.shape_facet_count() == 0 );
    CHECK( t_loader.nMaterials()        == 0 );
}

TEST_CASE ( "OBJ FORMAT Asset Test - Basic Load/Init Tests", "[format][obj][shape][bennu]" ) {
    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    
    psmrts::PsmrtsOBJAsset t_loader( objfile );
    CHECK_NOTHROW( t_loader.check_obj_errors() ); 
    CHECK( t_loader.isValid()           == true );

    CHECK( t_loader.obj_source()        == objfile );
    CHECK( t_loader.nShapes()           == 1 );
    CHECK( t_loader.nVertexes()         == 20 );
    CHECK( t_loader.shape_facet_count() == 36 );
    CHECK( t_loader.nMaterials()        == 0 );
    CHECK( t_loader.shape() != nullptr );
    CHECK( t_loader.config().mtl_search_path == "" );
}

TEST_CASE ( "OBJ FORMAT Asset Test - Load Fail Tests", "[format][obj][shape][failures]" ) {
    std::string objfile = psmrts_formats_path( "obj/data/NOT_FOUND.obj" );
    CHECK_THROWS( psmrts::PsmrtsOBJAsset( objfile ) ); 
}

// Test loading an OBJ from text
TEST_CASE ( "OBJ FORMAT Asset Test - Text OBJ Load", "[format][obj][shape][text]" ) {
    std::string objtext = " __ OBJ___ ";  // Just cut and paste from small object, server facets or even one!

    tinyobj::ObjReader *t_obj = psmrts::PsmrtsOBJAsset::load_obj_string( objtext );
    REQUIRE( nullptr != t_obj );
    
    psmrts::PsmrtsOBJAsset t_loader;
    CHECK_NOTHROW( t_loader = psmrts::PsmrtsOBJAsset( t_obj, objtext ) );
    const bool DoNotThrowFlag = false;
    CHECK( t_loader.check_obj_errors( "*** PsmrtsOBJAsset::Bad String", DoNotThrowFlag ) );
    CHECK_NOTHROW( t_loader.check_obj_errors() );

    // Test contents with string objtext contents. Use get_indexes() and get_vectors().
    const bool NotImplemented = true;
    //REQUIRE( false == NotImplemented );  // Remove when implemented

    auto obj_indexes = t_loader.get_indexes<int>();
    auto obj_vectors = t_loader.get_vectors<double>(); 

    CHECK( obj_indexes.size() == 0 );
    CHECK( obj_vectors.size() == 0 );


}


// Test export of get_indexes() and get_vector() match with contents of *shape().
TEST_CASE ( "OBJ FORMAT Asset Test - Data Export Tests", "[format][obj][shape][bennu][export]" ) {
    std::string file = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    
    psmrts::PsmrtsOBJAsset t_loader( file );
    CHECK_NOTHROW( t_loader.check_obj_errors() ); 
    CHECK( t_loader.isValid()           == true );

    // Test content of get_indexes() and get_vectors() directly with sources in *shape().
    const bool NotImplemented = true;
    // REQUIRE( false == NotImplemented );  // Remove when implemented

    auto obj_indexes = t_loader.get_indexes<int>();
    auto obj_vectors = t_loader.get_vectors<double>(); 

    CHECK ( obj_indexes(0)[0] == 18 );
    CHECK ( obj_indexes(0)[1] == 2 );
    CHECK ( obj_indexes(0)[2] == 1 );

    CHECK ( obj_indexes(17)[0] == 13 );
    CHECK ( obj_indexes(17)[1] == 6 );
    CHECK ( obj_indexes(17)[2] == 1 );
    
    CHECK ( obj_indexes(35)[0] == 18 );
    CHECK ( obj_indexes(35)[1] == 11 );
    CHECK ( obj_indexes(35)[2] == 10 );
    
    CHECK ( obj_vectors(0)[0] == -0.1634276539482 );
    CHECK ( obj_vectors(0)[1] == -0.1634276539482 );
    CHECK ( obj_vectors(0)[2] ==  0.1634276539482 );

    CHECK ( obj_vectors(9)[0] == 0.0 );
    CHECK ( obj_vectors(9)[1] == -0.2644314943232 );
    CHECK ( obj_vectors(9)[2] == -0.1010038565354 );

    CHECK ( obj_vectors(19)[0] == -0.1634276539482 );
    CHECK ( obj_vectors(19)[1] == -0.1634276539482 );
    CHECK ( obj_vectors(19)[2] == -0.1634276539482 );
}


#if 0
TEST_CASE ( "OBJ FORMAT Asset Test - Huge OBJ Tests", "[format][obj][shape][hugeone]" ) {
    //std::string objfile = "/opt/isis3/data/osirisrex/kernels/dsk/l_00050mm_alt_ptm_5595n04217_v020.obj";
    std::string objfile = "watermarkRedTest1.obj";

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