#include <psmrts_catch2_environment.hpp>

#include <PsmrtsPLYFormat.hpp>
#include <PsmrtsOBJFormat.hpp>

#include <DskKernelModel.hpp>


TEST_CASE( "PLY FORMAT Asset Test - Default Load", "[format][ply]") {
    std::string plyfile = psmrts_formats_path("ply/data/Bennu_Radar.ply");
    psmrts::PsmrtsPLYFormat ply( plyfile );

    CHECK( ply.ply_source() == plyfile );
    CHECK( ply.isValid() == true );
    CHECK( ply.extract_info() == "0" );
    CHECK( ply.print_info() == "" );
}
// Test Default Constructor for PsmrtsPLYFormat
/** 
TEST_CASE ( "PLY FORMAT Asset Test - Default Constructor", "[format][ply][default]") {
    std::string plyfile = psmrts_formats_path("ply/data/Bennu_Radar.ply");
    psmrts::PsmrtsPLYFormat ply( plyfile );

    CHECK( ply.ply_source() == plyfile );
    CHECK( ply.isValid()    == true );
    CHECK( ply.n_elements() == 2 );
    CHECK( ply.nVertexes()  == 1348 );
    CHECK( ply.nIndexes()   == 2692 );

    psmrts::PsmrtsMeshData p_data = ply.get_mesh();

    CHECK ( p_data.nfacets()  == 2692 );
    CHECK ( p_data.nvectors() == 1348 );

    // Check some facet indexes
    CHECK( p_data.indexes()( 0 ) == Eigen::Vector3i( { 0, 1, 2 } ) );
}


TEST_CASE( "PLY FORMAT Asset Test - Default Constructor", "[format][ply][default]") {
    psmrts::PsmrtsPLYFormat ply_loader;

    CHECK( ply_loader.format_model_source() ==  ""   );
    CHECK( ply_loader.isValid()             == false );
    CHECK( ply_loader.ply_source()          == ""    );
    CHECK( ply_loader.n_elements()          == 0     );
    CHECK( ply_loader.nVertexes()           == 0     );
    CHECK( ply_loader.nIndexes()            == 0     );
    CHECK( ply_loader.print_file()          == "No File Allocated - Bad Print"    );
}

TEST_CASE( "PLY FORMAT Asset Test - Basic Load/Init Tests", "[format][ply][shape][bennu]") {
    std::string plyfile = psmrts_formats_path("ply/data/Bennu_Radar.ply");
    psmrts::PsmrtsPLYFormat ply_loader( plyfile );

    CHECK( ply_loader.format_model_source() == plyfile );
    CHECK( ply_loader.ply_source()          == plyfile );
    CHECK( ply_loader.isValid()             == true );
    CHECK( ply_loader.n_elements()          == 2 );
    CHECK( ply_loader.nVertexes()           == 1348 );
    CHECK( ply_loader.nIndexes()            == 2692 );
    CHECK( ply_loader.print_file() == "element: vertex (1348 instances)\n \
    property: x (type: float)\n     property: y (type: float)\n \
    property: z (type: float)\nelement: face (2692 instances)\n \
    property: vertex_indices (type: int, list count type: uchar)\n" );

    json bennu_json;
    ply_loader.ply_to_json( bennu_json );
    CHECK( bennu_json.dump() == "{\"elements\":[{\"element\":\"vertex\",\"properties\":[{\"property\":\
\"x\",\"type\":\"float\"},{\"property\":\"y\",\"type\":\"float\"},{\"property\":\
\"z\",\"type\":\"float\"}],\"size\":1348},{\"element\":\"face\",\"properties\":[{\"list_count_type\":\
\"uchar\",\"property\":\"vertex_indices\",\"type\":\"int\"}],\"size\":2692}]}" );

    CHECK( ply_loader.get_mesh().isValid()            == true );
    CHECK( ply_loader.get_mesh().isVectorDouble()     == false );
    CHECK( ply_loader.get_mesh().vectors().isDouble() == false );
    CHECK( ply_loader.get_mesh().vectors().isFloat()  == true );

    CHECK( ply_loader.get_double_vectors().size() == 1348 );
    CHECK( ply_loader.get_float_vectors().size()  == 1348 );
    // test - data values? Make sure the values of each are the same
    // may need to do check_that or reconvert float to double for check
    // And need to find out how to convert .ply binary to text file, via meshlab?
    CHECK( ply_loader.get_indexes().size()        == 2692 );

    CHECK( ply_loader.fetch_ply_file()->get_elements().size() == ply_loader.n_elements() );

    CHECK_NOTHROW( ply_loader.elapsed_life_time_s() >= 0 );
    CHECK_NOTHROW( ply_loader.track_count()         == 0 );
    CHECK_NOTHROW( ply_loader.performance_snapshot().runtime_s() >= 0 );

    std::shared_ptr<tinyply::PlyFile> ply_read = psmrts::PsmrtsPLYFormat::read_ply_file( plyfile );
    CHECK( ply_read != nullptr );
    //CHECK_THROWS( ply_read->request_properties_from_element( "face" , {"vertex_indices"}, 3 ) );
}

TEST_CASE( "PLY FORMAT Asset Test - Vector Data Value Test" ) {
    auto tolerance = 1.0e-6;

    std::string plyfile = psmrts_formats_path( "ply/data/Bennu_Radar.ply" );
    psmrts::PsmrtsPLYFormat ply_data_loader( plyfile );

    psmrts::PsmrtsVector3f ply_floats = ply_data_loader.get_float_vectors();
    psmrts::PsmrtsVector3d ply_doubles = ply_data_loader.get_double_vectors();

    for (int i = 0; i < ply_doubles.size(); i++) {
        CHECK_THAT( ply_doubles(i)[0], Catch::Matchers::WithinAbs(ply_floats(i)[0], tolerance));
        CHECK_THAT( ply_doubles(i)[1], Catch::Matchers::WithinAbs(ply_floats(i)[1], tolerance));
        CHECK_THAT( ply_doubles(i)[2], Catch::Matchers::WithinAbs(ply_floats(i)[2], tolerance));
    }
    
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

TEST_CASE( "PLY FORMAT Asset Test - Failcases Branch Checks", "[format][ply][failcase]") {
    std::string false_plyfile = psmrts_formats_path("ply/data/NotBennu_Radar.ply");
    psmrts::PsmrtsPLYFormat ply_loader;
    CHECK_THROWS( ply_loader.load_ply_file( false_plyfile ) );
    CHECK_THROWS( psmrts::PsmrtsPLYFormat::read_ply_file( false_plyfile ) );
}


TEST_CASE( "PLY FORMAT Asset Test - txt to ply Comparison Check", "[format][ply][txt][bennu]") {
    std::string txt_path = psmrts_formats_path("ply/data/Bennu_Radar.txt");
    std::string ply_path = psmrts_formats_path("ply/data/Bennu_Radar.ply");

    psmrts::PsmrtsPLYFormat txt_loader( txt_path, true );
    psmrts::PsmrtsPLYFormat ply_loader( ply_path );

    CHECK( ply_loader.format_model_source() != txt_loader.format_model_source() );
    CHECK( ply_loader.ply_source()          != txt_loader.ply_source() );
    CHECK( ply_loader.isValid()             == true );
    CHECK( txt_loader.isValid()             == true );
    CHECK( ply_loader.n_elements()          == txt_loader.n_elements() );
    CHECK( ply_loader.nVertexes()           == txt_loader.nVertexes() ); // 1348
    CHECK( ply_loader.nIndexes()            == txt_loader.nIndexes() ); // 2692
}


TEST_CASE( "PLY FORMAT Asset Test - txt Based PLY Check", "[format][ply][txt]") {
    std::string txt_file = psmrts_formats_path("ply/data/teapot.ply");

    psmrts::PsmrtsPLYFormat txt_loader( txt_file, true );

    CHECK( txt_loader.format_model_source() == txt_file );
    CHECK( txt_loader.ply_source()          == txt_file );
    CHECK( txt_loader.isValid()             == true );
    CHECK( txt_loader.n_elements()          == 2 );
    CHECK( txt_loader.nVertexes()           == 10 );
    CHECK( txt_loader.nIndexes()            == 10 );


}
*/