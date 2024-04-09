
#include <psmrts_catch2_environment.hpp>
#include <NaifEllipsoidShape.hpp>
#include <DskKernelModel.hpp>
#include <RayTrace.hpp>

TEST_CASE( "Ray Trace Default Test - Basic Spheroid Ellipse", "[ray][trace]") {
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

TEST_CASE("Ray Trace - Basic DSK Equivalent.", "[ray][trace][dsk]") {
    const double tolerance = 1.0e-6;
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );
    naif::DskSegment segment = dsk.segment();
    psmrts::RayTrace dsk_ray;

    Eigen::Vector3d obs;
    double radius = segment.maximum_radius();
    double obs_long = 90.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10;

    Eigen::Vector3d surf;
    double surf_long = 90.0 * rpd_c();
    double surf_lat = 45.0 * rpd_c();
    latrec_c( radius, surf_long, surf_lat, surf.data() );

    Eigen::Vector3d lkdr = -obs + surf;
    auto ray_trace = dsk.ray_trace( obs, lkdr, dsk_ray ); 
    
    REQUIRE ( dsk_ray.hasHit() == true );
    CHECK( dsk_ray.observer() == obs );
    CHECK ( dsk_ray.lookdir() == lkdr );

    CHECK_THAT ( dsk_ray.normal()[0], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
    CHECK_THAT ( dsk_ray.normal()[1], Catch::Matchers::WithinAbs( 0.5257310809, tolerance ) );
    CHECK_THAT ( dsk_ray.normal()[2], Catch::Matchers::WithinAbs( 0.8506508276, tolerance ) );

    CHECK_THAT ( dsk_ray.surfpt()[0], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
    CHECK_THAT ( dsk_ray.surfpt()[1], Catch::Matchers::WithinAbs( -1.8381441539, tolerance ) );
    CHECK_THAT ( dsk_ray.surfpt()[2], Catch::Matchers::WithinAbs( -1.8381441539, tolerance ) );

    CHECK_THAT ( dsk_ray.xyz()[0], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
    CHECK_THAT ( dsk_ray.xyz()[1], Catch::Matchers::WithinAbs( 0.1634276563, tolerance ) );
    CHECK_THAT ( dsk_ray.xyz()[2], Catch::Matchers::WithinAbs( 0.1634276563, tolerance ) );

    CHECK ( dsk_ray.radius() == dsk_ray.xyz().norm() );
    CHECK_THAT ( dsk_ray.radius(), Catch::Matchers::WithinAbs( 0.231121608, tolerance) ); // DSK Radii known to have inherent bug complications

    CHECK_THAT ( dsk_ray.slant_distance(), Catch::Matchers::WithinAbs( 2.599528392, tolerance ) );
    CHECK ( dsk_ray.slant_distance() == dsk_ray.surfpt().norm() );


    Eigen::Vector3d obs2;
    double obs2_long = 90.5 * rpd_c();
    double obs2_lat = 44.5 * rpd_c(); 
    latrec_c( radius, obs2_long, obs2_lat, obs2.data() );

    Eigen::Vector3d surf2;
    double surf2_long = 91.0 * rpd_c();
    double surf2_lat = 46.0 * rpd_c();
    latrec_c( radius, surf2_long, surf2_lat, surf2.data() );

    Eigen::Vector3d lkdr2 = -obs + surf2;
    psmrts::RayTrace dsk_ray2;
    auto ray_trace2 = dsk.ray_trace(obs, lkdr2, dsk_ray2);

    REQUIRE( dsk_ray2.hasHit() == true );
    CHECK_THAT( dsk_ray.distance(dsk_ray2), Catch::Matchers::WithinAbs( 0.0062732157, tolerance ) );
    CHECK_THAT ( dsk_ray.separation_angle(surf, surf2), Catch::Matchers::WithinAbs( vsep_c(surf.data(), surf2.data()), tolerance ));

    CHECK ( dsk_ray.isNear(dsk_ray2) == false );

    CHECK_THAT ( dsk_ray.incidence(dsk_ray2), Catch::Matchers::WithinAbs( 0.2337750681, tolerance ) ); 

    CHECK_THAT (dsk_ray.emission(), Catch::Matchers::WithinAbs( 0.2318238412, tolerance) );

    double emission_angle = dpr_c() * 0.2318238412;
    CHECK_THAT ( emission_angle, Catch::Matchers::WithinAbs( 13.2825276913, tolerance ) );
    
}