#include <psmrts_catch2_environment.hpp>

#include <PsmrtsBulletWorldModel.hpp>
#include <BulletTracerModel.hpp>
#include <BulletShapeTracer.hpp>
#include <PsmrtsOBJFormat.hpp>
#include <PsmrtsUtilities.hpp>

TEST_CASE ( "Bullet Shape Tracer - Default Constructor", "[default][bullet][shapetracer]" ) {
    psmrts::BulletShapeTracer b_tracer;

    psmrts::PRQFeatures features;
    CHECK( b_tracer.process( features ) == true );

    CHECK( features.to_string()     == "[[[\"name\",\"bullet\"],[\"product\",\"shapetracer\"]\
,[\"mesh\",true],[\"optimizebvh\",false],[\"vectortype\",[\"double\",\"float\"]]]]" );
    CHECK( features.config().dump() == "[[[\"name\",\"bullet\"],[\"product\",\"shapetracer\"]\
,[\"mesh\",true],[\"optimizebvh\",false],[\"vectortype\",[\"double\",\"float\"]]]]" );

    psmrts_json add = "tracer";
    CHECK_NOTHROW(features.add_feature(add));
    CHECK(features.to_string() == "[[[\"name\",\"bullet\"],[\"product\",\"shapetracer\"]\
,[\"mesh\",true],[\"optimizebvh\",false],[\"vectortype\",[\"double\",\"float\"]]],\"tracer\"]");
    // etc...

    // Base Request Functions
    CHECK( features.name() == "PRQFeatures" ); 

    CHECK_NOTHROW( features.process_running() );
    CHECK_NOTHROW( features.process_complete() );
 
    CHECK( features.run_count()     == 1 );
    CHECK( features.was_invoked()   == true );
    CHECK( features.error_count()   == 0 );
    CHECK( features.errors().size() == 0 ); 

    CHECK_NOTHROW( features.throw_errors() ); 
    CHECK_NOTHROW( features.clear_errors() );

    // Default Photometric Functions
    // Needs reference RT or NaN Errors
    // psmrts::PRQPhotometricTrace photoTrace;
    // CHECK( b_tracer.process( photoTrace ) == false ); 
    // CHECK( photoTrace.isValid() == false );
    // CHECK( photoTrace.incidence() == 0.0 );
    // CHECK( photoTrace.emission() == 0.0 );
    // CHECK( photoTrace.phase() == 0.0 );
    // CHECK( photoTrace.compute_sun_lookdir() == false );

}

// Load a mesh and inspect contents of Bullet world

// Load mesh and run some traces

TEST_CASE( "Bullet Shape Tracer Test", "[bullet][shapetracer]" ) {
    const double tolerance_km = 1.0e-6;

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );

    // This spec saves significant memory... and confirms Bullet preserves data
    psmrts::bullet::PsmrtsBulletWorldModel bt_world( psmrts::bullet::PsmrtsBulletMeshMap ( psmrts::PsmrtsOBJFormat( objfile ) ), objfile );
    REQUIRE( bt_world.isValid() == true );

    psmrts::BulletShapeTracer b_tracer( bt_world );

    // Compute the position of the observer at ( 45,45 ) degrees
    Eigen::Vector3d obs;
    double radius = 1.0;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;

    // Compute the surface point at (45, 50 ). This is our surface target
    Eigen::Vector3d surf;
    double surf_lon = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_lon, surf_lat, surf.data() );

    // Find the real surface point using bullet
    Eigen::Vector3d surf_obs = surf * 1.5;
    psmrts::PRQRayTrace prq_ray(surf_obs, -surf_obs );
    REQUIRE( b_tracer.process( prq_ray ) == true );

    // Now compute expected/precise look vector from observer to surface intercept point
    Eigen::Vector3d lkdr = prq_ray.trace().xyz() - obs;

    // Trace it from observer to surface point to confirm
    psmrts::PRQRayTrace prq_spt(obs, lkdr );
    REQUIRE( b_tracer.process( prq_spt ) );
    
    Eigen::Vector3d normal = prq_spt.trace().normal();
    Eigen::Vector3d xyz = prq_spt.trace().xyz();

    // Compare expected results!
    CHECK( prq_ray.isValid() == true );
    CHECK( prq_spt.isValid() == prq_spt.trace().hasHit() );

    CHECK_THAT( normal[0], Catch::Matchers::WithinAbs(0.0,                tolerance_km ));
    CHECK_THAT( normal[1], Catch::Matchers::WithinAbs(0.5257310881115882, tolerance_km ));
    CHECK_THAT( normal[2], Catch::Matchers::WithinAbs(0.85065082318951801, tolerance_km ));

    // Compute radius/lon/lat from intercept surface point (body-fixed)
    double bt_lat, bt_lon, bt_radius;
    reclat_c( xyz.data(), &bt_radius, &bt_lon, &bt_lat);
    
    CHECK_THAT( bt_lon,    Catch::Matchers::WithinAbs( 45.0 * rpd_c(), tolerance_km ));    
    CHECK_THAT( bt_lat,    Catch::Matchers::WithinAbs( 50.0 * rpd_c(), tolerance_km ));  

    CHECK_THAT( bt_radius, Catch::Matchers::WithinAbs( prq_spt.trace().radius(), tolerance_km ));    
    CHECK_THAT( bt_lon,    Catch::Matchers::WithinAbs( surf_lon,     tolerance_km ));    
    CHECK_THAT( bt_lat,    Catch::Matchers::WithinAbs( surf_lat,     tolerance_km ));    

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[0], tolerance_km ) );
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[1], tolerance_km ) );
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( prq_ray.trace().xyz()[2], tolerance_km ) );

    psmrts::PRQFacet prq_facet( prq_ray.trace() );
    CHECK( prq_facet.isValid() == true );
    //CHECK( prq_facet.facet().isValid() == true ); \\shouldnt this be true..?
    CHECK( prq_facet.prq_trace().emission() == prq_ray.emission() ); 

    Eigen::Vector3d facet_xyz( prq_facet.trace().xyz() );
    CHECK_THAT( facet_xyz[0], Catch::Matchers::WithinAbs( xyz[0], tolerance_km));
    CHECK_THAT( facet_xyz[1], Catch::Matchers::WithinAbs( xyz[1], tolerance_km));
    CHECK_THAT( facet_xyz[2], Catch::Matchers::WithinAbs( xyz[2], tolerance_km));


}
