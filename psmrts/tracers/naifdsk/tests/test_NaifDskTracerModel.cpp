#include <psmrts/core/tests/psmrts_catch2_environment.hpp> 

#include <NaifDskTracerModel.hpp>
#include <DskKernelModel.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>

/**
TEST_CASE("NAIF Dsk Tracer Model - Default Constructor", "[default][naifdsk][tracer][model]") {
    psmrts::NaifDskTracerModel d_model;

    CHECK( d_model.tracer_model_type() == "naifdsk" );
    CHECK( d_model.tracer_model_name() == "DskKernelModel" ); 
    CHECK( d_model.shape_tracer_id() == "naifdsk::DskKernelModel::none" );
    CHECK( d_model.shapefile() == "" );
    CHECK( d_model.plate_count() == 0 );
    CHECK( d_model.vertex_count() == 0 );
    CHECK_THROWS( d_model.maximum_radius() == 0 ); // ERROR - [naif::DskKernelModel] - Dsk Segment 0 does not exist

    psmrts::PsmrtsTracerModel *d2_model = d_model.clone();
    CHECK( d2_model != &d_model );
    CHECK( d2_model->tracer_model_name() == "DskKernelModel" );
    // Same error:  {Unknown expression after the reported line}
    // due to unexpected exception with message:
    // Invalid radii (0.000000,0.000000,0.000000 - must be > 0

    psmrts::PsmrtsTracerModel *e_model = d_model.ellipsoid();
    CHECK( e_model->tracer_model_name() == "ellipsoid" );
}
*/

TEST_CASE( "NAIF Dsk Tracer Model - Ray Trace / Values Test", "[default][bullet][tracer][model][values]") {
    const double tolerance = 1.0e-6;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );

    psmrts::NaifDskTracerModel d_model( dsk );
    psmrts::NaifDskTracerModel d_model_filepath( psmrts_tracers_path("naifdsk/data/bennu_20facets.bds") );

    std::string d_shape_id = psmrts::psmrts_file_basename( d_model.shape_tracer_id() );
    std::string d_shapefile = psmrts::psmrts_file_basename( d_model.shapefile() );
    CHECK( d_model.tracer_model_type() == "naifdsk" );
    CHECK( d_model.tracer_model_name() == "DskKernelModel" );
    CHECK( d_shape_id                  == "bennu_20facets.bds" );
    CHECK( d_shapefile                 == "bennu_20facets.bds" );
    CHECK( d_model.plate_count()       == 36 );
    CHECK( d_model.vertex_count()      == 20 ); 
    CHECK_THAT( d_model.maximum_radius(), Catch::Matchers::WithinAbs(0.22493886860043516, tolerance ));

    CHECK( d_model_filepath.tracer_model_type() == d_model.tracer_model_type() );
    CHECK( d_model_filepath.tracer_model_name() == d_model.tracer_model_name() );
    CHECK( d_model_filepath.shape_tracer_id()   == d_model.shape_tracer_id() );
    CHECK( d_model_filepath.shapefile()         == d_model.shapefile() );
    CHECK( d_model_filepath.plate_count()       == 36 );
    CHECK( d_model_filepath.vertex_count()      == 20 );
    CHECK_THAT( d_model_filepath.maximum_radius(), Catch::Matchers::WithinAbs(0.22493886860043516, tolerance ));
    
    Eigen::Vector3d obs;
    double obs_long = 90.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c( d_model.maximum_radius(), obs_long, obs_lat, obs.data() );
    obs = obs * 10;

    Eigen::Vector3d surf;
    double surf_lon = 90.0 * rpd_c();
    double surf_lat = 45.0 * rpd_c();
    latrec_c( d_model.maximum_radius(), surf_lon, surf_lat, surf.data() );

    Eigen::Vector3d lkdr = surf - obs;

    psmrts::PsmrtsRayTrace ray;

    CHECK( d_model.ray_trace( obs, lkdr, ray ) == true ); 
    CHECK( ray.hasHit() == true );

    psmrts::PsmrtsRayTrace::FacetDatum facet;

    CHECK( d_model.get_facet(ray, facet) == true );

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
    
    psmrts::PsmrtsTracerModel *d_clone   = d_model.clone();
    CHECK( d_clone != &d_model );
    CHECK( d_clone->tracer_model_name() == "DskKernelModel" );
    CHECK( d_clone->tracer_model_type() == "naifdsk" );

    psmrts::PsmrtsTracerModel *d_ellipse   = d_model.ellipsoid();
    CHECK( d_ellipse->tracer_model_name() == "NaifEllipsoid" );
    CHECK( d_ellipse->tracer_model_type() == "psmrts" );
    CHECK( d_ellipse->shape_tracer_id()   == "psmrts::NaifEllipsoid::ellipsoid" );
    

    CHECK( d_model.tracer().n_dsk_segments()       == 1 );
    CHECK( d_model.tracer().segment(0).surfaceid() == 2101955 );

    psmrts::NaifDskTracerModel *d_surfaceid = d_model.tracer_from_id( 2101955 );
    CHECK( d_surfaceid->tracer().isValid() == true );
}

