#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include "../private/BulletTracerModel.hpp"
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>

#include <cspice/SpiceUsr.h>

TEST_CASE( "Bullet Tracer Model - Default Constructor", "[default][bullet][tracer][model]") {
    psmrts::bullet::BulletTracerModel b_model;
    
    CHECK( b_model.tracer_model_name() == "bullet" );
    CHECK( b_model.shapefile()         == "Body-Fixed-Coordinate-System" );
    CHECK( b_model.plate_count()       == 0 );
    CHECK( b_model.vertex_count()      == 0 );
    CHECK( b_model.maximum_radius()    == 0 );
}


TEST_CASE( "Bullet Tracer Model - Ray Trace / Values Test", "[default][bullet][tracer][model][values]") {
    const double tolerance = 1.0e-6;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::PsmrtsShape b_shape{ objfile };    
    psmrts::bullet::BulletTracerModel b_model( b_shape.get_mesh(), objfile );

    std::string b_shapefile = psmrts::psmrts_filename( b_model.shapefile() );
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

    CHECK( facet.m_plateid == ray.plateid() );
    CHECK( facet.m_segment == ray.segment_number() );

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
