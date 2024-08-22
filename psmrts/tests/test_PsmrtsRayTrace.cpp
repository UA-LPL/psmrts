#include <psmrts_catch2_environment.hpp>

#include <PsmrtsRayTrace.hpp>
#include <Eigen/Geometry>
#include <cspice/SpiceUsr.h>

TEST_CASE( "PsmrtsRayTrace Default Test", "[ray][trace][default]") {
    const double tolerance = 1.0e-12;

    // Remove:
    Eigen::Vector3d obs = {1.0, 1.0, 1.0};
    Eigen::Vector3d surf = {0.0, 0.5, 0.0};
    Eigen::Vector3d lkdr = -obs + surf;

    psmrts::PsmrtsRayTrace ray;
    // test each constructor, one with values (obs, surf, lkdr), one without - make sure they retain
    // state must be false in all
    // test datums in their own tests, down below
    CHECK( ray.hasHit() == false );
    CHECK( ray.observer() == Eigen::Vector3d {0.0, 0.0, 0.0} );
    CHECK( ray.lookdir() == lkdr );
    CHECK( ray.normal() == Eigen::Vector3d {0.0, 0.0, 0.0} );
    CHECK( ray.surfpt() == surf ); // {-1.0, -1.0, -1.0}
    CHECK( ray.xyz() == Eigen::Vector3d {0.0, 0.0, 0.0 } ); // should be 0.0, 0.5?, 0.0?
    CHECK( ray.radius() == 0.0 );
    CHECK( ray.slant_distance() == 1.73205080756887719 );
    CHECK( ray.separation_angle(obs, Eigen::Vector3d {0.0, 0.0, 0.0}) == 0.0 );
    // CHECK( ray.emission() == 0.0 ); NAN error
    // Still need to check with a comparative raytrace

    psmrts::PsmrtsRayTrace::facet_datum facet;
    CHECK( facet.isValid() == false );

    // Basic instantiation of a ray trace itself, making sure default conditions are set
    // create another instantiation with values, then use that to create another to ensure each constructor works

    // 2 more tests, one is for facet datum - default case, constructors, 
    //              second is for ray trace datum, constructers (obs, lkdir), reset, then retest (should no longer be valid)

    // These are to be done later:
    // For each tracer (ellipsoid), then check for facet in ellipsoid - should be false.
    // Any mathmatically based models should not have a facet.
    // Then naifdsk tracers now with valid facets, pull naif mesh, and see if you can get same facet from dsk
    // (can do similar with bullet, as well)

}

TEST_CASE( "PsmrtsRayTrace Values / Comparison Test", "[ray][trace][values]") {
    const double tolerance = 1.0e-12;

    Eigen::Vector3d obs = {1.0, 1.0, 1.0};
    Eigen::Vector3d surf = {0.0, 0.5, 0.0};
    Eigen::Vector3d lkdr = -obs + surf;

    psmrts::PsmrtsRayTrace ray( obs, lkdr );

    Eigen::Vector3d norm;
    surfnm_c( 0.5, 0.5, 0.5, surf.data(), norm.data() );

    CHECK_THAT ( ray.normal()[0], Catch::Matchers::WithinAbs( norm[0], tolerance ) );
    CHECK_THAT ( ray.normal()[1], Catch::Matchers::WithinAbs( norm[1], tolerance ) );
    CHECK_THAT ( ray.normal()[2], Catch::Matchers::WithinAbs( norm[2], tolerance ) );

    CHECK_THAT ( ray.surfpt()[0], Catch::Matchers::WithinAbs( lkdr[0], tolerance ) );
    CHECK_THAT ( ray.surfpt()[1], Catch::Matchers::WithinAbs( lkdr[1], tolerance ) );
    CHECK_THAT ( ray.surfpt()[2], Catch::Matchers::WithinAbs( lkdr[2], tolerance ) );

    CHECK_THAT ( ray.xyz()[0], Catch::Matchers::WithinAbs( surf[0], tolerance ) );
    CHECK_THAT ( ray.xyz()[1], Catch::Matchers::WithinAbs( surf[1], tolerance ) );
    CHECK_THAT ( ray.xyz()[2], Catch::Matchers::WithinAbs( surf[2], tolerance ) );

    CHECK( ray.radius() == ray.xyz().norm() );
    CHECK( ray.radius() == surf.norm() );

}
