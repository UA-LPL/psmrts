#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/products/PsmrtsProduct.hpp>


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
  CHECK( psmrts::PsmrtsUID::is_valid_uid( product.uid() ) );
}
