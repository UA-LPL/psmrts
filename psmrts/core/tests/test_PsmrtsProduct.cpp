#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>


TEST_CASE( "PSMRTS Product", "[product][default]") {

  char config_p[] = R"(
    {
      "name": [ "dsk", "bds" ],
      "type": "mesh",
      "file": "dsk/data/bennu_20facets.bds",
      "segment" : 0
    }
  )";

  psmrts::PsmrtsProduct product;
  CHECK( product.name() == "product" );
  CHECK( product.type() == "type" );
  CHECK( product.uid()  != psmrts::PsmrtsUID::UID_Reserved );
}

#if 0
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

  psmrts::PsmrtsOBJFormat obj_m( psmrts_formats_path( "obj/data/bennu_20facets.obj" ) );
  psmrts::PsmrtsProduct<psmrts::PsmrtsOBJFormat> obj_product( obj_m.obj_source(), "obj", obj_m );

  CHECK( obj_product.name()        == obj_m.obj_source() );
  CHECK( obj_product.type()        == "obj" );
  CHECK( obj_product.has_product() == true );
  CHECK( obj_product.isValid()     == true );
  CHECK( obj_product.id()          != psmrts::PsmrtsUID::UID_Reserved );
  CHECK( obj_product.id()          != product.id() );
  CHECK( obj_product.id()          >  psmrts::PsmrtsUID::UID_Reserved );
  CHECK( obj_product.contains<psmrts::PsmrtsOBJFormat>() == true );
  CHECK( obj_product.product<psmrts::PsmrtsOBJFormat>()  != nullptr );

  psmrts::PsmrtsMeshData mesh_o = obj_product.product<psmrts::PsmrtsOBJFormat>()->get_mesh();
  CHECK( mesh_o.nfacets()        == 36 );
  CHECK( mesh_o.nvectors()       == 20 );
  CHECK( mesh_o.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDouble );
  CHECK( mesh_o.isVectorDouble() == true );


  psmrts::PsmrtsPLYFormat ply_m( psmrts_formats_path( "ply/data/Bennu_Radar.ply" ) );
  psmrts::PsmrtsProduct<psmrts::PsmrtsPLYFormat> ply_product( ply_m.ply_source(), "ply", ply_m );

  CHECK( ply_product.name()        == ply_m.ply_source() );
  CHECK( ply_product.type()        == "ply" );
  CHECK( ply_product.has_product() == true );
  CHECK( ply_product.isValid()     == true );
  CHECK( ply_product.id()          != psmrts::PsmrtsUID::UID_Reserved );
  CHECK( ply_product.id()          != product.id() );
  CHECK( ply_product.contains<psmrts::PsmrtsPLYFormat>() == true );
  CHECK( ply_product.product<psmrts::PsmrtsPLYFormat>()  != nullptr );

  psmrts::PsmrtsMeshData mesh_p = ply_product.product<psmrts::PsmrtsPLYFormat>()->get_mesh();
  CHECK( mesh_p.nfacets()        == 36 );
  CHECK( mesh_p.nvectors()       == 20 );
  CHECK( mesh_p.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDouble );
  CHECK( mesh_p.isVectorDouble() == true );

  std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
  psmrts::bullet::PsmrtsBulletWorldModel bt_world( psmrts::bullet::PsmrtsBulletMeshMap( psmrts::PsmrtsOBJFormat( objfile ) ), objfile );
  psmrts::BulletTracer b_tracer( bt_world );
  psmrts::PsmrtsProduct<psmrts::BulletTracer> blt_product( objfile, "bullet", b_tracer);
  CHECK( blt_product.name()        == objfile );
  CHECK( blt_product.type()        == "bullet" );
  CHECK( blt_product.has_product() == true );
  CHECK( blt_product.isValid()     == true );
  CHECK( blt_product.id()          != psmrts::PsmrtsUID::UID_Reserved );
  CHECK( blt_product.id()          != product.id() );
  CHECK( blt_product.contains<psmrts::BulletTracer>() == true );
  CHECK( blt_product.product<psmrts::BulletTracer>()  != nullptr );
  /** 
  psmrts::PsmrtsMeshData mesh_blt = blt_product.product<psmrts::BulletTracer>()->get_mesh();
  CHECK( mesh_blt.nfacets()        == 36 );
  CHECK( mesh_blt.nvectors()       == 20 );
  CHECK( mesh_blt.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDouble );
  CHECK( mesh_blt.isVectorDouble() == true );
  */
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

#endif
