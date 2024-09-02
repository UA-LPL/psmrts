#include <psmrts_catch2_environment.hpp>

#include <PsmrtsShapeTracerAdapter.hpp>
#include <NaifEllipsoidShape.hpp>
#include <EllipsoidTracerModel.hpp>
#include <DskKernelModel.hpp>

// DO NOT WORRY ABOUT THIS ONE FOR NOW (8/28)
/*
TEST_CASE( "PsmrtsShapeTracerAdapter Default Test", "[shape][tracer][adapter][default]") {
    psmrts::PsmrtsShapeTracerAdapter<psmrts::PsmrtsTracerModel> base_adapter;

    CHECK( base_adapter.tracer_model_type() == "psmrts" ); 
    CHECK( base_adapter.tracer_model_name() == "" );
    CHECK( base_adapter.shape_tracer_id() == "psmrts::none" );
    CHECK( base_adapter.shapefile() == "" );
    CHECK( base_adapter.plate_count() == 0 );
    CHECK( base_adapter.vertex_count() == 0 );
    CHECK( base_adapter.maximum_radius() == 0 );
    CHECK( base_adapter.use_count() == 0 );
    psmrts::PsmrtsRayTrace ray;
    CHECK( base_adapter.ray_trace( Eigen::Vector3d {0.0, 0.0, 0.0}, Eigen::Vector3d {0.0, 0.0, 0.0}, ray ) == false );
    CHECK( ray.hasHit() == false );

    auto clone_adapter = base_adapter.clone(); 
    CHECK( clone_adapter->tracer_model_type() == "psmrts" ); 
    CHECK( base_adapter.use_count() == 1 ); // maybe?

    CHECK( base_adapter.ellipsoid() == nullptr );

}
*/