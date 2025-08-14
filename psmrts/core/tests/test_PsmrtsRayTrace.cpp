#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <Eigen/Geometry>
#include <cspice/SpiceUsr.h>

#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>

// Add to readme for testing: (conditions for testing)
// doing any higher level raytrace testing, we're going to use ellipse as a baseline
// due to it being the simplest, and most commonly relevant, shape object to 
// test raytracing on. It is also ensured to be available at all levels of
// psmrst/testing.

TEST_CASE( "PsmrtsRayTrace Default Test", "[ray][trace][default]") { 
    // No parameters
    psmrts::PsmrtsRayTrace ray_basic;

    CHECK( ray_basic.hasHit()         == false );
    CHECK( ray_basic.observer()       == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ray_basic.lookdir()        == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ray_basic.normal()         == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ray_basic.raypt()         == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ray_basic.xyz()            == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ray_basic.radius()         == 0.0 );
    CHECK( ray_basic.slant_distance() == 0.0 );
    CHECK( psmrts::isnull( ray_basic.emission() ) == true ); 
    CHECK( ray_basic.plateid()        == -1 );
    CHECK( ray_basic.segment_number() == -1 );
    CHECK( ray_basic.datum().hasHit() == false );

}

TEST_CASE( "PsmrtsRayTrace Facet Test", "[ray][trace][facet][default]") {
    psmrts::PsmrtsRayTrace::facet_datum d_facet;
    CHECK( d_facet.isValid() == false ); 
    CHECK( d_facet.m_indexes == Eigen::Vector3i { -1, -1, -1 } );
    CHECK( d_facet.m_vector1 == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_facet.m_vector2 == Eigen::Vector3d { 0.0, 0.0, 0.0 } ); 
    CHECK( d_facet.m_vector3 == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_facet.m_normal  == Eigen::Vector3d { 0.0, 0.0, 0.0 } );

    psmrts::PsmrtsRayTrace::ray_trace_datum d_ray;
    CHECK( d_ray.hasHit()   == false );
    CHECK( d_ray.m_observer == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_lookdir  == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_xyz      == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_normal   == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_plateid  == -1 );
    CHECK( d_ray.m_segment  == -1 );

    Eigen::Vector3d obs  = { 2.0, 2.0, 2.0 };
    Eigen::Vector3d surf = { 0.0, 0.5, 0.0 };
    Eigen::Vector3d lkdr = -obs + surf;
    // Reset datum with values
    d_ray.reset( obs, lkdr );
    CHECK( d_ray.hasHit()   == false );
    CHECK( d_ray.m_observer == Eigen::Vector3d { 2.0, 2.0, 2.0 } );
    CHECK( d_ray.m_lookdir  == (-obs + surf) );
    CHECK( d_ray.m_xyz      == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_normal   == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_plateid  == -1 );
    CHECK( d_ray.m_segment  == -1 );
    // Second reset without values, ensure default
    d_ray.reset();
    CHECK( d_ray.hasHit()   == false );
    CHECK( d_ray.m_observer == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_lookdir  == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_xyz      == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_normal   == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( d_ray.m_plateid  == -1 );
    CHECK( d_ray.m_segment  == -1 );
}

// Basic instantiation of a ray trace itself, making sure default conditions are set
// create another instantiation with values, then use that to create another to ensure each constructor works

// 2 more tests, one is for facet datum - default case, constructors, 
// second is for ray trace datum, constructers (obs, lkdir), reset, then retest (should no longer be valid)

// These are to be done later:
// For each tracer (ellipsoid), then check for facet in ellipsoid - should be false.
// Any mathmatically based models should not have a facet.
// Then naifdsk tracers now with valid facets, pull naif mesh, and see if you can get same facet from dsk
// (can do similar with bullet, as well)

TEST_CASE( "PsmrtsRayTrace Baseline Values Test - Ellipsoid", "[ray][trace][values][ellipse]" ) {
    const double tolerance = 1.0e-6;

    psmrts::EllipsoidTracer e_tracer; // a, b, c default to 1.0 

    Eigen::Vector3d obs;
    double radius   = 1.0;  /// FIX THIS!!  e_tracer.maximum_radius();
    CHECK( radius   == 1.0 );
    double obs_long = psmrts::degrees_to_radians(45.0);
    double obs_lat  = psmrts::degrees_to_radians(45.0);
    latrec_c( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 2.0;
    CHECK_THAT( obs[0], Catch::Matchers::WithinAbs( 1.00000000000000022, tolerance ));
    CHECK_THAT( obs[1], Catch::Matchers::WithinAbs( 1.0, tolerance ));
    CHECK_THAT( obs[2], Catch::Matchers::WithinAbs( 1.41421356237309492, tolerance ));

    Eigen::Vector3d surf;
    double surf_long = psmrts::degrees_to_radians(45.0);
    double surf_lat  = psmrts::degrees_to_radians(45.0);
    latrec_c ( radius, surf_long, surf_lat, surf.data() );

    Eigen::Vector3d lkdr = obs - surf;

    psmrts::PsmrtsRayTrace ellipse_ray;

    CHECK( e_tracer.ray_trace( -obs, lkdr, ellipse_ray) == true );
    CHECK( ellipse_ray.hasHit()   == true ); // If valid, other functions should be as well.
    CHECK( ellipse_ray.observer() == -obs );
    CHECK( ellipse_ray.lookdir()  == lkdr );
    CHECK( ellipse_ray.raypt()   == ellipse_ray.xyz() + obs ); // minus -obs?
    CHECK_THAT( ellipse_ray.normal()[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( ellipse_ray.normal()[1], Catch::Matchers::WithinAbs( -0.5, tolerance )); 
    CHECK_THAT( ellipse_ray.normal()[2], Catch::Matchers::WithinAbs( -0.707107, tolerance )); 
    CHECK_THAT( ellipse_ray.xyz()[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( ellipse_ray.xyz()[1], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( ellipse_ray.xyz()[2], Catch::Matchers::WithinAbs( -0.707107, tolerance ));
    CHECK_THAT( ellipse_ray.radius(), Catch::Matchers::WithinAbs( 1.0, tolerance ));
    CHECK_THAT( ellipse_ray.slant_distance(), Catch::Matchers::WithinAbs( 1.0, tolerance ));
    CHECK_THAT( ellipse_ray.emission(), Catch::Matchers::WithinAbs( 0.0, tolerance ));
    CHECK( ellipse_ray.plateid()        == -1 );
    CHECK( ellipse_ray.segment_number() == -1 ); 
    CHECK( psmrts::isnull(ellipse_ray.emission()) ==  false );   


    // Constructor initialized with another ray as parameter
    psmrts::PsmrtsRayTrace second_ray( ellipse_ray );

    CHECK( second_ray.hasHit()   == true ); 
    CHECK( second_ray.observer() == -obs );
    CHECK( second_ray.lookdir()  == lkdr ); // NAN (I/O)
    CHECK( second_ray.raypt()   == ellipse_ray.xyz() + obs ); // minus -obs?
    CHECK_THAT( second_ray.normal()[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( second_ray.normal()[1], Catch::Matchers::WithinAbs( -0.5, tolerance )); 
    CHECK_THAT( second_ray.normal()[2], Catch::Matchers::WithinAbs( -0.707107, tolerance )); 
    CHECK_THAT( second_ray.xyz()[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( second_ray.xyz()[1], Catch::Matchers::WithinAbs( -0.5, tolerance ));
    CHECK_THAT( second_ray.xyz()[2], Catch::Matchers::WithinAbs( -0.707107, tolerance ));
    CHECK_THAT( second_ray.radius(), Catch::Matchers::WithinAbs( 1.0, tolerance )); 
    CHECK_THAT( second_ray.slant_distance(), Catch::Matchers::WithinAbs( 1.0, tolerance ));
    CHECK_THAT( second_ray.emission(), Catch::Matchers::WithinAbs( 0.0, tolerance ));
    CHECK( second_ray.plateid()        == -1 );
    CHECK( second_ray.segment_number() == -1 ); 
    CHECK( psmrts::isnull(second_ray.emission()) ==  false );   


    // Reset with new obs / lkdr values
    Eigen::Vector3d obs_reset;
    double obs_long_r = psmrts::degrees_to_radians(44.5);
    double obs_lat_r = psmrts::degrees_to_radians(44.5);
    latrec_c( radius, obs_long_r, obs_lat_r, obs_reset.data() );
    obs_reset = obs_reset * 2.0;

    Eigen::Vector3d surf_reset;
    double surf_long_r = psmrts::degrees_to_radians(44.5);
    double surf_lat_r = psmrts::degrees_to_radians(44.5);
    latrec_c( radius, surf_long_r, surf_lat_r, surf_reset.data());
    Eigen::Vector3d lkdr_reset = obs_reset - surf_reset;

    CHECK_NOTHROW( ellipse_ray.reset( -obs_reset, lkdr_reset ) );
    psmrts::PsmrtsRayTrace reset_ray;
    e_tracer.ray_trace( ellipse_ray.observer(), ellipse_ray.lookdir(), reset_ray);
    CHECK( reset_ray.hasHit()   == true ); 
    CHECK( reset_ray.observer() == -obs_reset );
    CHECK( reset_ray.lookdir()  == lkdr_reset ); 
    CHECK_THAT( reset_ray.normal()[0], Catch::Matchers::WithinAbs( -0.508726, tolerance ));
    CHECK_THAT( reset_ray.normal()[1], Catch::Matchers::WithinAbs( -0.499924, tolerance )); 
    CHECK_THAT( reset_ray.normal()[2], Catch::Matchers::WithinAbs( -0.700909, tolerance )); 
    CHECK_THAT( reset_ray.xyz()[0], Catch::Matchers::WithinAbs( -0.508726, tolerance ));
    CHECK_THAT( reset_ray.xyz()[1], Catch::Matchers::WithinAbs( -0.499924, tolerance ));
    CHECK_THAT( reset_ray.xyz()[2], Catch::Matchers::WithinAbs( -0.700909, tolerance ));
    CHECK( reset_ray.raypt()          == reset_ray.xyz() + obs_reset );
    CHECK( reset_ray.radius()         == 1.0 ); 
    CHECK( reset_ray.slant_distance() == 1.0 ); 
    CHECK( reset_ray.emission()       == 0.0 ); 
    CHECK( reset_ray.plateid()        == -1 );
    CHECK( reset_ray.segment_number() == -1 );
    CHECK( psmrts::isnull(reset_ray.emission()) ==  false ); 


    // Full reset to default
    CHECK_NOTHROW( ellipse_ray.reset() );
    CHECK( ellipse_ray.hasHit()   == false );
    CHECK( ellipse_ray.observer() == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ellipse_ray.lookdir()  == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ellipse_ray.normal()   == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ellipse_ray.raypt()    == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ellipse_ray.xyz()      == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
    CHECK( ellipse_ray.radius()   == 0.0 );
    CHECK( ellipse_ray.slant_distance() == 0.0 );
    CHECK( psmrts::isnull( ellipse_ray.emission() ) == true ); 
    CHECK( ellipse_ray.plateid()        == -1 );
    CHECK( ellipse_ray.segment_number() == -1 );
    CHECK( ellipse_ray.datum().hasHit() == false );
}   