
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsPriorityTracer.hpp>
#include <PsmrtsTracerModel.hpp>

#include <PsmrtsUtilities.hpp>
#include <NaifDskTracerModel.hpp>
#include <DskKernelModel.hpp>
#include <EllipsoidTracerModel.hpp>
#include <BulletTracerModel.hpp>
#include <PsmrtsOBJFormat.hpp>


TEST_CASE("PsmrtsPriorityTracer Default Test", "[priority][tracer][default]") {

    psmrts::PsmrtsPriorityTracer test_tracer;
    std::vector<std::string> myString;

    CHECK( test_tracer.isValid()                == false );
    CHECK( test_tracer.size()                   == 0 );
    CHECK( test_tracer.find_model_by_name( "" ) == nullptr );
    CHECK( test_tracer.find_model_by_id( "" )   == nullptr );
    
    myString = test_tracer.get_shapefile_names();
    CHECK( myString.size() == 0 );
}


TEST_CASE( "PsmrtsPriorityTrace Values Test", "[priority][tracer][values]") {
    const double tolerance = 1.0e-12;
    psmrts::PsmrtsPriorityTracer p_tracer;

    // Bullet Tracer
    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::bullet::PsmrtsBulletWorldModel bt_world( psmrts::bullet::PsmrtsBulletMeshMap( psmrts::PsmrtsOBJFormat(objfile)), objfile);

    // NAIF Dsk Tracer
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );

    // Ellipse Tracer 
    naif::NaifEllipsoidShape n_ellipse(0.5, 0.5, 0.5);

    CHECK( p_tracer.size()    == 0 );
    CHECK( p_tracer.isValid() == false );

    p_tracer.add_tracer( std::shared_ptr<psmrts::PsmrtsTracerModel> ( new psmrts::bullet::BulletTracerModel (bt_world) ));
    p_tracer.add_tracer( std::shared_ptr<psmrts::PsmrtsTracerModel> ( new psmrts::NaifDskTracerModel (dskfile) ));

    CHECK( p_tracer.size()    == 2 );

    p_tracer.add_tracer( std::shared_ptr<psmrts::PsmrtsTracerModel> (new psmrts::EllipsoidTracerModel (n_ellipse) ));

    CHECK( p_tracer.size()    == 3 );
    CHECK( p_tracer.isValid() == true );

    CHECK( p_tracer.get_shapefile_names() == std::vector<std::string> {psmrts_shapes_path( "obj/data/bennu_20facets.obj" ), 
                                                                       psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" ),
                                                                       "TriaxialEllipsoid"} );
    CHECK( p_tracer.find_model_by_name( "TriaxialEllipsoid" )->maximum_radius() == 0.5 );
  
    // DSK Model Comparison Object (based on same file as in priority list)
    psmrts::NaifDskTracerModel dsk(dskfile);
    const std::string dsk_id = "naifdsk::DskKernelModel::" + psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    CHECK( p_tracer.find_model_by_id( dsk_id )->shape_tracer_id() == dsk.shape_tracer_id() );

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

    const psmrts::PsmrtsTracerModel* ray_result = p_tracer.ray_trace( obs, lookdir, ray );

    REQUIRE( ray_result            != nullptr );
    CHECK( ray_result->shapefile() == psmrts_shapes_path( "obj/data/bennu_20facets.obj" ) );

    p_tracer.clear();
    CHECK( p_tracer.size()    == 0 );
    CHECK( p_tracer.isValid() == false );
}
