#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>

TEST_CASE( "ProductOrder Default Test", "[order][default]") {
  psmrts::ProductOrder po;

  CHECK( po.isvalid()          == false );
  CHECK( po.isempty()          == true );
  CHECK( po.size()             == 0 );

  CHECK( po.name()             == "none" );
  CHECK( po.config().name()    == "none" ); 
  CHECK( po.cart_keys()        == std::vector<std::string>{ } );
  CHECK( po.cart_values()      == std::vector<psmrts::SharedCart>{ } );
  CHECK( po.find( "tracer" )   == nullptr );
  CHECK( po.translations()     == nullptr );
  CHECK( po.translate_path( "$osirisrex/kernels" ) == "$osirisrex/kernels" );
  CHECK( po.to_json().dump(-1) == R"({"submitted":{"options":{},"metadata":{}},"products":[]})" );
}

TEST_CASE( "ProductOrder Config Processing Test", "[order][config][processing]") {

  psmrts::ProductConfiguration config( std::string( "order_test" ), 
                               { psmrts::ProductOption( "tracer", "spheroid" ),
                                 psmrts::ProductOption( "radii", { 1.0, 2.0 } ),
                                 psmrts::ProductOption( "name", "test_spheroid" ) } );
                                 
  auto order_t = psmrts::ProductProcessing().process_order( config );
  REQUIRE( order_t != nullptr );

  CHECK( order_t->isvalid()   == true );
  CHECK( order_t->isempty()   == false );
  CHECK( order_t->size()      == 1 );

  CHECK( order_t->name()             == "order_test" );
  CHECK( order_t->config().name()    == "order_test" );

  CHECK( order_t->translate_path( "$osirisrex/kernels" ) == "$osirisrex/kernels" );

  CHECK( order_t->find( "shape" )  == nullptr );
  CHECK( order_t->find( "tracer" ) != nullptr );
  CHECK( order_t->cart_keys()      == std::vector<std::string>{ "tracer" } );
  CHECK( order_t->cart_values().size() == 1 );

  CHECK(order_t->to_json().dump(-1) == R"({"submitted":{"options":{"tracer":"spheroid","radii":[1.0,2.0],"name":"test_spheroid"},"metadata":{}},"products":[{"specification":{"info":{"name":"ellipsoid","product":"tracer","description":"Ellipsoid, spheroid and sphere ray tracer"},"features":[{"name":"tracer","type":"string","description":"Describe the product type: ellipsoid, spheroid or sphere","status":"optional","default":"ellipsoid","valid":["ellipsoid","spheroid","sphere"]},{"name":"radii","type":"double","description":"Radius values of the object: 1, 2 or 3 double values","status":"required","aliases":"radius"},{"name":"name","type":"string","description":"Name of the ellipsoid model","status":"optional","default":"ellipsoid","aliases":"model"}]},"configuration":{"options":{"tracer":"spheroid","radii":[1.0,2.0],"name":"test_spheroid"},"metadata":{}},"residualoptions":{},"uid":"0"}]})" );
}
