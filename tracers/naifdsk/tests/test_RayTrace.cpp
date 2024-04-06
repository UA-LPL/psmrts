
#include <psmrts_catch2_environment.hpp>
#include <NaifEllipsoidShape.hpp>
#include <RayTrace.hpp>

TEST_CASE( "Ray Trace Default Test", "[ray][trace]") {
    const double tolerance = 1.0e-10;

    naif::NaifEllipsoidShape ellipse(0.5);
    psmrts::RayTrace ray;

    Eigen::Vector3d obs = {1.0, 1.0, 1.0};
    Eigen::Vector3d surf = {0.0, 0.5, 0.0};
    Eigen::Vector3d lkdr = -obs + surf;

    auto ray_trace = ellipse.ray_trace(obs, lkdr, ray);

    CHECK ( ray.hasHit() == true );
    CHECK ( ray.observer() == obs );
    CHECK ( ray.lookdir() == lkdr );
    Eigen::Vector3d norm;
    surfnm_c( ellipse.a(), ellipse.b(), ellipse.c(), surf.data(), norm.data() );
    
    CHECK_THAT ( ray.normal()[0], Catch::Matchers::WithinAbs( norm[0], tolerance ) );
    CHECK_THAT ( ray.normal()[1], Catch::Matchers::WithinAbs( norm[1], tolerance ) );
    CHECK_THAT ( ray.normal()[2], Catch::Matchers::WithinAbs( norm[2], tolerance ) );

    CHECK_THAT ( ray.surfpt()[0], Catch::Matchers::WithinAbs( lkdr[0], tolerance ) );
    CHECK_THAT ( ray.surfpt()[1], Catch::Matchers::WithinAbs( lkdr[1], tolerance ) );
    CHECK_THAT ( ray.surfpt()[2], Catch::Matchers::WithinAbs( lkdr[2], tolerance ) );

    CHECK_THAT ( ray.xyz()[0], Catch::Matchers::WithinAbs( surf[0], tolerance ) );
    CHECK_THAT ( ray.xyz()[1], Catch::Matchers::WithinAbs( surf[1], tolerance ) );
    CHECK_THAT ( ray.xyz()[2], Catch::Matchers::WithinAbs( surf[2], tolerance ) );

    CHECK ( ray.radius() == ray.xyz().norm() );
    CHECK ( ray.radius() == surf.norm() );

    CHECK_THAT ( ray.slant_distance(), Catch::Matchers::WithinAbs( lkdr.norm(), tolerance ) );
    CHECK ( ray.slant_distance() == ray.surfpt().norm() );

    Eigen::Vector3d surf2 = {0.0, 0.5, 0.1};
    Eigen::Vector3d lkdr2 = -obs + surf2;
    psmrts::RayTrace ray2;
    auto ray_trace2 = ellipse.ray_trace(obs, lkdr2, ray2);
    REQUIRE( ray2.hasHit() == true );
    CHECK_THAT( ray.distance(ray2), Catch::Matchers::WithinAbs( 0.0878260912, tolerance ) );
    CHECK( ray.separation_angle(surf, surf2) == vsep_c(surf.data(), surf2.data()) ); // in Radians

    CHECK ( ray.isNear(ray2) == false );

    CHECK_THAT ( ray.incidence(ray2), Catch::Matchers::WithinAbs( 1.2149684443, tolerance ) ); // tied to sep angle

    CHECK_THAT (ray.emission(), Catch::Matchers::WithinAbs( 1.2309594173, tolerance) );

    double emission_angle = dpr_c() * 1.2309594173;
    CHECK_THAT ( emission_angle, Catch::Matchers::WithinAbs( 70.5287793632, tolerance ) );

}
