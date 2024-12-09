#include <psmrts_catch2_environment.hpp>

#include <BulletTracerModel.hpp>
#include <PsmrtsOBJFormat.hpp>
#include <PsmrtsUtilities.hpp>

TEST_CASE( "Bullet Tracer Model - Default Constructor", "[default][bullet][tracer][model]") {
    psmrts::bullet::BulletTracerModel b_model;
    
    CHECK( b_model.tracer_model_name() == "bullet" );
    CHECK( b_model.shapefile()         == "Body-Fixed-Coordinate-System" );
    CHECK( b_model.plate_count()       == 0 );
    CHECK( b_model.vertex_count()      == 0 );
    CHECK( b_model.maximum_radius()    == 0 );

    psmrts::PsmrtsTracerModel *b2_model   = b_model.clone();
    CHECK( b2_model                      != &b_model );
    CHECK( b2_model->tracer_model_name() == "bullet" );
    // Interesting error here: 
    // "due to unexpected exception with message:
    // Invalid radii (0.000000,0.000000,0.000000 - must be > 0" - missing that right paren, it seems

    psmrts::PsmrtsTracerModel *e_model   = b_model.ellipsoid();
    CHECK( e_model->tracer_model_name() == "ellipsoid" );
}

TEST_CASE( "Bullet Tracer Model - Ray Trace / Values Test", "[default][bullet][tracer][model][values]") {
    const double tolerance = 1.0e-6;

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    psmrts::bullet::PsmrtsBulletWorldModel bt_world( psmrts::bullet::PsmrtsBulletMeshMap( psmrts::PsmrtsOBJFormat(objfile)), objfile);
    
    psmrts::bullet::BulletTracerModel b_model( bt_world );

    std::string b_shapefile = psmrts::psmrts_file_basename( b_model.shapefile() );
    CHECK( b_model.tracer_model_name()  == "bullet" );
    CHECK( b_shapefile                  == "bennu_20facets.obj" );
    CHECK( b_model.plate_count()        == 36 );
    CHECK( b_model.vertex_count()       == 20 ); 
    CHECK_THAT( b_model.maximum_radius(), Catch::Matchers::WithinAbs( 0.283065, tolerance ));

    Eigen::Vector3d obs;
    double radius = 1.0;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;

    Eigen::Vector3d surf;
    double surf_lon = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_lon, surf_lat, surf.data() );

    Eigen::Vector3d lookdir = surf - obs;

    psmrts::PsmrtsRayTrace ray;
    
    CHECK( b_model.ray_trace(obs, lookdir, ray) == true );
    CHECK( ray.hasHit() == true );

    psmrts::PsmrtsRayTrace::FacetDatum facet;

    CHECK( b_model.get_facet(ray, facet) == true );

    CHECK( facet.isValid() == true );
    CHECK( facet.m_indexes == Eigen::Vector3i { 12, 11, 5 } );

    Eigen::Vector3d vector1  = facet.m_vector1;
    Eigen::Vector3d vector2  = facet.m_vector2;
    Eigen::Vector3d vector3  = facet.m_vector3;
    Eigen::Vector3d f_normal = facet.m_normal;

    CHECK_THAT( vector1[0], Catch::Matchers::WithinAbs(-0.101004, tolerance) );
    CHECK_THAT( vector1[1], Catch::Matchers::WithinAbs( 0.0,      tolerance) );
    CHECK_THAT( vector1[2], Catch::Matchers::WithinAbs( 0.264431, tolerance) );

    CHECK_THAT( vector2[0], Catch::Matchers::WithinAbs( 0.101004, tolerance) );
    CHECK_THAT( vector2[1], Catch::Matchers::WithinAbs( 0.0,      tolerance) );
    CHECK_THAT( vector2[2], Catch::Matchers::WithinAbs( 0.264431, tolerance) );

    CHECK_THAT( vector3[0], Catch::Matchers::WithinAbs( 0.0,      tolerance) );
    CHECK_THAT( vector3[1], Catch::Matchers::WithinAbs( 0.264431, tolerance) );
    CHECK_THAT( vector3[2], Catch::Matchers::WithinAbs( 0.101004, tolerance) );

    CHECK_THAT( f_normal[0], Catch::Matchers::WithinAbs( 0.0, tolerance) );
    CHECK_THAT( f_normal[1], Catch::Matchers::WithinAbs( 0.525731, tolerance) );
    CHECK_THAT( f_normal[2], Catch::Matchers::WithinAbs( 0.850651, tolerance) );
}
