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

    //CHECK( ply.getString() == "" );
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

    CHECK( ply.get_double_vectors()(0)[0] == 0.0 );                     // txt ply (via meshlab) value: 0.0
    CHECK( ply.get_double_vectors()(0)[1] == 0.0 );                     // txt ply (via meshlab) value: 0.0
    CHECK( ply.get_double_vectors()(0)[2] == 0.25321400165557861 );     // txt ply (via meshlab) value: 0.253214

    CHECK( ply.get_double_vectors()(1347)[0] == -0.05905099958181381 ); // txt ply (via meshlab) value: -0.059051
    CHECK( ply.get_double_vectors()(1347)[1] ==  0.12654499709606171 ); // txt ply (via meshlab) value:  0.126545
    CHECK( ply.get_double_vectors()(1347)[2] == -0.18491800129413605 ); // txt ply (via meshlab) value: -0.184918

    CHECK_THROWS( ply.get_double_vectors()(1348)[0] );
    CHECK_THROWS( ply.get_double_vectors()(1348)[1] );
    CHECK_THROWS( ply.get_double_vectors()(1348)[2] );

    CHECK( ply.get_indexes()(0)[0] == 0);                               // txt ply (via meshlab) value: 0
    CHECK( ply.get_indexes()(0)[1] == 1);                               // txt ply (via meshlab) value: 1
    CHECK( ply.get_indexes()(0)[2] == 2);                               // txt ply (via meshlab) value: 2

    CHECK( ply.get_indexes()(2691)[0] == 1301);                         // txt ply (via meshlab) value: 1301
    CHECK( ply.get_indexes()(2691)[1] == 1347);                         // txt ply (via meshlab) value: 1347
    CHECK( ply.get_indexes()(2691)[2] == 1270);                         // txt ply (via meshlab) value: 1270

    CHECK_THROWS( ply.get_indexes()(2692)[0] );
    CHECK_THROWS( ply.get_indexes()(2692)[1] );
    CHECK_THROWS( ply.get_indexes()(2692)[2] );


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

TEST_CASE("PLY FORMAT Asset Test - Text Based Ply Reader and Comparison", "[format][ply][text]") {
    // Check to make sure binary ply version pulls data of basic shape, compare to text conversion
    // Original binary-base ply file was converted to text version via Meshlab: https://github.com/cnr-isti-vclab/meshlab

    std::string binary_file = psmrts_formats_path("ply/data/icosahedron_binary.ply");

    psmrts::PsmrtsPLYFormat binary_ply( binary_file );

    CHECK( binary_ply.ply_source() == binary_file );
    CHECK( binary_ply.isValid()    == true    );
    CHECK( binary_ply.nVertexes()  == 12    );
    CHECK( binary_ply.nIndexes()   == 20    );

    CHECK( binary_ply.get_mesh().isValid()            == true  );
    CHECK( binary_ply.get_mesh().isVectorDouble()     == true  );
    CHECK( binary_ply.get_mesh().vectors().isDouble() == true  );
    CHECK( binary_ply.get_mesh().vectors().isFloat()  == false );

    CHECK( binary_ply.get_double_vectors().size()     == 12 );
    CHECK( binary_ply.get_float_vectors().size()      == 12 );
    CHECK( binary_ply.get_indexes().size()            == 20 );

    CHECK( binary_ply.get_double_vectors()(0)[0] ==  0.0                 ); // txt ply (via meshlab) value:  0.0
    CHECK( binary_ply.get_double_vectors()(0)[1] == -0.52573102712631226 ); // txt ply (via meshlab) value: -0.525731
    CHECK( binary_ply.get_double_vectors()(0)[2] ==  0.85065102577209473 ); // txt ply (via meshlab) value:  0.850651

    CHECK( binary_ply.get_double_vectors()(11)[0] == 0.0                 ); // txt ply (via meshlab) value: 0.0
    CHECK( binary_ply.get_double_vectors()(11)[1] == 0.52573102712631226 ); // txt ply (via meshlab) value: 0.525731
    CHECK( binary_ply.get_double_vectors()(11)[2] == 0.85065102577209473 ); // txt ply (via meshlab) value: 0.850651
    
    CHECK( binary_ply.get_indexes()(0)[0] == 6 );                           // txt ply (via meshlab) value: 6
    CHECK( binary_ply.get_indexes()(0)[1] == 2 );                           // txt ply (via meshlab) value: 2
    CHECK( binary_ply.get_indexes()(0)[2] == 1 );                           // txt ply (via meshlab) value: 1

    // Create text converted ply version for comparison
    std::string text_file  = psmrts_formats_path("ply/data/icosahedron.ply");

    psmrts::PsmrtsPLYFormat text_ply( text_file );

    CHECK( text_ply.ply_source() == text_file );
    CHECK( text_ply.isValid()    == true      );
    CHECK( text_ply.nVertexes()  == 12        );
    CHECK( text_ply.nIndexes()   == 20        );

    CHECK( text_ply.get_mesh().isValid()            == true  );
    CHECK( text_ply.get_mesh().isVectorDouble()     == true  );
    CHECK( text_ply.get_mesh().vectors().isDouble() == true  );
    CHECK( text_ply.get_mesh().vectors().isFloat()  == false );

    CHECK( text_ply.get_double_vectors().size()     == 12 );
    CHECK( text_ply.get_float_vectors().size()      == 12 );
    CHECK( text_ply.get_indexes().size()            == 20 );

    CHECK( text_ply.get_double_vectors()(0)[0] == binary_ply.get_double_vectors()(0)[0] );                 
    CHECK( text_ply.get_double_vectors()(0)[1] == binary_ply.get_double_vectors()(0)[1] ); 
    CHECK( text_ply.get_double_vectors()(0)[2] == binary_ply.get_double_vectors()(0)[2] ); 

    CHECK( text_ply.get_double_vectors()(11)[0] == binary_ply.get_double_vectors()(11)[0] );                 
    CHECK( text_ply.get_double_vectors()(11)[1] == binary_ply.get_double_vectors()(11)[1] ); 
    CHECK( text_ply.get_double_vectors()(11)[2] == binary_ply.get_double_vectors()(11)[2] ); 
    
    CHECK( text_ply.get_indexes()(0)[0] == binary_ply.get_indexes()(0)[0] ); 
    CHECK( text_ply.get_indexes()(0)[1] == binary_ply.get_indexes()(0)[1] );
    CHECK( text_ply.get_indexes()(0)[2] == binary_ply.get_indexes()(0)[2] );
}

// Need to fix ply converted to obj? Or compare to values that are already in the obj?
#if 0
TEST_CASE( "PLY FORMAT Asset Test - OBJ Data Value Test", "[format][ply][obj]" ) {
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