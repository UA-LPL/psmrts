#include <psmrts_catch2_environment.hpp>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsProduct.hpp>
#include <PsmrtsDSKFormat.hpp>
#include <PsmrtsOBJFormat.hpp>
#include <PsmrtsPLYFormat.hpp>


TEST_CASE( "PSMRTS Product", "[product][type][config][dsk]") {

  char config_p[] = R"(
    {
      "name": [ "dsk", "bds" ],
      "type": "mesh",
      "file": "dsk/data/bennu_20facets.bds",
      "segment" : 0
    }
  )";

  psmrts::PsmrtsProduct<psmrts::PsmrtsDSKFormat> product( "dsk",  "mesh", std::string( config_p ) );

  CHECK( product.name()        == "dsk" );
  CHECK( product.type()        == "mesh" );
  CHECK( product.has_product() == false );
  CHECK( product.contains<psmrts::PsmrtsDSKFormat>() == false );
  CHECK( product.isValid()     == false );
  CHECK( product.product<psmrts::PsmrtsDSKFormat>()     == nullptr );
}

TEST_CASE( "PSMRTS Product Specifications", "[product][type][mesh][dsk]") {

  psmrts::PsmrtsDSKFormat dsk_m( psmrts_formats_path( "dsk/data/bennu_20facets.bds" ) );
  psmrts::PsmrtsProduct<psmrts::PsmrtsDSKFormat> product( dsk_m.dsk_source(), "dsk", dsk_m );

  CHECK( product.name()        == dsk_m.dsk_source() );
  CHECK( product.type()        == "dsk" );
  CHECK( product.has_product() == true );
  CHECK( product.isValid()     == true );
  CHECK( product.contains<psmrts::PsmrtsDSKFormat>() == true );
  CHECK( product.product<psmrts::PsmrtsDSKFormat>()  != nullptr );  

  psmrts::PsmrtsMeshData mesh_d = product.product<psmrts::PsmrtsDSKFormat>()->get_mesh( );
  CHECK( mesh_d.nfacets()        == 36 );
  CHECK( mesh_d.nvectors()       == 20 );
  CHECK( mesh_d.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDouble );
  CHECK( mesh_d.isVectorDouble() == true );

  // CHECK(product.config().dump(2) == "json");

}

TEST_CASE( "PSMRTS Product Specifications", "[product][type][cache]") {
  using MeshProducts = psmrts::PsmrtsProduct<psmrts::PsmrtsDSKFormat, psmrts::PsmrtsOBJFormat, psmrts::PsmrtsPLYFormat>;
  MeshProducts mesh_product( "meshes", "cache" );

  CHECK( sizeof( MeshProducts::Product ) == 536 );
  CHECK( mesh_product.name()        == "meshes" );
  CHECK( mesh_product.type()        == "cache" );
  CHECK( mesh_product.has_product() == false );
  CHECK( mesh_product.isValid()     == false );
  CHECK( mesh_product.contains<psmrts::PsmrtsDSKFormat>() == false );
  CHECK( mesh_product.product<psmrts::PsmrtsDSKFormat>()  == nullptr );  

  mesh_product.set_product( psmrts::PsmrtsDSKFormat( psmrts_formats_path( "dsk/data/bennu_20facets.bds" ) ) );
  CHECK( mesh_product.name()        == "meshes" );
  CHECK( mesh_product.type()        == "cache" );
  CHECK( mesh_product.has_product() == true );
  CHECK( mesh_product.isValid()     == true );
  CHECK( mesh_product.contains<psmrts::PsmrtsDSKFormat>() == true );
  CHECK( mesh_product.product<psmrts::PsmrtsDSKFormat>() != nullptr );

  const psmrts::PsmrtsDSKFormat *product_v = mesh_product.product<psmrts::PsmrtsDSKFormat>();
  psmrts::PsmrtsMeshData mesh              = product_v->get_mesh();

  CHECK( product_v->format_model_source() == psmrts_formats_path( "dsk/data/bennu_20facets.bds" ) );
  CHECK( product_v->isValid()             == true );
  CHECK( product_v->nIndexes()            == 36 );
  CHECK( product_v->nVertexes()           == 20 );

}