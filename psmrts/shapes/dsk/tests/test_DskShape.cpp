#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/dsk/DskShape.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>


TEST_CASE( "DSK SHAPE - Default Test", "[dsk][shape][specification]") {
    psmrts::ProductSpecification spec = psmrts::DskShape::product_specifications();

    CHECK( spec.name()              == "dsk"   );
    CHECK( spec.product()           == "shape" ); 
    CHECK( spec.size()              == 5 );
    CHECK( spec.features().size()   == 5 );
    CHECK( spec.required().size()   == 1 );
    CHECK( spec.optional().size()   == 4 );

    CHECK( spec.contains( "dsk_mtl_search_path" ) == false );
    CHECK( spec.contains( "dsk_data_type" )       == true );
    CHECK( spec.contains( "dsk_body_id" )         == true );
    CHECK( spec.contains( "dsk_segment_index" )   == true );

}


TEST_CASE( "PSMRTS Product DSK Specification Test", "[product][type][mesh][dsk]") {
    double tolerance = 1.0e-6;
    CHECK( sizeof( psmrts::DskShape ) <= 1370 );  

    std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
    psmrts::DskShape dsk_m( dskfile );
    CHECK( dsk_m.name() == dskfile );
    CHECK( dsk_m.type() == "shape" );
    CHECK( dsk_m.model() == "dsk" );
    CHECK( psmrts::PsmrtsUID::is_valid_uid( dsk_m.uid() ) );
    
    psmrts::PsmrtsMeshData mesh_d = dsk_m.get_mesh( );
    CHECK( mesh_d.nfacets()        == 36 );
    CHECK( mesh_d.nvectors()       == 20 );
    CHECK( mesh_d.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDouble );
    CHECK( mesh_d.isVectorDouble() == true );

    // Values Comparison Test
    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::ObjShape obj_m( objfile );
    psmrts::PsmrtsMeshData mesh_o = obj_m.get_mesh();

    for ( size_t i = 0; i < mesh_d.nvectors(); i ++ ) {
        CHECK( mesh_d.get_vector(i) == mesh_o.get_vector(i) );
    }

    for ( size_t j = 0; j < mesh_d.nfacets(); j++ ) {
        CHECK( mesh_d.get_index(j) == mesh_o.get_index(j) );
    }

    CHECK_THAT( mesh_d.mesh_surface_area(),
                Catch::Matchers::WithinAbs( 0.842492, tolerance ) );
    CHECK_THAT( mesh_d.mesh_volume(),
                Catch::Matchers::WithinAbs( 0.063170, tolerance ) );
    
    psmrts::ProductConfiguration config_data = dsk_m.config();
    auto config_list = config_data.options();
    auto config_meta = config_data.metadata(); 

    CHECK( config_data.isvalid() > 0 );
    CHECK( config_data.name()    == dskfile );
    CHECK( config_data.size()    == 2 ); 
    CHECK( psmrts::psmrts_filename(  config_list.find("dsk_file").to_string() )  == "bennu_20facets.bds" );
    CHECK( config_list.contains("shape") );
    CHECK( config_list.find("shape").to_string()     == "dsk" );

    CHECK( config_meta.find("dsk_data_type").to_string() == "double" );
    CHECK( config_meta.find("dsk_segments").to_string()       == "1" );
    CHECK( config_meta.find("dsk_segment_number").to_string() == "0" );
    CHECK( config_meta.find("dsk_surface_id").to_string()     == "2101955" );
    CHECK( config_meta.find("n_vertices").to_string()         == "20" ); 
    CHECK( config_meta.find("n_facets").to_string()           == "36" ); 
    CHECK( config_meta.find("dsk_reference_id").to_string()   == "2101955" );
    CHECK( config_meta.find("dsk_body_id").to_string()        == "2101955" ); 
    CHECK( config_meta.find("dsk_frame_id").to_string()       == "10106" );
    CHECK( config_meta.find("dsk_type").to_string()           == "2" );
    CHECK( config_meta.find("dsk_class").to_string()          == "1" ); 
    CHECK_THAT( config_meta.find("minimum_radius").to_double(), 
                Catch::Matchers::WithinAbs( 0.22493886860043516, tolerance) ); 
    CHECK_THAT( config_meta.find("maximum_radius").to_double(), 
                Catch::Matchers::WithinAbs( 0.2830650000000668, tolerance) ); 

    psmrts::PsmrtsTranslations tln;
    psmrts::ProductSpecification spec_d = psmrts::DskShape::product_specifications();
    psmrts::ProductCart cart_d = psmrts::ProductCart( spec_d, config_data );
    psmrts::DskShape shape2( cart_d );

    CHECK( shape2.name() == dskfile );
    CHECK( shape2.type() == dsk_m.type() );
    CHECK( shape2.type() == dsk_m.type() );
    CHECK( psmrts::PsmrtsUID::is_valid_uid( shape2.uid() ) );

    // bad shape
    psmrts::ProductConfiguration bad_config1("bad_one", { psmrts::ProductOption("shape", "ply") } );
    CHECK_THROWS( psmrts::DskShape( psmrts::ProductCart( spec_d, bad_config1 )) );

    // bad file ("dsk_file")
    psmrts::ProductConfiguration bad_config2("bad_two", { psmrts::ProductOption("dsk_file", "bad_file_name") } );
    CHECK_THROWS( psmrts::DskShape( psmrts::ProductCart( spec_d, bad_config2 )) );
}
