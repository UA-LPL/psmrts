#include <psmrts_catch2_environment.hpp>

#include <PsmrtsPLYFormat.hpp>
#include <PsmrtsOBJFormat.hpp>

#include <DskKernelModel.hpp>

// Test Default (No / Bad file) Constructor Cases
TEST_CASE( "PLY FORMAT Asset Test - No File Default Constructor", "[format][ply]") {
    std::string no_file = psmrts_formats_path("ply/data/bad_path.ply");
    psmrts::PsmrtsPLYFormat ply;
    CHECK( ply.ply_source()          == "" );
    CHECK( ply.format_model_source() == "" );
    CHECK( ply.isValid()             == false );
    CHECK( ply.nVertexes()           == 0 );
    CHECK( ply.nIndexes()            == 0 );
   
    CHECK_THROWS( ply.open( no_file ) );
    CHECK_THROWS( ply.load_ply_file( no_file ) ); 

    CHECK( ply.get_mesh().isValid()        == false ); 
    CHECK( ply.get_float_vectors().size()  == 0 );
    CHECK( ply.get_double_vectors().size() == 0 );
    CHECK( ply.get_indexes().size()        == 0 );
}

// Test Actual File Constructor for PsmrtsPLYFormat - Using Bennu PLY data
TEST_CASE ( "PLY FORMAT Asset Test - Default Constructor", "[format][ply][default]") {
    auto tolerance = 1.0e-6;

    std::string plyfile = psmrts_formats_path("ply/data/Bennu_Radar.ply");
    psmrts::PsmrtsPLYFormat ply( plyfile );

    CHECK( ply.ply_source() == plyfile );
    CHECK( ply.isValid()    == true    );
    CHECK( ply.nVertexes()  == 1348    );
    CHECK( ply.nIndexes()   == 2692    );

    CHECK( ply.get_mesh().isValid()            == true  );
    CHECK( ply.get_mesh().isVectorDouble()     == true  );
    CHECK( ply.get_mesh().vectors().isDouble() == true  );
    CHECK( ply.get_mesh().vectors().isFloat()  == false );

    CHECK( ply.get_double_vectors().size()     == 1348 );
    CHECK( ply.get_float_vectors().size()      == 1348 );
    CHECK( ply.get_indexes().size()            == 2692 );

    // Conversion Check
    psmrts::PsmrtsVector3f ply_floats  = ply.get_float_vectors();
    psmrts::PsmrtsVector3d ply_doubles = ply.get_double_vectors();

    for (int i = 0; i < ply_doubles.size(); i++) {
        CHECK_THAT( ply_doubles(i)[0], Catch::Matchers::WithinAbs(ply_floats(i)[0], tolerance));
        CHECK_THAT( ply_doubles(i)[1], Catch::Matchers::WithinAbs(ply_floats(i)[1], tolerance));
        CHECK_THAT( ply_doubles(i)[2], Catch::Matchers::WithinAbs(ply_floats(i)[2], tolerance));
    }
    
    CHECK_NOTHROW( ply.elapsed_life_time_s()              >= 0 );
    CHECK_NOTHROW( ply.track_count()                      == 0 );
    CHECK_NOTHROW( ply.performance_snapshot().runtime_s() >= 0 );

    std::shared_ptr<miniply::PLYReader> ply_read( psmrts::PsmrtsPLYFormat::open( plyfile ) );
    REQUIRE( ply_read != nullptr );
    
}

// Need to fix ply converted to obj? Or compare to values that are already in the obj?
#if 0
TEST_CASE( "PLY FORMAT Asset Test - Vector Data Value Test" ) {
    auto tolerance = 1.0e-6;

    std::string plyfile = psmrts_formats_path( "ply/data/Bennu_Radar.ply" );
    psmrts::PsmrtsPLYFormat ply_data_loader( plyfile );
    
    std::string objfile = psmrts_formats_path( "obj/data/Bennu_Radar.obj" );
    psmrts::PsmrtsOBJFormat obj_data_loader( objfile );

    auto obj_floats = obj_data_loader.get_float_vectors();
    float sum_float = 0;
    for (int j = 0; j < obj_floats.size(); j++) {
        sum_float += fabs(ply_floats(j)[0] - obj_floats(j)[0]);
        sum_float += fabs(ply_floats(j)[1] - obj_floats(j)[1]);
        sum_float += fabs(ply_floats(j)[2] - obj_floats(j)[2]);
    }

    CHECK_THAT( sum_float, Catch::Matchers::WithinAbs(0.0, tolerance));

    auto obj_doubles = obj_data_loader.get_double_vectors();
    double sum_double = 0;
    for (int k=0; k< obj_doubles.size(); k++) {
        sum_double += fabs(ply_doubles(k)[0] - obj_doubles(k)[0]);
        sum_double += fabs(ply_doubles(k)[1] - obj_doubles(k)[1]);
        sum_double += fabs(ply_doubles(k)[2] - obj_doubles(k)[2]);
    }

    CHECK_THAT( sum_double, Catch::Matchers::WithinAbs(0.0, tolerance));
    
}
#endif

TEST_CASE( "PLY FORMAT Asset Test - txt to ply Comparison Check", "[format][ply][txt][bennu]") {
    std::string txt_path = psmrts_formats_path("ply/data/icosahedron.ply");
    std::string ply_path = psmrts_formats_path("ply/data/icosahedron_binary.ply");

    psmrts::PsmrtsPLYFormat txt_loader( txt_path );
    psmrts::PsmrtsPLYFormat ply_loader( ply_path );

    CHECK( ply_loader.format_model_source() != txt_loader.format_model_source() );
    CHECK( ply_loader.ply_source()          != txt_loader.ply_source() );
    CHECK( ply_loader.isValid()             == true );
    CHECK( txt_loader.isValid()             == true );
    CHECK( ply_loader.nVertexes()           == txt_loader.nVertexes() ); // 1348
    CHECK( ply_loader.nIndexes()            == txt_loader.nIndexes() ); // 2692
}

// Move to Above
TEST_CASE( "PLY FORMAT Asset Test - txt Based PLY Check", "[format][ply][txt]") {
    std::string txt_file = psmrts_formats_path("ply/data/teapot.ply");

    psmrts::PsmrtsPLYFormat txt_loader( txt_file );

    CHECK( txt_loader.format_model_source() == txt_file );
    CHECK( txt_loader.ply_source()          == txt_file );
    CHECK( txt_loader.isValid()             == true );
    CHECK( txt_loader.nVertexes()           == 1177 );
    CHECK( txt_loader.nIndexes()            == 2256 );


}
