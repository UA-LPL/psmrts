
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
    
    psmrts::PsmrtsPriorityTracer p_tracer;

    // Bullet Tracer
    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    psmrts::bullet::PsmrtsBulletWorldModel bt_world( psmrts::bullet::PsmrtsBulletMeshMap( psmrts::PsmrtsOBJFormat(objfile)), objfile);

    // NAIF Tracer
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );

    // Ellipse Tracer 
    naif::NaifEllipsoidShape n_ellipse(0.5, 0.5, 0.5);
   

    CHECK( p_tracer.size() == 0 );
    CHECK( p_tracer.isValid() == false );
    p_tracer.add_tracer( std::shared_ptr<psmrts::PsmrtsTracerModel> ( new psmrts::bullet::BulletTracerModel (bt_world) ));
    p_tracer.add_tracer( std::shared_ptr<psmrts::PsmrtsTracerModel> ( new psmrts::NaifDskTracerModel(dskfile) ));
    CHECK( p_tracer.size() == 2 );
    p_tracer.add_tracer( std::shared_ptr<psmrts::PsmrtsTracerModel> (new psmrts::EllipsoidTracerModel (n_ellipse) ));
    CHECK( p_tracer.size() == 3 );
    CHECK( p_tracer.isValid() == true );

    CHECK( p_tracer.get_shapefile_names() == std::vector<std::string> {psmrts_formats_path( "obj/data/bennu_20facets.obj" ), 
                                                                       psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" ),
                                                                       "TriaxialEllipsoid"} );
    CHECK( p_tracer.find_model_by_name( "TriaxialEllipsoid" )->maximum_radius() == 0.5 );
  
    //CHECK( p_tracer.find_model_by_id( d_model.shape_tracer_id() ) == &d_model );
    // NEEDS RAY TRACE TESTING
}
