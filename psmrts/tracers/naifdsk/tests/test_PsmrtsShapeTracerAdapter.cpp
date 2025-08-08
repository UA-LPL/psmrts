#include <psmrts/core/testspsmrts_catch2_environment.hpp>

#include <NaifUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsTracerModel.hpp>
#include <psmrts/core/PsmrtsShapeTracerAdapter.hpp>
#include <NaifEllipsoidShape.hpp>

typedef psmrts::PsmrtsShapeTracerAdapter<naif::NaifEllipsoidShape> PsmrtsAdaptedEllipsoidShape;

TEST_CASE( "Naif Shape Tracer Adapter Default Test", "[naif][shape][tracer][default]") {
    const double tolerance = 1.0e-6;

    PsmrtsAdaptedEllipsoidShape adapt_ellipsoid;
    
    CHECK( adapt_ellipsoid.model().a() == 1.0);
    CHECK( adapt_ellipsoid.model().b() == 1.0);
    CHECK( adapt_ellipsoid.model().c() == 1.0);

    CHECK( adapt_ellipsoid.model().a() == adapt_ellipsoid.model().minimum_radius() );
    CHECK( adapt_ellipsoid.model().c() == adapt_ellipsoid.maximum_radius() );


    Eigen::Vector3d obs;
    double radius = 1.0;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;


    Eigen::Vector3d surf;
    double surf_long = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_long, surf_lat, surf.data() );


    Eigen::Vector3d lkdr = surf - obs;

    psmrts::PsmrtsRayTrace spt;

    bool good = adapt_ellipsoid.ray_trace( obs, lkdr, spt );
    Eigen::Vector3d normal = spt.normal();

    CHECK( good == true );



    Eigen::Vector3d naif_spt ( { 0, 0, 0, } );
    SpiceBoolean found; 
    (void) surfpt_c( obs.data(), lkdr.data(), adapt_ellipsoid.model().a(), adapt_ellipsoid.model().b(), adapt_ellipsoid.model().c(), naif_spt.data(), &found );

    Eigen::Vector3d naif_normal ( { 0, 0, 0, } );
    (void) surfnm_c( adapt_ellipsoid.model().a(), adapt_ellipsoid.model().b(), adapt_ellipsoid.model().c(), naif_spt.data(), naif_normal.data() );

    CHECK( found == SPICETRUE );
    CHECK_THAT ( normal[0] , Catch::Matchers::WithinAbs( naif_normal[0], tolerance )); 
    CHECK_THAT ( normal[1] , Catch::Matchers::WithinAbs( naif_normal[1], tolerance ));
    CHECK_THAT ( normal[2] , Catch::Matchers::WithinAbs( naif_normal[2], tolerance ));

    CHECK( adapt_ellipsoid.clone() != nullptr );

    CHECK( adapt_ellipsoid.tracer_model_type() == "psmrts" );
    CHECK( adapt_ellipsoid.tracer_model_name() == "NaifEllipsoid" );
    CHECK( adapt_ellipsoid.shape_tracer_id() == "psmrts::NaifEllipsoid::UnitSpheroid" );
    CHECK( adapt_ellipsoid.shapefile() == "UnitSpheroid" );
    CHECK( adapt_ellipsoid.plate_count() == 0 );
    CHECK( adapt_ellipsoid.vertex_count() == 0 );
    CHECK( adapt_ellipsoid.use_count() == 0 );

}