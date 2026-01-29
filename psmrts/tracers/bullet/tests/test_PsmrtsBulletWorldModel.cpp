#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsVector3.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>

#include "../private/BulletSystemModel.hpp"
#include "../private/PsmrtsBulletMeshMap.hpp"
#include "../private/PsmrtsBulletWorldModel.hpp"
#include "../private/BulletTracerModel.hpp"
#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>

#include <psmrts/tracers/naifdsk/private/NaifUtilities.hpp>
#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>

TEST_CASE ( "Bullet World Test - Default Constructor", "[default][bullet][world]" ) {
    psmrts::bullet::PsmrtsBulletWorldModel b_world;

    CHECK ( b_world.isValid() == false );
    CHECK ( b_world.name() == "Body-Fixed-Coordinate-System" );
}

// Load a mesh and inspect contents of Bullet world

// Load mesh and run some traces

TEST_CASE( "Bullet Tracer Test", "[bullet][tracer]" ) {
    const double tolerance_km = 1.0e-6;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::PsmrtsShape b_shape{ objfile };

    // This spec saves significant memory... and confirms Bullet preserves data
    psmrts::bullet::PsmrtsBulletWorldModel bt_world{ psmrts::bullet::PsmrtsBulletMeshMap ( b_shape.get_mesh(), objfile, 0 ), objfile };
    REQUIRE( bt_world.isValid() == true );

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
    psmrts::PsmrtsRayTrace raysurf;
    Eigen::Vector3d surf_obs = surf * 1.5;
    bool got_surf = bt_world.ray_trace( surf_obs, -surf_obs, raysurf );
    REQUIRE( got_surf == true );

    // Now compute expected/precise look vector from observer to surface intercept point
    Eigen::Vector3d lkdr = raysurf.xyz() - obs;

    // Trace it from observer to surface point to confirm
    psmrts::PsmrtsRayTrace spt;
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

    CHECK( bt_world.track_count()         > 0 );
    CHECK( bt_world.elapsed_life_time_s() >= 0.0 );
    
    psmrts::PsmrtsThreadSafeCounter count_t = bt_world.performance_snapshot();
    CHECK( count_t.count()     > 0 );
    CHECK( count_t.runtime_s() >= 0.0 );
}

TEST_CASE("Bullet-DSK Comparison Test", "[bullet][dsk][raytrace]") {
    auto tolerance = 1.0e-12;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );

    psmrts::PsmrtsShape b_shape{ objfile };
    psmrts::bullet::PsmrtsBulletMeshMap bt_data( b_shape.get_mesh(), objfile, 0 );
    psmrts::bullet::PsmrtsBulletWorldModel bt_world( bt_data, objfile );

    naif::DskKernelModel dsk( dskfile );

    CHECK ( bt_world.isValid() == true );
    CHECK ( dsk.isValid() == true );

    double radius( bt_data.maximum_radius() ); 

    Eigen::Vector3d obs;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 1.5;

    Eigen::Vector3d surf;
    double surf_lon = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_lon, surf_lat, surf.data() );

    psmrts::PsmrtsRayTrace raysurf;
    Eigen::Vector3d surf_obs = surf * 1.5;
    bt_world.ray_trace(surf_obs, -surf_obs, raysurf );

    Eigen::Vector3d lkdr = raysurf.xyz() - obs;

    psmrts::PsmrtsRayTrace bullet_spt;
    bt_world.ray_trace( obs, lkdr, bullet_spt ); 

    psmrts::PsmrtsRayTrace dsk_spt; 
    dsk.ray_trace( obs, lkdr, dsk_spt );

    CHECK ( bullet_spt.hasHit() == true );
    CHECK ( dsk_spt.hasHit()    == true );

    CHECK_THAT( bullet_spt.normal()(0), Catch::Matchers::WithinAbs( dsk_spt.normal()(0), tolerance ));
    CHECK_THAT( bullet_spt.normal()(1), Catch::Matchers::WithinAbs( dsk_spt.normal()(1), tolerance ));
    CHECK_THAT( bullet_spt.normal()(2), Catch::Matchers::WithinAbs( dsk_spt.normal()(2), tolerance ));

    CHECK_THAT (bullet_spt.xyz()(0), Catch::Matchers::WithinAbs( dsk_spt.xyz()(0), tolerance ));
    CHECK_THAT (bullet_spt.xyz()(1), Catch::Matchers::WithinAbs( dsk_spt.xyz()(1), tolerance ));
    CHECK_THAT (bullet_spt.xyz()(2), Catch::Matchers::WithinAbs( dsk_spt.xyz()(2), tolerance ));

    double bt_lat, bt_lon, bt_radius;
    reclat_c( bullet_spt.xyz().data(), &bt_radius, &bt_lon, &bt_lat );

    double dsk_lat, dsk_lon, dsk_radius;
    reclat_c( dsk_spt.xyz().data(), &dsk_radius, &dsk_lon, &dsk_lat );

    CHECK_THAT ( bt_lon, Catch::Matchers::WithinAbs( dsk_lon, tolerance ));
    CHECK_THAT ( bt_lat, Catch::Matchers::WithinAbs( dsk_lat, tolerance ));
    CHECK_THAT ( bt_radius, Catch::Matchers::WithinAbs( dsk_radius, tolerance ));

    // facets, indexes, vertices - as compares to the raytrace
    CHECK ( bullet_spt.plateid() == dsk_spt.plateid() ); // bt=30, dsk=31
    CHECK ( bt_data.nfacets() == dsk.n_total_plates() );
    CHECK ( bt_data.nvectors() == dsk.n_total_vertices() );

    auto bt_facet = bt_data.get_facet(30); // bt_data = NativeBulletMeshMap
    psmrts::PsmrtsRayTrace::FacetDatum dsk_facet;

    CHECK( dsk.get_facet(dsk_spt, dsk_facet) == true );

    CHECK( bt_facet.m_plateid == dsk_facet.m_plateid );

    CHECK( bt_facet.m_indexes == dsk_facet.m_indexes );
  
    CHECK_THAT ( bt_facet.m_vector1[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[0], tolerance ));
    CHECK_THAT ( bt_facet.m_vector1[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[1], tolerance ));
    CHECK_THAT ( bt_facet.m_vector1[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[2], tolerance ));

    CHECK_THAT ( bt_facet.m_vector2[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[0], tolerance ));
    CHECK_THAT ( bt_facet.m_vector2[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[1], tolerance ));
    CHECK_THAT ( bt_facet.m_vector2[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[2], tolerance ));

    CHECK_THAT ( bt_facet.m_vector3[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[0], tolerance ));
    CHECK_THAT ( bt_facet.m_vector3[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[1], tolerance ));
    CHECK_THAT ( bt_facet.m_vector3[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[2], tolerance ));

    CHECK_THAT ( bt_facet.m_normal[0], Catch::Matchers::WithinAbs( dsk_facet.m_normal[0], tolerance ));
    CHECK_THAT ( bt_facet.m_normal[1], Catch::Matchers::WithinAbs( dsk_facet.m_normal[1], tolerance ));
    CHECK_THAT ( bt_facet.m_normal[2], Catch::Matchers::WithinAbs( dsk_facet.m_normal[2], tolerance ));

    psmrts::PsmrtsRayTrace ray;

    // Generate loop
    naif::DskKernelModel::DskIndexDataModel dsk_facet_index = dsk.load_facet_indexes();
    naif::DskKernelModel::DskVectorDataModel dsk_facet_vector = dsk.load_facet_vectors();

    for(int i=0; i < dsk_facet_index.size(); i++) {
        ray.datum().m_hit = true; 
        ray.datum().m_plateid = i;
        ray.datum().m_segment = dsk_spt.datum().m_segment;

        psmrts::PsmrtsRayTrace::FacetDatum dsk_facet;
        CHECK( dsk.get_facet(ray, dsk_facet) == true );

        auto bt_facet = bt_data.get_facet( ray.plateid() );
        CHECK ( bt_facet.m_plateid == dsk_facet.m_plateid );
        CHECK ( bt_facet.m_indexes == dsk_facet.m_indexes );
        CHECK ( bt_facet.m_vector1 == dsk_facet.m_vector1 );
        CHECK ( bt_facet.m_vector2 == dsk_facet.m_vector2 );
        CHECK ( bt_facet.m_vector3 == dsk_facet.m_vector3 );
        CHECK ( bt_facet.m_normal == dsk_facet.m_normal );
    }

}


TEST_CASE("Bullet-DSK Comparison Test - float", "[bullet][dsk][raytrace][float]") {
    typedef psmrts::PsmrtsMeshData::PsmrtsDataType  PsmrtsDataType;

    auto tolerance = 1.0e-7;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );

    // Use direct loading of float data from OBJ file
    psmrts::PsmrtsOBJFormat t_loader{ objfile };
    psmrts::PsmrtsMeshData d_float_mesh( t_loader.get_indexes(), t_loader.get_float_vectors() );
    psmrts::bullet::PsmrtsBulletMeshMap bt_mesh( d_float_mesh, objfile, 0 );
    psmrts::bullet::PsmrtsBulletWorldModel bt_world( bt_mesh, objfile );
    

    naif::DskKernelModel dsk( dskfile );

    CHECK ( bt_world.isValid() == true );
    CHECK ( dsk.isValid() == true );

    double radius( bt_mesh.maximum_radius() ); 

    Eigen::Vector3d obs;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 1.5;

    Eigen::Vector3d surf;
    double surf_lon = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_lon, surf_lat, surf.data() );

    psmrts::PsmrtsRayTrace raysurf;
    Eigen::Vector3d surf_obs = surf * 1.5;
    bt_world.ray_trace(surf_obs, -surf_obs, raysurf );

    Eigen::Vector3d lkdr = raysurf.xyz() - obs;

    psmrts::PsmrtsRayTrace bullet_spt;
    bt_world.ray_trace( obs, lkdr, bullet_spt ); 

    psmrts::PsmrtsRayTrace dsk_spt; 
    dsk.ray_trace( obs, lkdr, dsk_spt );

    CHECK ( bullet_spt.hasHit() == true );
    CHECK ( dsk_spt.hasHit()    == true );

    CHECK_THAT( bullet_spt.normal()(0), Catch::Matchers::WithinAbs( dsk_spt.normal()(0), tolerance ));
    CHECK_THAT( bullet_spt.normal()(1), Catch::Matchers::WithinAbs( dsk_spt.normal()(1), tolerance ));
    CHECK_THAT( bullet_spt.normal()(2), Catch::Matchers::WithinAbs( dsk_spt.normal()(2), tolerance ));

    CHECK_THAT (bullet_spt.xyz()(0), Catch::Matchers::WithinAbs( dsk_spt.xyz()(0), tolerance ));
    CHECK_THAT (bullet_spt.xyz()(1), Catch::Matchers::WithinAbs( dsk_spt.xyz()(1), tolerance ));
    CHECK_THAT (bullet_spt.xyz()(2), Catch::Matchers::WithinAbs( dsk_spt.xyz()(2), tolerance ));

    double bt_lat, bt_lon, bt_radius;
    reclat_c( bullet_spt.xyz().data(), &bt_radius, &bt_lon, &bt_lat );

    double dsk_lat, dsk_lon, dsk_radius;
    reclat_c( dsk_spt.xyz().data(), &dsk_radius, &dsk_lon, &dsk_lat );

    CHECK_THAT ( bt_lon, Catch::Matchers::WithinAbs( dsk_lon, tolerance ));
    CHECK_THAT ( bt_lat, Catch::Matchers::WithinAbs( dsk_lat, tolerance ));
    CHECK_THAT ( bt_radius, Catch::Matchers::WithinAbs( dsk_radius, tolerance ));

    // facets, indexes, vertices - as compares to the raytrace
    CHECK ( bullet_spt.plateid() == dsk_spt.plateid() ); // bt=30, dsk=31
    CHECK ( t_loader.nIndexes() == dsk.n_total_plates() );
    CHECK ( t_loader.nVertexes() == dsk.n_total_vertices() );

    auto bt_facet = bt_mesh.get_facet(30); // bt_data = NativeBulletMeshMap
    psmrts::PsmrtsRayTrace::FacetDatum dsk_facet;

    CHECK( dsk.get_facet(dsk_spt, dsk_facet) == true );

    CHECK ( bt_facet.m_indexes == dsk_facet.m_indexes );
   
    CHECK_THAT ( bt_facet.m_vector1[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[0], tolerance ));
    CHECK_THAT ( bt_facet.m_vector1[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[1], tolerance ));
    CHECK_THAT ( bt_facet.m_vector1[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[2], tolerance ));

    CHECK_THAT ( bt_facet.m_vector2[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[0], tolerance ));
    CHECK_THAT ( bt_facet.m_vector2[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[1], tolerance ));
    CHECK_THAT ( bt_facet.m_vector2[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[2], tolerance ));

    CHECK_THAT ( bt_facet.m_vector3[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[0], tolerance ));
    CHECK_THAT ( bt_facet.m_vector3[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[1], tolerance ));
    CHECK_THAT ( bt_facet.m_vector3[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[2], tolerance ));

    CHECK_THAT ( bt_facet.m_normal[0], Catch::Matchers::WithinAbs( dsk_facet.m_normal[0], tolerance ));
    CHECK_THAT ( bt_facet.m_normal[1], Catch::Matchers::WithinAbs( dsk_facet.m_normal[1], tolerance ));
    CHECK_THAT ( bt_facet.m_normal[2], Catch::Matchers::WithinAbs( dsk_facet.m_normal[2], tolerance ));

    psmrts::PsmrtsRayTrace ray;

    // Generate loop
    naif::DskKernelModel::DskIndexDataModel dsk_facet_index   = dsk.load_facet_indexes();
    naif::DskKernelModel::DskVectorDataModel dsk_facet_vector = dsk.load_facet_vectors();

    for(int i=0; i < dsk_facet_index.size(); i++) {
        ray.datum().m_hit = true; 
        ray.datum().m_plateid = i;
        ray.datum().m_segment = dsk_spt.datum().m_segment;

        psmrts::PsmrtsRayTrace::FacetDatum dsk_facet;
        CHECK( dsk.get_facet(ray, dsk_facet) == true );

        auto bt_facet = bt_mesh.get_facet( ray.plateid() );

        CHECK ( bt_facet.m_indexes == dsk_facet.m_indexes );

        CHECK_THAT ( bt_facet.m_vector1[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[0], tolerance ));
        CHECK_THAT ( bt_facet.m_vector1[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[1], tolerance ));
        CHECK_THAT ( bt_facet.m_vector1[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector1[2], tolerance ));

        CHECK_THAT ( bt_facet.m_vector2[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[0], tolerance ));
        CHECK_THAT ( bt_facet.m_vector2[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[1], tolerance ));
        CHECK_THAT ( bt_facet.m_vector2[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector2[2], tolerance ));

        CHECK_THAT ( bt_facet.m_vector3[0], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[0], tolerance ));
        CHECK_THAT ( bt_facet.m_vector3[1], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[1], tolerance ));
        CHECK_THAT ( bt_facet.m_vector3[2], Catch::Matchers::WithinAbs( dsk_facet.m_vector3[2], tolerance ));

        CHECK_THAT ( bt_facet.m_normal[0], Catch::Matchers::WithinAbs( dsk_facet.m_normal[0], tolerance ));
        CHECK_THAT ( bt_facet.m_normal[1], Catch::Matchers::WithinAbs( dsk_facet.m_normal[1], tolerance ));
        CHECK_THAT ( bt_facet.m_normal[2], Catch::Matchers::WithinAbs( dsk_facet.m_normal[2], tolerance ));
    }

}

TEST_CASE( "BulletTracerModel Test", "[bullet][tracer][model]" ) {
    const double tolerance_km = 1.0e-6;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );

    psmrts::PsmrtsShape b_shape{ objfile };

    // This spec saves significant memory... and confirms Bullet preserves data
    psmrts::bullet::PsmrtsBulletWorldModel bt_world_model{ psmrts::bullet::PsmrtsBulletMeshMap ( b_shape.get_mesh(), objfile, 0 ), objfile };
    REQUIRE( bt_world_model.isValid() == true );

    // Now create the tracer model
    psmrts::bullet::BulletTracerModel bt_world( bt_world_model );

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
    psmrts::PsmrtsRayTrace raysurf;
    Eigen::Vector3d surf_obs = surf * 1.5;
    bool got_surf = bt_world.ray_trace( surf_obs, -surf_obs, raysurf );
    REQUIRE( got_surf == true );

    // Now compute expected/precise look vector from observer to surface intercept point
    Eigen::Vector3d lkdr = raysurf.xyz() - obs;

    // Trace it from observer to surface point to confirm
    psmrts::PsmrtsRayTrace spt;
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

TEST_CASE( "BulletTracerModel Callback Test", "[bullet][tracer][callback]" ) {
  const double tolerance_km = 1.0e-6;

  // Tetst default construction
  psmrts::bullet::PsmrtsBulletClosestRayCallback callback_none;
  CHECK( callback_none.isValid()  == false );
  CHECK( callback_none.hasHit()   == false );
  CHECK( callback_none.fraction() == 1.0 );
  CHECK( callback_none.triangleIndex() == -1 );
  CHECK( callback_none.partId() == -1 );

  // Create a shape a test a trace operation with callbacks
  std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
  psmrts::PsmrtsShape b_shape{ objfile };

  // This spec saves significant memory... and confirms Bullet preserves data
  psmrts::bullet::PsmrtsBulletWorldModel bt_world_model{ psmrts::bullet::PsmrtsBulletMeshMap ( b_shape.get_mesh(), objfile, 0 ), objfile };
  REQUIRE( bt_world_model.isValid() == true );

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
  Eigen::Vector3d observer = surf * 1.5;
  Eigen::Vector3d lookdir = -observer;

  psmrts::bullet::PsmrtsBulletClosestRayCallback callback_t( observer, lookdir );
  CHECK( callback_t.toStdVector( callback_t.observer() ) == observer );
  CHECK( callback_t.toStdVector( callback_t.lookdir() )  == lookdir );

  psmrts::PsmrtsRayTrace ray_t;
  psmrts::bullet::PsmrtsBulletClosestRayCallback results;
  bool gothit = bt_world_model.bullet_ray_trace( observer, lookdir, results );
  CHECK( bt_world_model.extract_ray_trace_results( results, ray_t ) == true );
  CHECK( results.isValid() == true );   
  CHECK( results.hasHit()  == ray_t.hasHit() ); 
  CHECK_THAT( results.fraction(), Catch::Matchers::WithinAbs(0.41580917311320742, tolerance_km ));
  CHECK( callback_t.toStdVector( results.point() )  == ray_t.xyz() );
  CHECK( callback_t.toStdVector( results.normal() ) == ray_t.normal() );

  psmrts::bullet::PsmrtsBulletClosestRayCallback copy_t( results, 
                                                         results.point(),
                                                         results.normal() );
  CHECK( copy_t.isValid() == true);
  CHECK( copy_t.hasHit()  == true );
}
