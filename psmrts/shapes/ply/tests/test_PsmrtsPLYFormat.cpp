#include <psmrts/core/tests/psmrts_catch2_environment.hpp>
#include <psmrts/shapes/ply/PlyShape.hpp>
#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>

// Test Default (No / Bad file) Constructor Cases
TEST_CASE( "PLY FORMAT Asset Test - No File Default Constructor", "[format][ply]") {
    std::string no_file = psmrts_shapes_path("ply/data/bad_path.ply");
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
    auto tolerance = 1.0e-12;

    std::string plyfile = psmrts_shapes_path("ply/data/Bennu_Radar.ply");
    psmrts::PsmrtsPLYFormat ply( plyfile );
    
    ordered_json test;
    test["name"] = "ply";
    test["type"] = "mesh";
    test["ply_file"] = plyfile;
    test["ply_file_type"] = "binary";
    test["ply_data_type"] = "float";

    /**
    CHECK( ply.compare( test ) == true );
    std::string file_path = ply.config()["header"]["file"];

    CHECK( psmrts::psmrts_file_basename(file_path) == "Bennu_Radar.ply" );
    CHECK( file_path != "Bennu_Radar.ply" );

    nlohmann::ordered_json j_result = nlohmann::ordered_json::object();

    j_result["header"]["file"]      = ply.ply_source();
    j_result["header"]["type"]      = ply.file_type();
    j_result["header"]["nElements"] = 2;
    
    nlohmann::ordered_json j_elements = nlohmann::ordered_json::array();

    nlohmann::ordered_json j_el1;
    j_el1["element"]["name"] = "vertex";
    j_el1["element"]["size"] = 1348;

    nlohmann::ordered_json j_proplist1 = nlohmann::ordered_json::array();

    nlohmann::ordered_json j_prop1;
    j_prop1["property"]["name"] = "x";
    j_prop1["property"]["type"] = "float";
    j_proplist1.push_back(j_prop1);

    nlohmann::ordered_json j_prop2;
    j_prop2["property"]["name"] = "y";
    j_prop2["property"]["type"] = "float";
    j_proplist1.push_back(j_prop2);

    nlohmann::ordered_json j_prop3;
    j_prop3["property"]["name"] = "z";
    j_prop3["property"]["type"] = "float";
    j_proplist1.push_back(j_prop3);

    j_el1["properties"] = j_proplist1;

    nlohmann::ordered_json j_el2;
    j_el2["element"]["name"] = "face";
    j_el2["element"]["size"] = 2692;

    nlohmann::ordered_json j_proplist2 = nlohmann::ordered_json::array();

    nlohmann::ordered_json j_prop4;
    j_prop4["property"]["name"]  = "vertex_indices";
    j_prop4["property"]["type"]  = "int";
    j_prop4["property"]["count"] = "uchar";
    j_proplist2.push_back(j_prop4);

    j_el2["properties"] = j_proplist2;

    j_elements.push_back(j_el1);
    j_elements.push_back(j_el2);
    j_result["elements"]   = j_elements;
    nlohmann::json j_final = j_result;

    CHECK( nlohmann::ordered_json::diff( j_final, ply.config() ).empty() );
    */

    CHECK( ply.format_model_source() == plyfile  );
    CHECK( ply.ply_source()          == plyfile  );
    CHECK( ply.file_type()           == "binary" );
    CHECK( ply.isValid()             == true     );
    CHECK( ply.nVertexes()           == 1348     );
    CHECK( ply.nIndexes()            == 2692     );

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

    // Change to local variables
    CHECK_THAT( ply_doubles(0)[0], Catch::Matchers::WithinAbs( 0.0, tolerance ));                     // txt ply (via meshlab) value: 0.0
    CHECK_THAT( ply_doubles(0)[1], Catch::Matchers::WithinAbs( 0.0, tolerance ));                     // txt ply (via meshlab) value: 0.0
    CHECK_THAT( ply_doubles(0)[2], Catch::Matchers::WithinAbs( 0.25321400165557861, tolerance ));     // txt ply (via meshlab) value: 0.253214
   
    CHECK_THAT( ply_doubles(1347)[0], Catch::Matchers::WithinAbs( -0.05905099958181381, tolerance )); // txt ply (via meshlab) value: -0.059051
    CHECK_THAT( ply_doubles(1347)[1], Catch::Matchers::WithinAbs(  0.12654499709606171, tolerance )); // txt ply (via meshlab) value:  0.126545
    CHECK_THAT( ply_doubles(1347)[2], Catch::Matchers::WithinAbs( -0.18491800129413605, tolerance )); // txt ply (via meshlab) value: -0.184918

    CHECK_THROWS( ply_doubles(1348)[0] );
    CHECK_THROWS( ply_doubles(1348)[1] );
    CHECK_THROWS( ply_doubles(1348)[2] );

    psmrts::PsmrtsVector3i ply_indexes = ply.get_indexes();

    CHECK( ply_indexes(0)[0] == 0);       // txt ply (via meshlab) value: 0
    CHECK( ply_indexes(0)[1] == 1);       // txt ply (via meshlab) value: 1
    CHECK( ply_indexes(0)[2] == 2);       // txt ply (via meshlab) value: 2

    CHECK( ply_indexes(2691)[0] == 1301); // txt ply (via meshlab) value: 1301
    CHECK( ply_indexes(2691)[1] == 1347); // txt ply (via meshlab) value: 1347
    CHECK( ply_indexes(2691)[2] == 1270); // txt ply (via meshlab) value: 1270

    CHECK_THROWS( ply_indexes(2692)[0] );
    CHECK_THROWS( ply_indexes(2692)[1] );
    CHECK_THROWS( ply_indexes(2692)[2] );

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

/** 
TEST_CASE("PLY FORMAT Asset Test - Text Based Ply Reader and Comparison", "[format][ply][text]") {
    // Check to make sure binary ply version pulls data of basic shape, compare to text conversion
    // Original binary-base ply file was converted to text version via Meshlab: https://github.com/cnr-isti-vclab/meshlab
    auto tolerance = 1.0e-6;

    std::string binary_file = psmrts_shapes_path("ply/data/icosahedron_binary.ply");

    psmrts::PsmrtsPLYFormat binary_ply( binary_file );

    std::string file_path = binary_ply.config()["header"]["file"];
    
    CHECK( psmrts::psmrts_file_basename(file_path) == "icosahedron_binary.ply");
    CHECK( file_path != "icosahedron_binary.ply");

    nlohmann::ordered_json j_result = nlohmann::ordered_json::object();

    j_result["header"]["file"]      = binary_ply.ply_source();
    j_result["header"]["type"]      = binary_ply.file_type();
    j_result["header"]["nElements"] = 2;
    
    nlohmann::ordered_json j_elements = nlohmann::ordered_json::array();

    nlohmann::ordered_json j_el1;
    j_el1["element"]["name"] = "vertex";
    j_el1["element"]["size"] = 12;

    nlohmann::ordered_json j_proplist1 = nlohmann::ordered_json::array();

    nlohmann::ordered_json j_prop1;
    j_prop1["property"]["name"] = "x";
    j_prop1["property"]["type"] = "float";
    j_proplist1.push_back(j_prop1);

    nlohmann::ordered_json j_prop2;
    j_prop2["property"]["name"] = "y";
    j_prop2["property"]["type"] = "float";
    j_proplist1.push_back(j_prop2);

    nlohmann::ordered_json j_prop3;
    j_prop3["property"]["name"] = "z";
    j_prop3["property"]["type"] = "float";
    j_proplist1.push_back(j_prop3);

    j_el1["properties"] = j_proplist1;

    nlohmann::ordered_json j_el2;
    j_el2["element"]["name"] = "face";
    j_el2["element"]["size"] = 20;

    nlohmann::ordered_json j_proplist2 = nlohmann::ordered_json::array();

    nlohmann::ordered_json j_prop4;
    j_prop4["property"]["name"]  = "vertex_indices";
    j_prop4["property"]["type"]  = "int";
    j_prop4["property"]["count"] = "uchar";
    j_proplist2.push_back(j_prop4);

    j_el2["properties"] = j_proplist2;

    j_elements.push_back(j_el1);
    j_elements.push_back(j_el2);
    j_result["elements"]   = j_elements;
    nlohmann::json j_final = j_result;

    CHECK( nlohmann::ordered_json::diff( j_final, binary_ply.config() ).empty() );

    CHECK( binary_ply.ply_source() == binary_file );
    CHECK( binary_ply.file_type()  == "binary" );
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

    psmrts::PsmrtsVector3f binary_floats  = binary_ply.get_float_vectors();
    psmrts::PsmrtsVector3d binary_doubles = binary_ply.get_double_vectors();

    CHECK_THAT( binary_doubles(0)[0], Catch::Matchers::WithinAbs(  0.0, tolerance ));                 // txt ply (via meshlab) value:  0.0
    CHECK_THAT( binary_doubles(0)[1], Catch::Matchers::WithinAbs( -0.52573102712631226, tolerance )); // txt ply (via meshlab) value: -0.525731
    CHECK_THAT( binary_doubles(0)[2], Catch::Matchers::WithinAbs(  0.85065102577209473, tolerance )); // txt ply (via meshlab) value:  0.850651
    
    CHECK_THAT( binary_doubles(11)[0], Catch::Matchers::WithinAbs( 0.0, tolerance ));                 // txt ply (via meshlab) value: 0.0
    CHECK_THAT( binary_doubles(11)[1], Catch::Matchers::WithinAbs( 0.52573102712631226, tolerance )); // txt ply (via meshlab) value: 0.525731
    CHECK_THAT( binary_doubles(11)[2], Catch::Matchers::WithinAbs( 0.85065102577209473, tolerance )); // txt ply (via meshlab) value: 0.850651

    psmrts::PsmrtsVector3i binary_indexes = binary_ply.get_indexes();
    
    CHECK( binary_indexes(0)[0] == 6 ); // txt ply (via meshlab) value: 6
    CHECK( binary_indexes(0)[1] == 2 ); // txt ply (via meshlab) value: 2
    CHECK( binary_indexes(0)[2] == 1 ); // txt ply (via meshlab) value: 1

    // Create text converted ply version for comparison
    std::string text_file  = psmrts_shapes_path("ply/data/icosahedron.ply");

    psmrts::PsmrtsPLYFormat text_ply( text_file );

    CHECK( text_ply.config().dump() != binary_ply.config().dump() );

    std::string text_path = text_ply.config()["header"]["file"];
    CHECK( psmrts::psmrts_file_basename(text_path) == "icosahedron.ply" );
    CHECK( text_path != "icosahedron.ply");
    CHECK( text_ply.config()["header"]["type"].dump() == "\"ascii\"" );
    CHECK( text_ply.config()["header"]["nElements"].dump() == binary_ply.config()["header"]["nElements"].dump() );
    CHECK( text_ply.config()["elements"].dump() == binary_ply.config()["elements"].dump() );

    CHECK( text_ply.ply_source() == text_file );
    CHECK( text_ply.file_type()  == "ascii"   );
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

    psmrts::PsmrtsVector3f text_floats  = text_ply.get_float_vectors();
    psmrts::PsmrtsVector3d text_doubles = text_ply.get_double_vectors();

    CHECK_THAT( text_doubles(0)[0], Catch::Matchers::WithinAbs( binary_doubles(0)[0],   tolerance));
    CHECK_THAT( text_doubles(0)[1], Catch::Matchers::WithinAbs( binary_doubles(0)[1],   tolerance));
    CHECK_THAT( text_doubles(0)[2], Catch::Matchers::WithinAbs( binary_doubles(0)[2],   tolerance));

    CHECK_THAT( text_doubles(11)[0], Catch::Matchers::WithinAbs( binary_doubles(11)[0], tolerance));
    CHECK_THAT( text_doubles(11)[1], Catch::Matchers::WithinAbs( binary_doubles(11)[1], tolerance));
    CHECK_THAT( text_doubles(11)[2], Catch::Matchers::WithinAbs( binary_doubles(11)[2], tolerance));

    psmrts::PsmrtsVector3i text_indexes = text_ply.get_indexes();
    
    CHECK( text_indexes(0)[0] == binary_indexes(0)[0] ); 
    CHECK( text_indexes(0)[1] == binary_indexes(0)[1] );
    CHECK( text_indexes(0)[2] == binary_indexes(0)[2] );

    CHECK( text_indexes(19)[0] == binary_indexes(19)[0] ); 
    CHECK( text_indexes(19)[1] == binary_indexes(19)[1] );
    CHECK( text_indexes(19)[2] == binary_indexes(19)[2] );
}
*/

TEST_CASE( "PLY FORMAT Asset Test - OBJ Data Value Test", "[format][ply][obj]" ) {
    auto tolerance_f = 1.0e-6;
    auto tolerance_d = 1.0e-4;

    std::string plyfile = psmrts_shapes_path( "ply/data/Bennu_Radar.ply" );
    psmrts::PsmrtsPLYFormat ply_data_loader( plyfile );
    
    std::string objfile = psmrts_shapes_path( "ply/data/Bennu_Radar.obj" );
    psmrts::PsmrtsOBJFormat obj_data_loader( objfile );

    auto ply_floats = ply_data_loader.get_float_vectors();
    auto obj_floats = obj_data_loader.get_float_vectors();
    float sum_float = 0;
    for (int j = 0; j < obj_floats.size(); j++) {
        sum_float += fabs(ply_floats(j)[0] - obj_floats(j)[0]);
        sum_float += fabs(ply_floats(j)[1] - obj_floats(j)[1]);
        sum_float += fabs(ply_floats(j)[2] - obj_floats(j)[2]);
    }

    CHECK_THAT( sum_float, Catch::Matchers::WithinAbs(0.0, tolerance_f ) );

    auto ply_doubles  = ply_data_loader.get_double_vectors();
    auto obj_doubles  = obj_data_loader.get_double_vectors();
    double sum_double = 0;
    for (int k=0; k< obj_doubles.size(); k++) {
        sum_double += fabs(ply_doubles(k)[0] - obj_doubles(k)[0]);
        sum_double += fabs(ply_doubles(k)[1] - obj_doubles(k)[1]);
        sum_double += fabs(ply_doubles(k)[2] - obj_doubles(k)[2]);
    }

    // The PLY version is float and the OBJ is double
    CHECK_THAT( sum_double, Catch::Matchers::WithinAbs(0.0, tolerance_d ) );
}
