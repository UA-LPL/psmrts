
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsPriorityTracer.hpp>
#include <PsmrtsTracerModel.hpp>


TEST_CASE("PsmrtsPriorityTracer Default Test", "[priority][tracer][default]") {

    psmrts::PsmrtsPriorityTracer test_tracer;
    std::vector<std::string> myString;

    CHECK( test_tracer.isValid() == false );
    CHECK( test_tracer.size() == 0 );
    CHECK( test_tracer.find_model_by_name( "" ) == nullptr );
    CHECK( test_tracer.find_model_by_id( "" ) == nullptr );
    myString = test_tracer.get_shapefile_names();
    CHECK( myString.size() == 0 );
}
