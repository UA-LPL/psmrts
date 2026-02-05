#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/shapes/dsk/private/PsmrtsDSKFormat.hpp>
#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>


TEST_CASE( "DSK FORMAT Asset Test - Default Constructor", "[format][dsk][default]") {
    std::string no_file = psmrts_shapes_path( "dsk/data/bad_path.bds" );
    psmrts::PsmrtsDSKFormat d_loader;

    CHECK( d_loader.isValid()             == false );
    CHECK( d_loader.dsk_source()          == "" );
    CHECK( d_loader.format_model_source() == "" );
    CHECK( d_loader.nVertexes()           == 0 );
    CHECK( d_loader.nIndexes()            == 0 );

    CHECK_THROWS( d_loader.load_dsk_file( no_file ) );

    CHECK( d_loader.get_mesh().isValid()        == false );
    CHECK( d_loader.get_float_vectors().size()  == 0 );
    CHECK( d_loader.get_double_vectors().size() == 0 );
    CHECK( d_loader.get_indexes().size()        == 0 );

}


TEST_CASE( "DSK FORMAT Basic Load/Innit Test", "[format][dsk][shape][bennu]") {
    std::string dsk_file = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
    psmrts::PsmrtsDSKFormat d_loader( dsk_file );
    
    CHECK( d_loader.isValid()                   == true );
    CHECK( d_loader.dsk_source()                == psmrts_shapes_path( "dsk/data/bennu_20facets.bds" ) );
    CHECK( d_loader.format_model_source()       == psmrts_shapes_path( "dsk/data/bennu_20facets.bds" ) );
    CHECK( d_loader.nVertexes()                 == 20 );
    CHECK( d_loader.nIndexes()                  == 36 );
    CHECK( d_loader.get_mesh().isValid()        == true );
    CHECK( d_loader.get_float_vectors().size()  == 20 );
    CHECK( d_loader.get_double_vectors().size() == 20 );
    CHECK( d_loader.get_indexes().size()        == 36 );

    
    naif::DskKernelModel dsk( dsk_file );

    psmrts::ProductConfiguration meta_data = d_loader.get_segment_metadata( dsk.segment() );

    CHECK( meta_data.name() == "dsk" );
    CHECK( meta_data.size() == 3 ); 
    CHECK( meta_data.metadata().size() == 12 ); 
    CHECK( psmrts::psmrts_filename(  meta_data.find("dsk_file").to_string() )  == "bennu_20facets.bds" );
    CHECK(  meta_data.find("data_type").to_string()                 == "double" );
    CHECK(  meta_data.find_metadata("dsk_segment_number").to_string() == "0" );
    CHECK(  meta_data.find_metadata("dsk_surface_id").to_string()     == "2101955" ); 
    CHECK(  meta_data.find_metadata("n_vertices").to_string()         == "20" );
    CHECK(  meta_data.find_metadata("n_facets").to_string()           == "36" );
    CHECK(  meta_data.find_metadata("dsk_reference_id").to_string()   == "2101955" );
    CHECK(  meta_data.find_metadata("dsk_body_id").to_string()        == "2101955" );
    CHECK(  meta_data.find_metadata("dsk_surface_id").to_string()     == "2101955" );
    CHECK(  meta_data.find_metadata("dsk_frame_id").to_string()       == "10106" );
    CHECK(  meta_data.find_metadata("dsk_type").to_string()           == "2" );
    CHECK(  meta_data.find_metadata("dsk_class").to_string()          == "1" );
    CHECK(  meta_data.find_metadata("minimum_radius").to_string()     == "0.224938869" );
    CHECK(  meta_data.find_metadata("maximum_radius").to_string()     == "0.283065000" );
    
}

TEST_CASE( "DSK FORMAT Comparative Values Test", "[format][dsk][kernel][bennu]") {
    const double tolerance = 1.0e-6;
    std::string dsk_file = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
    naif::DskKernelModel dsk( dsk_file );
    psmrts::PsmrtsDSKFormat d_loader( dsk_file );

    
    CHECK( dsk.plate_count()    == 36 );
    CHECK( dsk.n_dsk_segments() == 1);

    psmrts::PsmrtsVector3i model_indexes  = dsk.load_facet_indexes();
    psmrts::PsmrtsVector3i format_indexes = d_loader.get_indexes();

    for (int i=0; i < d_loader.nIndexes(); i++) {
        CHECK( format_indexes(i)[0] == model_indexes(i)[0] );
        CHECK( format_indexes(i)[1] == model_indexes(i)[1] );
        CHECK( format_indexes(i)[2] == model_indexes(i)[2] );
    }

    psmrts::PsmrtsVector3d model_vectors  = dsk.load_facet_vectors();
    psmrts::PsmrtsVector3d format_vectors = d_loader.get_double_vectors();

    for (int i=0; i < d_loader.nVertexes(); i++) {
        CHECK_THAT( format_vectors(i)[0], Catch::Matchers::WithinAbs( model_vectors(i)[0], tolerance ));
        CHECK_THAT( format_vectors(i)[1], Catch::Matchers::WithinAbs( model_vectors(i)[1], tolerance ));
        CHECK_THAT( format_vectors(i)[2], Catch::Matchers::WithinAbs( model_vectors(i)[2], tolerance ));
    }

    // Conversion Check
    psmrts::PsmrtsVector3f format_floats = d_loader.get_float_vectors();
    for (int i=0; i < d_loader.nVertexes(); i++) {
        CHECK_THAT( format_floats(i)[0], Catch::Matchers::WithinAbs( format_vectors(i)[0], tolerance ));
        CHECK_THAT( format_floats(i)[1], Catch::Matchers::WithinAbs( format_vectors(i)[1], tolerance ));
        CHECK_THAT( format_floats(i)[2], Catch::Matchers::WithinAbs( format_vectors(i)[2], tolerance ));
    }

    psmrts::PsmrtsRayTrace::FacetDatum target_facet;
    psmrts::PsmrtsRayTrace  raytrace;
    
    raytrace.datum().m_hit     = true;
    raytrace.datum().m_segment = dsk.segment().surfaceid(); 
    raytrace.datum().m_plateid = 1;

    naif::DskKernelModel::DskIndexDataModel::vector_type ones = naif::DskKernelModel::DskIndexDataModel::vector_type::Ones();
    for (int i = 0; i < dsk.n_total_plates(); i++) {
        raytrace.datum().m_plateid = i; 
        dsk.get_facet( raytrace, target_facet );
        CHECK ( target_facet.isValid() == true ); 
        
        psmrts::PsmrtsRayTrace::FacetDatum format_facet = d_loader.get_mesh().get_facet(i);

        CHECK( target_facet.m_indexes == format_facet.m_indexes );
        CHECK( target_facet.m_normal  == format_facet.m_normal  );
        CHECK( target_facet.m_vector1 == format_facet.m_vector1 );
        CHECK( target_facet.m_vector2 == format_facet.m_vector2 );
        CHECK( target_facet.m_vector3 == format_facet.m_vector3 );
    };
    
}
