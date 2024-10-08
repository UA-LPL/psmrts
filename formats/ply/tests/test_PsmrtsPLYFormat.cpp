#include <psmrts_catch2_environment.hpp>

#include <PsmrtsPLYFormat.hpp>

#include <DskKernelModel.hpp>

// Test Default Constructor for PsmrtsPLYFormat
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