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
    CHECK( sys2.invoice().name() == "test" );
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

    std::string file = psmrts_tracers_path("dsk/data/bennu_20facets.bds");
    CHECK(sys1.add_product("dsk", file) == true );

    std::vector<double> sunpos = { 0.0, 1000.0, 0.0 };
    psmrts::PRQPhotometricTrace p_ray = sys1.ellipsoid_photometric_trace(obs, lkdr, sunpos);
    CHECK( p_ray.observer_trace().hasHit() == true );

    //psmrts::PsmrtsTracer naifdsk_t ( psmrts::PsmrtsTracer::naifdsk( file ) );
    //void naif_add = sys1.add_tracer( naifdsk_t ); // this returns void, not bool like above..
    //CHECK( naif_add == true );                    // should we be consistent?

    psmrts::PsmrtsTracer e_tracer = psmrts::PsmrtsTracer::ellipsoid( { 0.283065,0.271215,0.249720 }, "Bennu" );
    psmrts::PsmrtsTracerSystem sys2("test2");
    sys2.set_reference_ellipsoid( e_tracer );

    CHECK( sys2.get_ellipsoid_tracer().isValid() == true );
}

TEST_CASE("PsmrtsTracerSystem Priority Tracer Test", "[tracer][system][priority]") {
    psmrts::PsmrtsTracerSystem sys("p_tracers");

    std::string file = psmrts_tracers_path("naifdsk/data/bennu_20facets.bds");
    bool added = sys.add_product("dsk", file);
    //CHECK( file == "" );
    REQUIRE( added == true ); // this is failing, 

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

