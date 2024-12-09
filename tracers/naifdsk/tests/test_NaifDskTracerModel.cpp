#include <psmrts_catch2_environment.hpp> 

#include <NaifDskTracerModel.hpp>
#include <DskKernelModel.hpp>
#include <PsmrtsUtilities.hpp>

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

TEST_CASE( "NAIF Dsk Tracer Model - Ray Trace / Values Test", "[default][bullet][tracer][model][values]") {
    const double tolerance = 1.0e-6;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dskfile );

    psmrts::NaifDskTracerModel d_model( dsk );
    //psmrts::NaifDskTracerModel d_model_filepath( "naifdsk/data/bennu_20facets.bds" );

    std::string d_shape_id = psmrts::psmrts_file_basename( d_model.shape_tracer_id() );
    std::string d_shapefile = psmrts::psmrts_file_basename( d_model.shapefile() );
    CHECK( d_model.tracer_model_type() == "naifdsk" );
    CHECK( d_model.tracer_model_name() == "DskKernelModel" );
    CHECK( d_shape_id == "bennu_20facets.bds" );
    CHECK( d_shapefile == "bennu_20facets.bds" );
    CHECK( d_model.plate_count() == 36 );
    CHECK( d_model.vertex_count() == 20 ); 
    CHECK_THAT( d_model.maximum_radius(), Catch::Matchers::WithinAbs(0.22493886860043516, tolerance ));

    /** WIP
    CHECK( d_model_filepath.tracer_model_type() == d_model.tracer_model_type() );
    CHECK( d_model_filepath.tracer_model_name() == d_model.tracer_model_name() );
    CHECK( d_model_filepath.shape_tracer_id() == d_model.shape_tracer_id() );
    CHECK( d_model_filepath.shapefile() == d_model.shapefile() );
    */

}