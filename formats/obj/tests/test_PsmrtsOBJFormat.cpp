#include <psmrts_catch2_environment.hpp>

#include <PsmrtsOBJFormat.hpp>

#include <DskKernelModel.hpp>


// Test Default constructor for PsmrtsOBJFormat
TEST_CASE ( "OBJ FORMAT Asset Test - Default Constructor", "[format][obj][default]"){
    psmrts::PsmrtsOBJFormat t_loader;

    CHECK( t_loader.isValid()           == false );
    CHECK( t_loader.obj_source()        == "" );
    CHECK( t_loader.nVertexes()         == 0 );
    CHECK( t_loader.nShapes()           == 0 );
    CHECK( t_loader.shape_facet_count() == 0 );
    CHECK( t_loader.nMaterials()        == 0 );
}

TEST_CASE ( "OBJ FORMAT Asset Test - Basic Load/Init Tests", "[format][obj][shape][bennu]" ) {
    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    
    psmrts::PsmrtsOBJFormat t_loader( objfile );
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
    CHECK_THROWS( psmrts::PsmrtsOBJFormat( objfile ) ); 
}

// Test loading an OBJ from text
TEST_CASE ( "OBJ FORMAT Asset Test - Text OBJ Load", "[format][obj][shape][text]" ) {
    std::string objtext = "v     -0.1634276539482     -0.1634276539482      0.1634276539482\n\
                           v      0.2644314943232      0.1010038565354      0.0000000000000\n\
                           v      0.2644314943232     -0.1010038565354      0.0000000000000\n\
                           f         19          3          2\n\
                           f         12         19          2\n\
                           f         15         12          2\n";  

    tinyobj::ObjReader *t_obj = psmrts::PsmrtsOBJFormat::load_obj_string( objtext );
    REQUIRE( nullptr != t_obj );
    
    psmrts::PsmrtsOBJFormat t_loader;
    CHECK_NOTHROW( t_loader = psmrts::PsmrtsOBJFormat( t_obj, objtext ) );
    const bool DoNotThrowFlag = false;
    CHECK( t_loader.check_obj_errors( "*** PsmrtsOBJFormat::Bad String", DoNotThrowFlag ) );
    CHECK_NOTHROW( t_loader.check_obj_errors() );

    auto obj_indexes = t_loader.get_indexes();
    auto obj_vectors = t_loader.get_vectors(); 

    CHECK( obj_indexes.size() == 3 );
    CHECK( obj_vectors.size() == 3 );

    CHECK ( obj_indexes(0)[0] == 18 );
    CHECK ( obj_indexes(0)[1] == 2 );
    CHECK ( obj_indexes(0)[2] == 1 );

    CHECK ( obj_indexes(1)[0] == 11 );
    CHECK ( obj_indexes(1)[1] == 18 );
    CHECK ( obj_indexes(1)[2] == 1 );

    CHECK ( obj_indexes(2)[0] == 14 );
    CHECK ( obj_indexes(2)[1] == 11 );
    CHECK ( obj_indexes(2)[2] == 1 );

    CHECK ( obj_vectors(0)[0] == -0.1634276539482 );
    CHECK ( obj_vectors(0)[1] == -0.1634276539482 );
    CHECK ( obj_vectors(0)[2] ==  0.1634276539482 );

    CHECK ( obj_vectors(1)[0] ==  0.2644314943232 );
    CHECK ( obj_vectors(1)[1] ==  0.1010038565354 );
    CHECK ( obj_vectors(1)[2] ==  0.0000000000000 );

    CHECK ( obj_vectors(2)[0] ==  0.2644314943232 );
    CHECK ( obj_vectors(2)[1] == -0.1010038565354 );
    CHECK ( obj_vectors(2)[2] ==  0.0000000000000 );

    std::string objtext2 = "v     -1.0     -2.0     -3.0\n\
                           v      -1.0      0.0      1.0\n\
                           v       1.0      2.0      3.0\n\
                           v       2.0      3.0      4.0\n\
                           v      -3.0      0.0      2.0\n\
                           f         1        2        3\n\
                           f         1        5        2\n\
                           f         2        3        1\n\
                           f         4        3        1\n\
                           f         5        4        2\n\
                           f         2        3        4 \n"; 
    tinyobj::ObjReader *t_obj2 = psmrts::PsmrtsOBJFormat::load_obj_string( objtext2 );
    REQUIRE( nullptr != t_obj2 );
    
    psmrts::PsmrtsOBJFormat t_loader2;
    CHECK_NOTHROW( t_loader2 = psmrts::PsmrtsOBJFormat( t_obj2, objtext2 ) );
    const bool DoNotThrowFlag2 = false;
    CHECK( t_loader2.check_obj_errors( "*** PsmrtsOBJFormat::Bad String", DoNotThrowFlag2 ) );
    CHECK_NOTHROW( t_loader2.check_obj_errors() );

    auto obj_indexes2 = t_loader2.get_indexes();
    auto obj_vectors2 = t_loader2.get_vectors(); 

    CHECK( obj_indexes2.size() == 6 );
    CHECK( obj_vectors2.size() == 5 );

    CHECK ( obj_indexes2(0)[0] == 0 );
    CHECK ( obj_indexes2(0)[1] == 1 );
    CHECK ( obj_indexes2(0)[2] == 2 );

    CHECK ( obj_indexes2(1)[0] == 0 );
    CHECK ( obj_indexes2(1)[1] == 4 );
    CHECK ( obj_indexes2(1)[2] == 1 );

    CHECK ( obj_indexes2(2)[0] == 1 );
    CHECK ( obj_indexes2(2)[1] == 2 );
    CHECK ( obj_indexes2(2)[2] == 0 );
    
    CHECK ( obj_indexes2(3)[0] == 3 );
    CHECK ( obj_indexes2(3)[1] == 2 );
    CHECK ( obj_indexes2(3)[2] == 0 );

    CHECK ( obj_indexes2(4)[0] == 4 );
    CHECK ( obj_indexes2(4)[1] == 3 );
    CHECK ( obj_indexes2(4)[2] == 1 );

    CHECK ( obj_indexes2(5)[0] == 1 );
    CHECK ( obj_indexes2(5)[1] == 2 );
    CHECK ( obj_indexes2(5)[2] == 3 );

    CHECK ( obj_vectors2(0)[0] == -1.0 );
    CHECK ( obj_vectors2(0)[1] == -2.0 );
    CHECK ( obj_vectors2(0)[2] == -3.0 );

    CHECK ( obj_vectors2(1)[0] == -1.0 );
    CHECK ( obj_vectors2(1)[1] ==  0.0 );
    CHECK ( obj_vectors2(1)[2] ==  1.0 );

    CHECK ( obj_vectors2(2)[0] == 1.0 );
    CHECK ( obj_vectors2(2)[1] == 2.0 );
    CHECK ( obj_vectors2(2)[2] == 3.0 );

    CHECK ( obj_vectors2(3)[0] == 2.0 );
    CHECK ( obj_vectors2(3)[1] == 3.0 );
    CHECK ( obj_vectors2(3)[2] == 4.0 );

    CHECK ( obj_vectors2(4)[0] == -3.0 );
    CHECK ( obj_vectors2(4)[1] ==  0.0 );
    CHECK ( obj_vectors2(4)[2] ==  2.0 );

}


// Test export of get_indexes() and get_vector() match with contents of *shape().
TEST_CASE ( "OBJ FORMAT Asset Test - Data Export Tests", "[format][obj][shape][bennu][export]" ) {
    std::string file = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    
    psmrts::PsmrtsOBJFormat t_loader( file );
    CHECK_NOTHROW( t_loader.check_obj_errors() ); 
    CHECK( t_loader.isValid()           == true );

    auto obj_indexes = t_loader.get_indexes();
    auto obj_vectors = t_loader.get_vectors(); 

    // Index Comparisons
    CHECK ( obj_indexes(0)[0] == 18 );
    CHECK ( obj_indexes(0)[1] == 2 );
    CHECK ( obj_indexes(0)[2] == 1 );

    CHECK ( obj_indexes(17)[0] == 13 );
    CHECK ( obj_indexes(17)[1] == 6 );
    CHECK ( obj_indexes(17)[2] == 1 );
    
    CHECK ( obj_indexes(35)[0] == 18 );
    CHECK ( obj_indexes(35)[1] == 11 );
    CHECK ( obj_indexes(35)[2] == 10 );
    
    // Vector Comparisons
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

TEST_CASE ( "OBJ FORMAT Asset Test - OBJ / DSK Vector Comparison Test", "[format][obj][dsk][vectors]") {
    auto tolerance = 1.0e-6;

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds");
    
    psmrts::PsmrtsOBJFormat t_loader( objfile );
    naif::DskKernelModel dsk( dskfile );
    
    auto obj_floats = t_loader.get_float_vectors();
    auto dsk_vectors = dsk.load_facet_vectors();
    float sum_float = 0;
    for (int i = 0; i < obj_floats.size(); i++ ) {
        sum_float += fabs(dsk_vectors(i)[0] - double(obj_floats(i)[0]) );
        sum_float += fabs(dsk_vectors(i)[1] - double(obj_floats(i)[1]) );
        sum_float += fabs(dsk_vectors(i)[2] - double(obj_floats(i)[2]) );
    }

    // fails at tolerances > 1.0e-6
    CHECK_THAT ( sum_float, Catch::Matchers::WithinAbs(0.0, tolerance) );

    auto obj_double = t_loader.get_vectors();
    double sum_double = 0;
    for (int i = 0; i < obj_double.size(); i++ ) {
        sum_double += fabs(dsk_vectors(i)[0] - obj_double(i)[0] );
        sum_double += fabs(dsk_vectors(i)[1] - obj_double(i)[1] ); 
        sum_double += fabs(dsk_vectors(i)[2] - obj_double(i)[2] );
    }

    CHECK_THAT ( sum_double, Catch::Matchers::WithinAbs(0.0, tolerance) );
}


#if 0
TEST_CASE ( "OBJ FORMAT Asset Test - Huge OBJ Tests", "[format][obj][shape][hugeone]" ) {
    //std::string objfile = "/opt/isis3/data/osirisrex/kernels/dsk/l_00050mm_alt_ptm_5595n04217_v020.obj";
    std::string objfile = "watermarkRedTest1.obj";

    psmrts::PsmrtsOBJFormat t_loader( objfile );
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