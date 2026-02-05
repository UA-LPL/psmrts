#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include "../MeshShape.hpp"
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/core/PsmrtsVector3.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>

TEST_CASE ("MESH SHAPE - Default Test", "[mesh][shape][default]") {
    psmrts::ProductSpecification spec = psmrts::MeshShape::product_specifications();

    CHECK( spec.name()              == "mesh" );
    CHECK( spec.product()           == "shape" );
    CHECK( spec.features().size()   == 3 );
    CHECK( spec.required().size()   == 1 );
    CHECK( spec.optional().size()   == 2 );

    CHECK( spec.contains( "obj_mtl_search_path" ) == false );
    CHECK( spec.contains( "mesh_name" )           == true );
    CHECK( spec.contains( "data_type" )           == true );

    psmrts::MeshShape shape;
    CHECK( shape.get_mesh().isValid()  == false );
    CHECK( shape.get_mesh().nvectors() == 0 );

    psmrts::ProductConfiguration mesh_config = shape.config();
    psmrts_json mesh_json = mesh_config.to_json();

    CHECK( mesh_config.name()          == "mesh" ); // possible constructor error in PConfig file?
    CHECK( mesh_config.size()          == 4 );
    CHECK( mesh_json["data_type"]      == "undefined" );
    CHECK( mesh_json["metadata"]["n_vertices"]     == 0 );
    CHECK( mesh_json["metadata"]["n_facets"]       == 0 );
    CHECK( mesh_json["metadata"]["minimum_radius"] == 0 );
    CHECK( mesh_json["metadata"]["maximum_radius"] == 0 );
}

TEST_CASE( "MESH SHAPE - Values Test", "[mesh][shape][values]") {
    double tolerance = 1.0e-6;
    typedef psmrts::PsmrtsVector3<double> ObjVectorData;
    typedef psmrts::PsmrtsVector3i ObjIndexData;

    const int n_data = 10;
    auto v_model  = ObjVectorData( n_data );

    double value = 1.0;
    for ( int n = 0 ; n < v_model.size() ; n++ ) {
        ObjVectorData::vector_reference data_t = v_model( n ); 
        for ( int v = 0 ; v < data_t.size() ; v++ ) {
          data_t[v] = value++; 
        }
      }
    
    auto i_model = ObjIndexData( n_data );
    for (int i = 0 ; i < i_model.size(); i++ ) {
        ObjIndexData::vector_reference data_y = i_model( i );
        data_y[0] = i;
        data_y[1] = i;
        data_y[2] = i;
    }

    CHECK( sizeof( psmrts::MeshShape ) <= 860 );  

    psmrts::PsmrtsMeshData mesh( i_model, v_model );
    psmrts::MeshShape shape( mesh, "mesh" );

    CHECK( shape.get_mesh().isValid()  == true );
    CHECK( shape.get_mesh().nvectors() == 10 );

    psmrts::ProductConfiguration mesh_config = shape.config();
    psmrts_json mesh_json = mesh_config.to_json();
    psmrts_json meta_json = mesh_config.to_json( mesh_config.metadata() );

    CHECK( mesh_config.name()      == "mesh" );
    CHECK( mesh_config.size()      == 2 );
    CHECK( mesh_json["name"]       == "mesh" ); 
    CHECK( mesh_json["data_type"]  == "double" );
    CHECK( mesh_json["metadata"]["n_vertices"] == 10 );
    CHECK( mesh_json["metadata"]["n_facets"]   == 10 );
    CHECK_THAT( mesh_json["metadata"]["minimum_radius"], Catch::Matchers::WithinAbs( 3.7416573867739413, tolerance ) );
    CHECK_THAT( mesh_json["metadata"]["maximum_radius"], Catch::Matchers::WithinAbs( 50.24937810560445,  tolerance ) );
}
