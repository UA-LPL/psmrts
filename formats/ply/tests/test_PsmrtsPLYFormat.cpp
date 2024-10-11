#include <psmrts_catch2_environment.hpp>

#include <PsmrtsPLYFormat.hpp>

#include <DskKernelModel.hpp>

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
*/

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
    CHECK( ply_loader.ply_source() == plyfile );
    CHECK( ply_loader.isValid() == true );
    CHECK( ply_loader.n_elements() == 2 );
    CHECK( ply_loader.nVertexes() == 1348 );
    CHECK( ply_loader.nIndexes() == 2692 );
    CHECK( ply_loader.print_file() == "element: vertex (1348 instances)\n \
    property: x (type: float)\n     property: y (type: float)\n \
    property: z (type: float)\nelement: face (2692 instances)\n \
    property: vertex_indices (type: int, list count type: uchar)\n" );

    CHECK( ply_loader.get_mesh().isValid() == true );
    CHECK( ply_loader.get_mesh().isVectorDouble() == false );
    CHECK( ply_loader.get_mesh().vectors().isDouble() == false );
    CHECK( ply_loader.get_mesh().vectors().isFloat() == true );

    CHECK( ply_loader.get_double_vectors().size() == 1348 );
    CHECK( ply_loader.get_float_vectors().size() == 1348 );
    // test - data values? Make sure the values of each are the same
    // may need to do check_that or reconvert float to double for check
    // And need to find out how to convert .ply binary to text file, via meshlab?
    CHECK( ply_loader.get_indexes().size() == 2692 );

    CHECK( ply_loader.fetch_ply_file()->get_elements().size() == ply_loader.n_elements() );

    CHECK_NOTHROW( ply_loader.elapsed_life_time_s() >= 0 );
    CHECK_NOTHROW( ply_loader.track_count() == 0 );
    CHECK_NOTHROW( ply_loader.performance_snapshot().runtime_s() >= 0 );

    std::shared_ptr<tinyply::PlyFile> ply_read = psmrts::PsmrtsPLYFormat::read_ply_file( plyfile );
    CHECK( ply_read != nullptr );
    //CHECK_THROWS( ply_read->request_properties_from_element( "face" , {"vertex_indices"}, 3 ) );
}

TEST_CASE( "PLY FORMAT Asset Test - Failcases Branch Checks", "[format][ply][failcase]") {
    std::string false_plyfile = psmrts_formats_path("ply/data/NotBennu_Radar.ply");
    psmrts::PsmrtsPLYFormat ply_loader;
    CHECK_THROWS( ply_loader.load_ply_file( false_plyfile ) );
    CHECK_THROWS( psmrts::PsmrtsPLYFormat::read_ply_file( false_plyfile ) );
}