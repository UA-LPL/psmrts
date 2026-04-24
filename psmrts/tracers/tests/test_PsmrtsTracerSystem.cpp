#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>

#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/PsmrtsTracerSystem.hpp>
#include <psmrts/tracers/PsmrtsPriorityTracer.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>
#include <psmrts/tracers/naifdsk/NaifDskTracer.hpp>
#include <psmrts/shapes/obj/ObjShape.hpp>

#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>

TEST_CASE("PsmrtsTracerSystem Default Test", "[tracer][system][default]") {
    psmrts::PsmrtsTracerSystem sys1;
    CHECK( sys1.get_ellipsoid_tracer().isValid() == false ); 
    CHECK( sys1.get_shape_tracer().isValid()     == false ); 
    CHECK( sys1.get_shape_tracer().size()        == 0 );

    psmrts::PsmrtsTracerSystem sys2("test");
    CHECK( sys2.name() == "test" );
    CHECK( sys2.get_ellipsoid_tracer().isValid() == false ); 
    CHECK( sys2.get_shape_tracer().isValid()     == false ); 
    CHECK( sys2.get_shape_tracer().size()        == 0 );

    std::vector<std::string> bad_list{"bad/path"};
    CHECK_THROWS( psmrts::PsmrtsTracerSystem("test2", bad_list) );
}   


TEST_CASE("PsmrtsTracerSytem Values Test", "[tracer][system][values]") {
    psmrts::PsmrtsTracerSystem sys1("test");

    std::vector<double> radii = { 1.0, 2.0, 3.0 };
    bool result = sys1.set_reference_ellipsoid("TestRadii", radii);

    REQUIRE( result == true );
    REQUIRE( sys1.get_ellipsoid_tracer().isValid() );

    std::vector<double> obs = { 100.0, 0.0, 0.0 };
    std::vector<double> lkdr = { -1.0, 0.0, 0.0 };

    psmrts::PRQRayTrace ray = sys1.ellipsoid_trace(obs, lkdr);
    CHECK( ray.hasHit() == true );
    CHECK( ray.trace().radius() > 0.0 );
    CHECK( ray.trace().normal().norm() > 0.0 );

    Eigen::Vector3d e_obs = { 100.0, 0.0, 0.0 };
    Eigen::Vector3d e_lkdr = { -1.0, 0.0, 0.0 };
    psmrts::PRQRayTrace e_ray = sys1.ellipsoid_trace(e_obs, e_lkdr);
    CHECK( e_ray.hasHit() == true );
    CHECK( e_ray.trace().radius() > 0.0 );
    CHECK( e_ray.trace().normal().norm() > 0.0 );

    std::string file = psmrts_tracers_path("naifdsk/data/bennu_20facets.bds");
    CHECK(sys1.add_product("dsk_file", file, "naifdsk") == true );

    std::vector<double> sunpos = { 0.0, 1000.0, 0.0 };
    psmrts::PRQPhotometricTrace p_ray = sys1.ellipsoid_photometric_trace(obs, lkdr, sunpos);
    CHECK( p_ray.observer_trace().hasHit() == true );

    psmrts::PsmrtsTracer naifdsk_t ( psmrts::PsmrtsTracer::naifdsk( file ) );
    bool naif_add = sys1.add_tracer( naifdsk_t ); 
    CHECK( naif_add == true );                    

    sys1.create_priority_tracer("test1");

    psmrts::PsmrtsTracer e_tracer = psmrts::PsmrtsTracer::ellipsoid( { 0.283065,0.271215,0.249720 }, "Bennu" );
    psmrts::PsmrtsTracerSystem sys2("test2");
    sys2.set_reference_ellipsoid( e_tracer );

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    CHECK( sys2.get_ellipsoid_tracer().isValid() == true );
    // shape didnt work as first parameter here
    // said it expected a mesh
    CHECK( sys2.add_product("obj_file", objfile, "bullet") == true );

    sys2.create_priority_tracer("test2");

    bool naif_check = false;
    bool bullet_check = false;
    
    for (const auto &[uid, tracer] : sys1.inventory().tracers().cache() ) {
        if (tracer.config().contains("tracer") ) {
            std::string type = tracer.config().find("tracer").to_string();
            if (type == "naifdsk") {
                naif_check = true;
            }
            if (type == "bullet") {
                bullet_check = true;
            }
        }
    }

    CHECK( naif_check == true ); 
    CHECK( bullet_check == false );

    for (const auto &[uid, tracer] : sys2.inventory().tracers().cache() ) {
        if (tracer.config().contains("tracer") ) {
            std::string type = tracer.config().find("tracer").to_string();
            if (type == "bullet") {
                bullet_check = true;
            }
        }
    }
    CHECK( bullet_check == true ); 

    psmrts::PsmrtsShape obj_shape( objfile );
    CHECK( obj_shape.isValid() == true );
    CHECK( sys1.add_shape( obj_shape ) == true );

    bool obj_check = false;
    for (const auto &[uid, shape] : sys1.inventory().shapes().cache()) {
        if ( shape.config().contains("shape") ) {
            if ( shape.config().find("shape").to_string() == "obj" ) {
                obj_check = true;
            }
        }
    }
    CHECK( obj_check == true );

}

TEST_CASE("PsmrtsTracerSystem Priority Tracer Test", "[tracer][system][priority]") {
    psmrts::PsmrtsTracerSystem sys("p_tracers");

    std::string file = psmrts_tracers_path("naifdsk/data/bennu_20facets.bds");
    bool added = sys.add_product("dsk_file", file);
    
    CHECK( sys.error_count()  == 0 );
    if ( sys.error_count() > 0 ) sys.throw_errors();
    CHECK( added == true ); 

    psmrts::PsmrtsPriorityTracer pt = sys.create_priority_tracer();
    CHECK( pt.isValid() == true );

    CHECK( sys.get_ellipsoid_tracer().isValid() == true );

    std::vector<double> obs  = { 100.0, 0.0, 0.0 };
    std::vector<double> lkdr = {  -1.0, 0.0, 0.0 };

    psmrts::PRQRayTrace ray = sys.shape_trace( obs, lkdr );
    CHECK( ray.hasHit() == true );
    CHECK( ray.trace().radius() > 0.0 );
    CHECK( ray.trace().normal().norm() > 0.0 );

    psmrts::PsmrtsTracer hit_tracer = sys.get_tracer_from_intercept( ray );
    CHECK( hit_tracer.isValid() == true );

    std::vector<double> sunpos = { 0.0, 100.0, 0.0 };
    psmrts::PRQPhotometricTrace p_ray = sys.shape_photometric_trace( obs, lkdr, sunpos );
    CHECK( p_ray.observer_trace().hasHit() == true );
    CHECK( p_ray.isValid() == true );
}

TEST_CASE("PsmrtsTracerSystem Shapes Test", "[tracer][system][shapes]") {
    std::string objfile = psmrts_shapes_path("obj/data/bennu_20facets.obj");
    std::string bdsfile = psmrts_tracers_path("naifdsk/data/bennu_20facets.bds");

    std::vector<double> obs    = { 100.0, 0.0, 0.0 };
    std::vector<double> lkdr   = {  -1.0, 0.0, 0.0 };
    std::vector<double> sunpos = {   0.0, 100.0, 0.0 };
    std::vector<double> away   = {   1.0, 0.0, 0.0 };
    Eigen::Vector3d e_obs      = { 100.0, 0.0, 0.0 };
    Eigen::Vector3d e_lkdr     = {  -1.0, 0.0, 0.0 };
    Eigen::Vector3d e_sunpos   = {   0.0, 100.0, 0.0 };

 
    // process_shape_list() - valid list and :: syntax
    /** 
    psmrts::PsmrtsTracerSystem sys_shapelist("test_shapelist", 
                                              std::vector<std::string>{ objfile });
    CHECK( sys_shapelist.get_shape_tracer().isValid() == true );
    CHECK( sys_shapelist.get_shape_tracer().size()    == 1 );

    psmrts::PsmrtsTracerSystem sys_bullet_syntax("test_bullet_syntax", 
                                                  std::vector<std::string>{ "bullet::" + objfile });
    CHECK( sys_bullet_syntax.get_shape_tracer().isValid() == true );
    CHECK( sys_bullet_syntax.get_shape_tracer().size()    == 1 );
    */


    // Shared bullet system for shape_trace, photometric, process,
    // and get_tracer_from_intercept tests
    psmrts::PsmrtsTracerSystem sys_bullet("test_bullet");
    sys_bullet.add_product("obj_file", objfile, "bullet");
    sys_bullet.create_priority_tracer("test_bullet");

    // shape_trace() PRQRayTrace overload
    psmrts::PRQRayTrace prq_ray( Eigen::Vector3d(obs.data()), 
                                 Eigen::Vector3d(lkdr.data()) );
    CHECK( sys_bullet.shape_trace( prq_ray ) == true );
    CHECK( prq_ray.hasHit() == true );

    // shape_trace() Eigen overload (non-const refs required)
    psmrts::PRQRayTrace eigen_ray = sys_bullet.shape_trace( e_obs, e_lkdr );
    CHECK( eigen_ray.hasHit() == true );

    // shape_photometric_trace() PRQPhotometricTrace overload
    psmrts::PRQPhotometricTrace prq_photo( Eigen::Vector3d(obs.data()),
                                           Eigen::Vector3d(lkdr.data()),
                                           Eigen::Vector3d(sunpos.data()) );
    CHECK( sys_bullet.shape_photometric_trace( prq_photo ) == true );
    CHECK( prq_photo.observer_trace().hasHit() == true );

    // shape_photometric_trace() Eigen overload
    psmrts::PRQPhotometricTrace eigen_photo = sys_bullet.shape_photometric_trace(e_obs, e_lkdr, e_sunpos);
    CHECK( eigen_photo.observer_trace().hasHit() == true );

    // process() template method
    psmrts::PRQRayTrace process_ray( Eigen::Vector3d(obs.data()), 
                                     Eigen::Vector3d(lkdr.data()) );
    CHECK( sys_bullet.process( process_ray ) == true );
    CHECK( process_ray.hasHit() == true );

    // get_tracer_from_intercept() - hit case
    psmrts::PsmrtsTracer hit_tracer = sys_bullet.get_tracer_from_intercept( prq_ray );
    CHECK( hit_tracer.isValid() == true );

    // get_tracer_from_intercept() - miss case (observer at +X, looking further +X)
    psmrts::PRQRayTrace miss_ray = sys_bullet.shape_trace( obs, away );
    CHECK( miss_ray.hasHit() == false );
    CHECK( sys_bullet.get_tracer_from_intercept( miss_ray ).isValid() == true ); // should this be false?

    // Shared ellipsoid system for ellipsoid_trace and 
    // ellipsoid_photometric_trace tests
    psmrts::PsmrtsTracerSystem sys_ell("test_ell");
    sys_ell.set_reference_ellipsoid("Bennu", { 0.283065, 0.271215, 0.249720 });

    // ellipsoid_trace() PRQRayTrace overload
    psmrts::PRQRayTrace ell_ray( Eigen::Vector3d(obs.data()), 
                                 Eigen::Vector3d(lkdr.data()) );
    CHECK( sys_ell.ellipsoid_trace( ell_ray ) == true );
    CHECK( ell_ray.hasHit() == true );

    // ellipsoid_photometric_trace() PRQPhotometricTrace overload
    psmrts::PRQPhotometricTrace prq_ellphoto( Eigen::Vector3d(obs.data()),
                                              Eigen::Vector3d(lkdr.data()),
                                              Eigen::Vector3d(sunpos.data()) );
    CHECK( sys_ell.ellipsoid_photometric_trace( prq_ellphoto ) == true );
    CHECK( prq_ellphoto.observer_trace().hasHit() == true );

    // ellipsoid_photometric_trace() Eigen overload
    psmrts::PRQPhotometricTrace eigen_ellphoto = sys_ell.ellipsoid_photometric_trace(e_obs, e_lkdr, e_sunpos);
    CHECK( eigen_ellphoto.observer_trace().hasHit() == true );

    // translations()
    psmrts::PsmrtsTracerSystem sys_trans("test_trans");
    const psmrts::PsmrtsTranslations &trans = sys_trans.translations();
    std::string plain = "/some/plain/path.obj";
    CHECK( trans.translate_path( plain ) == plain );
}

