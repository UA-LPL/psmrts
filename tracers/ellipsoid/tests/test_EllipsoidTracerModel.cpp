
#include <psmrts_catch2_environment.hpp>

#include <EllipsoidTracerModel.hpp>

TEST_CASE( "EllipsoidTracerModel Default Test", "[ellipsoid][default]") {
    
    psmrts::EllipsoidTracerModel e_shape;

    // No Parameter Initialization
    CHECK ( e_shape.tracer_model_type() == "psmrts" );
    CHECK ( e_shape.tracer_model_name() == "NaifEllipsoid" );
    CHECK ( e_shape.shape_tracer_id() == "psmrts::NaifEllipsoid::UnitSpheroid" );

    CHECK ( e_shape.shapefile() == "UnitSpheroid" );

    CHECK ( e_shape.plate_count() == 0 );
    CHECK ( e_shape.vertex_count() == 0 );
    CHECK ( e_shape.maximum_radius() == 1 );  // Naif auto-initializes this max to 1.0

    // Creating Tracer Model from Naif Ellipsoid
    naif::NaifEllipsoidShape naif_ellipse( 1.0, 2.0, 3.0 );

    psmrts::EllipsoidTracerModel etm_ellipse ( naif_ellipse );

    CHECK ( etm_ellipse.tracer_model_type() == "psmrts" );
    CHECK ( etm_ellipse.tracer_model_name() == "NaifEllipsoid" );
    CHECK ( etm_ellipse.shape_tracer_id() == "psmrts::NaifEllipsoid::TriaxialEllipsoid" );

    CHECK ( etm_ellipse.shapefile() == "TriaxialEllipsoid" );

    CHECK ( etm_ellipse.plate_count() == 0 );
    CHECK ( etm_ellipse.vertex_count() == 0 );
    CHECK ( etm_ellipse.maximum_radius() == naif_ellipse.c() );

    // Creating Tracer Model from Eigen Radii
    Eigen::Vector3d e_radii( {1.0, 1.0, 2.0 } );

    psmrts::EllipsoidTracerModel etm_radii_ellipse ( e_radii, "my_sphere" );

    CHECK ( etm_radii_ellipse.tracer_model_type() == "psmrts" );
    CHECK ( etm_radii_ellipse.tracer_model_name() == "NaifEllipsoid" );
    CHECK ( etm_radii_ellipse.shape_tracer_id() == "psmrts::NaifEllipsoid::my_sphere" );

    CHECK ( etm_radii_ellipse.shapefile() == "my_sphere" ); 

    CHECK ( etm_radii_ellipse.plate_count() == 0 );
    CHECK ( etm_radii_ellipse.vertex_count() == 0 );
    CHECK ( etm_radii_ellipse.maximum_radius() == 2.0 );

}

TEST_CASE ("EllipsoidTracerModel Basic Values / RayTrace Test", "[tracer][ellipsoid][raytrace]") {

    const double tolerance = 1.0e-6;

    naif::NaifEllipsoidShape naif_ellipse(1.0, 2.0, 3.0);

    psmrts::EllipsoidTracerModel etm_ellipse ( naif_ellipse );

    Eigen::Vector3d observer;
    double radius = etm_ellipse.maximum_radius();
    double obs_long = psmrts::degrees_to_radians(45.0); 
    double obs_lat = psmrts::degrees_to_radians(45.0);
    latrec_c ( radius, obs_long, obs_lat, observer.data() );
    observer = observer * 2.0;

    Eigen::Vector3d surf;
    double surf_long = psmrts::degrees_to_radians(45.0);
    double surf_lat = psmrts::degrees_to_radians(45.0); 
    latrec_c ( radius, surf_long, surf_lat, surf.data() );

    Eigen::Vector3d lkdr = observer - surf;

    psmrts::PsmrtsRayTrace naif_ray;
    psmrts::PsmrtsRayTrace etm_ray;

    CHECK ( naif_ellipse.ray_trace( -observer, lkdr, naif_ray ) == true );
    CHECK ( etm_ellipse.ray_trace( -observer, lkdr, etm_ray ) == true );
    
    psmrts::PsmrtsRayTrace::FacetDatum naif_facet;
    psmrts::PsmrtsRayTrace::FacetDatum etm_facet;

    // Facets not meant to exist on shape models
    CHECK ( naif_ellipse.get_facet( naif_ray, naif_facet ) == false ); 
    CHECK ( etm_ellipse.get_facet( etm_ray, etm_facet ) == false );

    CHECK ( etm_facet.m_indexes == naif_facet.m_indexes ); // -1, -1, -1
    CHECK ( etm_facet.m_vector1 == naif_facet.m_vector1 ); // 0, 0, 0
    CHECK ( etm_facet.m_vector2 == naif_facet.m_vector2 ); // 0, 0, 0
    CHECK ( etm_facet.m_vector3 == naif_facet.m_vector3 ); // 0, 0, 0
    CHECK ( etm_facet.m_normal == naif_facet.m_normal ); // 0, 0, 0

    psmrts::PsmrtsTracerModel *etm_clone = etm_ellipse.clone(); // creates a new obj in memory, with same data
    // const psmrts::EllipsoidTracerModel &etm_ellipse_r = etm_ellipse;
    // CHECK( &etm_ellipse == &etm_ellipse_r ); - Basically same, they point to the same thing in memory

    CHECK ( etm_clone != &etm_ellipse );
    CHECK ( etm_clone->plate_count() == etm_ellipse.plate_count() );
    CHECK ( etm_clone->tracer_model_type() == etm_ellipse.tracer_model_type() );
    CHECK ( etm_clone->tracer_model_name() == etm_ellipse.tracer_model_name() );
    CHECK ( etm_clone->shape_tracer_id() == etm_ellipse.shape_tracer_id() );
    CHECK ( etm_clone->shapefile() == etm_ellipse.shapefile() );
    CHECK ( etm_clone->plate_count() == etm_ellipse.plate_count() );
    CHECK ( etm_clone->vertex_count() == etm_ellipse.vertex_count() );
    CHECK ( etm_clone->maximum_radius() == etm_ellipse.maximum_radius() );
}