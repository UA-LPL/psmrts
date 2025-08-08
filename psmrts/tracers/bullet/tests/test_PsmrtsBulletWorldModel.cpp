#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <BulletSystemModel.hpp>
#include <psmrts/core/PsmrtsVector3.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <PsmrtsOBJFormat.hpp>
#include <PsmrtsBulletMeshMap.hpp>
#include <PsmrtsBulletWorldModel.hpp>
#include <BulletTracerModel.hpp>

#include <NaifUtilities.hpp>
#include <DskKernelModel.hpp>

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


    // psmrts::PsmrtsOBJFormat t_loader( objfile );
    // psmrts::bullet::PsmrtsBulletMeshMap  bt_data( t_loader ) );
    // psmrts::bullet::PsmrtsBulletWorldModel bt_world( bt_data, objfile );

    // This spec saves significant memory... and confirms Bullet preserves data
    psmrts::bullet::PsmrtsBulletWorldModel bt_world( psmrts::bullet::PsmrtsBulletMeshMap ( psmrts::PsmrtsOBJFormat( objfile ) ), objfile );
    REQUIRE( bt_world.isValid() == true );

    // CHECK( t_loader.nIndexes()  == 36 );
    // CHECK( t_loader.nVertexes() == 20 );

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

TEST_CASE("Bullet-DSK Comparison Test", "[bullet][dsk][raytrace]") {
    auto tolerance = 1.0e-12;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );

    psmrts::PsmrtsOBJFormat t_loader( objfile );
    psmrts::bullet::PsmrtsBulletMeshMap bt_data( t_loader );
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
    CHECK ( t_loader.nIndexes() == dsk.n_total_plates() );
    CHECK ( t_loader.nVertexes() == dsk.n_total_vertices() );

    auto bt_facet = bt_data.get_facet(30); // bt_data = NativeBulletMeshMap
    psmrts::PsmrtsRayTrace::FacetDatum dsk_facet;

    CHECK( dsk.get_facet(dsk_spt, dsk_facet) == true );

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

    psmrts::PsmrtsOBJFormat t_loader( objfile );
    
    PsmrtsDataType float_type = psmrts::PsmrtsMeshData::PsmrtsFloat;
    psmrts::bullet::PsmrtsBulletMeshMap bt_mesh( t_loader, float_type );
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

    // This spec saves significant memory... and confirms Bullet preserves data
    psmrts::bullet::PsmrtsBulletWorldModel bt_world_model( psmrts::bullet::PsmrtsBulletMeshMap ( psmrts::PsmrtsOBJFormat( objfile ) ), objfile );
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