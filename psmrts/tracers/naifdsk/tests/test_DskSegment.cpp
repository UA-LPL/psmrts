#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/tracers/naifdsk/NaifDskTracer.hpp>
#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>
#include <psmrts/tracers/naifdsk/private/DskSegment.hpp>

TEST_CASE( "NAIF Dsk Segment Default Test", "[naifdsk][segment]") {
    naif::DskSegment seg;

    CHECK( seg.isValid()        == false );
    CHECK( seg.segment_number() == -1 );
    CHECK( seg.id()             == 0 );
    CHECK( seg.n_vertices()     == 0 );
    CHECK( seg.n_plates()       == 0); 
    CHECK( seg.n_vectors()      == 0 );
    CHECK( seg.dladsc_ptr()     != nullptr ); 
    CHECK( seg.dskdsc_ptr()     != nullptr );

    SpiceDLADescr dladsc = seg.dladsc();
    CHECK( dladsc.bwdptr == 0 );
    CHECK( dladsc.cbase  == 0 );
    CHECK( dladsc.csize  == 0 );
    CHECK( dladsc.dbase  == 0 );
    CHECK( dladsc.dsize  == 0 );
    CHECK( dladsc.fwdptr == 0 );
    CHECK( dladsc.ibase  == 0 );
    CHECK( dladsc.isize  == 0 );

    SpiceDSKDescr dskdsc = seg.dskdsc();
    CHECK( dskdsc.center == 0 );
    CHECK( dskdsc.co1max == 0 );
    CHECK( dskdsc.co1min == 0 );
    CHECK( dskdsc.co2max == 0 );
    CHECK( dskdsc.co2min == 0 );
    CHECK( dskdsc.co3max == 0 );
    CHECK( dskdsc.co3min == 0 );
    CHECK( dskdsc.corsys == 0 );
    CHECK( dskdsc.dclass == 0 );
    CHECK( dskdsc.dtype  == 0 );
    CHECK( dskdsc.frmcde == 0 );
    CHECK( dskdsc.start  == 0 );
    CHECK( dskdsc.stop   == 0 ); 
    CHECK( dskdsc.surfce == 0 );

    for (int i = 0; i < SPICE_DSK_NSYPAR; ++i) {
        CHECK( dskdsc.corpar[i] == 0 );
    }
    
    CHECK( seg.radii()          == Eigen::Vector3d({0, 0, 0}));
    CHECK( seg.minimum_radius() == 0 );
    CHECK( seg.maximum_radius() == 0 );
    CHECK( seg.bodyid()         == 0 );
    CHECK( seg.surfaceid()      == 0 );
    CHECK( seg.frameid()        == 0 ); 
    CHECK( seg.dtype()          == dskdsc.dtype );
    CHECK( seg.dclass()         == dskdsc.dclass );   
}

TEST_CASE( "NAIF Dsk Segment Values Test", "[naifdsk][segment][values]") {
    const double tolerance = 1.0e-9;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );
    naif::DskSegment seg = dsk.segment();

    CHECK( seg.isValid()        == true );
    CHECK( seg.segment_number() == 0 );
    CHECK( seg.id()             == 2101955 );
    CHECK( seg.n_vertices()     == 20 );
    CHECK( seg.n_plates()       == 36 ); 
    CHECK( seg.n_vectors()      == 20  );
    CHECK( seg.dladsc_ptr()     != nullptr ); 
    CHECK( seg.dskdsc_ptr()     != nullptr );

    SpiceDLADescr dladsc = seg.dladsc();
    CHECK( dladsc.bwdptr == -1 );
    CHECK( dladsc.cbase  == 0 );
    CHECK( dladsc.csize  == 0 );
    CHECK( dladsc.dbase  == 0 );
    CHECK( dladsc.dsize  == 94 );
    CHECK( dladsc.fwdptr == -1 );
    CHECK( dladsc.ibase  == 11 );
    CHECK( dladsc.isize  == 726 );

    SpiceDSKDescr dskdsc = seg.dskdsc();
    CHECK( dskdsc.center == 2101955 );

    CHECK_THAT( dskdsc.co1max, Catch::Matchers::WithinAbs(  6.28318530717958623, tolerance));

    CHECK( dskdsc.co1min == 0 );

    CHECK_THAT( dskdsc.co2max, Catch::Matchers::WithinAbs(  1.57079632679489656, tolerance));
    CHECK_THAT( dskdsc.co2min, Catch::Matchers::WithinAbs( -1.57079632679489656, tolerance));
    CHECK_THAT( dskdsc.co3max, Catch::Matchers::WithinAbs(  0.28306500000006679, tolerance));
    CHECK_THAT( dskdsc.co3min, Catch::Matchers::WithinAbs(  0.22493886860043516, tolerance));

    CHECK( dskdsc.corsys == 1 );
    CHECK( dskdsc.dclass == 1 );
    CHECK( dskdsc.dtype  == 2 );
    CHECK( dskdsc.frmcde == 10106 );

    CHECK_THAT( dskdsc.start, Catch::Matchers::WithinAbs( -1577879958.81605863571166992, tolerance));
    CHECK_THAT( dskdsc.stop,  Catch::Matchers::WithinAbs(   1577880069.18391323089599609, tolerance));
 
    CHECK( dskdsc.surfce == 2101955 );

    for (int i = 0; i < SPICE_DSK_NSYPAR; ++i) {
        CHECK( dskdsc.corpar[i] == 0 );
    }

    CHECK_THAT( seg.radii()[0], Catch::Matchers::WithinAbs( 0.283065, tolerance));
    CHECK_THAT( seg.radii()[1], Catch::Matchers::WithinAbs( 0.283065, tolerance));
    CHECK_THAT( seg.radii()[2], Catch::Matchers::WithinAbs( 0.283065, tolerance));
    CHECK_THAT( seg.minimum_radius(), Catch::Matchers::WithinAbs( 0.22493886860043516, tolerance));
    CHECK_THAT( seg.maximum_radius(), Catch::Matchers::WithinAbs( 0.28306500000006679, tolerance));

    CHECK( seg.bodyid()         == 2101955 );
    CHECK( seg.surfaceid()      == 2101955 );
    CHECK( seg.frameid()        == 10106); 
    CHECK( seg.dtype()          == dskdsc.dtype );
    CHECK( seg.dclass()         == dskdsc.dclass );  
}
