#include <psmrts_catch2_environment.hpp>

#include <PsmrtsPLYFormat.hpp>

#include <DskKernelModel.hpp>

// Test Default Constructor for PsmrtsPLYFormat
TEST_CASE ( "PLY FORMAT Asset Test - Default Constructor", "[format][ply][default]") {
    std::string plyfile = psmrts_formats_path("ply/data/Bennu_Radar.ply");
    psmrts::PsmrtsPLYFormat ply(plyfile);

    CHECK( ply.isValid() == true );
    CHECK( ply.nElements() == 2 );
    //CHECK( ply.nVertexes() == 1348 );
    CHECK( ply.nIndexes() == 2692 );
    CHECK( ply.print_file() == "000" );

    CHECK ( ply.get_double_vectors().vector_size() == 1348 );
    CHECK ( ply.get_indexes().vector_size() == 2692 );
}