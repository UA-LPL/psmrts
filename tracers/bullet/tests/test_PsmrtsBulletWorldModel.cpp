
#include <psmrts_catch2_environment.hpp>

#include <BulletSystemModel.hpp>
#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>
#include <PsmrtsOBJAsset.hpp>
#include <PsmrtsBulletMeshMap.hpp>
#include <PsmrtsBulletWorldModel.hpp>

#include <NaifUtilities.hpp>
#include <DskKernelModel.hpp>

TEST_CASE ( "Bullet World Test - Default Constructor", "[default][bullet][world]" ) {
    psmrts::bullet::PsmrtsBulletWorldModel b_world;

    CHECK ( b_world.isValid() == false );
    CHECK ( b_world.name() == "Body-Fixed-Coordinate-System" );
}

// Load a mesh and inspect contents of Bullet world

// Load mesh and run some traces

TEST_CASE( "Bullet Shape Tracer Test", "[bullet][shape][tracer]" ) {
    const double tolerance_km = 1.0e-6;

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    psmrts::PsmrtsOBJAsset t_loader( objfile );
    psmrts::bullet::NativeBulletMeshMap  bt_data( t_loader );
    psmrts::bullet::PsmrtsBulletWorldModel bt_world( bt_data.create_collision_shape(), objfile );
    REQUIRE( bt_world.isValid() == true );

    CHECK( t_loader.nIndexes()  == 36 );
    CHECK( t_loader.nVertexes() == 20 );

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
    psmrts::RayTrace raysurf;
    Eigen::Vector3d surf_obs = surf * 1.5;
    bool got_surf = bt_world.ray_trace( surf_obs, -surf_obs, raysurf );
    REQUIRE( got_surf == true );

    // Now compute expected/precise look vector from observer to surface intercept point
    Eigen::Vector3d lkdr = raysurf.xyz() - obs;

    // Trace it from observer to surface point to confirm
    psmrts::RayTrace spt;
    bool good = bt_world.ray_trace( obs, lkdr, spt );
    
    Eigen::Vector3d normal = spt.normal();
    Eigen::Vector3d xyz = spt.xyz();

    // Compare expected results!
    CHECK( good == true );
    CHECK( good == spt.hasHit() );

    CHECK( spt.plateid()        == 30 );
    CHECK( spt.segment_number() == 0 );

    CHECK_THAT( normal[0], Catch::Matchers::WithinAbs(0.0,                tolerance_km ));
    CHECK_THAT( normal[1], Catch::Matchers::WithinAbs(0.5257310881115882, tolerance_km ));
    CHECK_THAT( normal[2], Catch::Matchers::WithinAbs(0.85065082318951801, tolerance_km ));

    // Compute radius/lon/lat from intercept surface point (body-fixed)
    double bt_lat, bt_lon, bt_radius;
    reclat_c( xyz.data(), &bt_radius, &bt_lon, &bt_lat);
    
    CHECK_THAT( bt_lon,    Catch::Matchers::WithinAbs( 45.0 * rpd_c(), tolerance_km ));    
    CHECK_THAT( bt_lat,    Catch::Matchers::WithinAbs( 50.0 * rpd_c(), tolerance_km ));  

    CHECK_THAT( bt_radius, Catch::Matchers::WithinAbs( spt.radius(), tolerance_km ));    
    CHECK_THAT( bt_lon,    Catch::Matchers::WithinAbs( surf_lon,     tolerance_km ));    
    CHECK_THAT( bt_lat,    Catch::Matchers::WithinAbs( surf_lat,     tolerance_km ));    

    CHECK_THAT( xyz[0], Catch::Matchers::WithinAbs( raysurf.xyz()[0], tolerance_km ) );
    CHECK_THAT( xyz[1], Catch::Matchers::WithinAbs( raysurf.xyz()[1], tolerance_km ) );
    CHECK_THAT( xyz[2], Catch::Matchers::WithinAbs( raysurf.xyz()[2], tolerance_km ) );
}