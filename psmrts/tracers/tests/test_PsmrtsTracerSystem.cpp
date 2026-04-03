#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>

#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/PsmrtsTracerSystem.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>
#include <psmrts/tracers/naifdsk/NaifDskTracer.hpp>

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

/** 
TEST_CASE("PsmrtsTracerSytem Values Test", "[tracer][system][values]") {
    psmrts::PsmrtsTracerSystem sys1("test");

    char test_p[] = R"({"name": [ "dsk", "bds" ], "type": "mesh",
      "file": "dsk/data/bennu_20facets.bds","segment" : 0})";
}
*/
