#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <string>
#include <memory>
#include <iostream>
#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/ProductFeature.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductOrder.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/ProductCart.hpp>
#include <psmrts/core/ProductProcessing.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>


TEST_CASE ( "ProductSpecification Constructor / Base Function Test", "[product][specification][constructor][base]") {
    psmrts::ProductSpecification product1;

    CHECK( product1.name() == "none" );
    
    // No constructor to create based on ProductFeature, seemingly must use json
    CHECK_THROWS( psmrts::ProductFeature( product1.to_json() ) );

    ordered_json result;
    result["name"] = "feature";
    psmrts::ProductFeature prodspecs( result );   

    CHECK( prodspecs.size()               == 1      );
    CHECK( prodspecs.contains("Required") == false  );
    CHECK( prodspecs.specs()         == result );

    
    CHECK_NOTHROW( product1.required() );
    CHECK_NOTHROW( product1.optional() );

    CHECK( product1.required().size() == 0 );
    CHECK( product1.optional().size() == 0 );
    const char *c1 = "name=double;type=double;status=required";
    const char *c2 = "name=boolean;type=bool;status=required";
    const char *c3 = "name=integer;type=int;status=required";
    //const char *c4 = "name=double2;type=double;status=optional";
    psmrts::ProductFeature dbl_feat(psmrts::ProductFeature::from_pvl( c1 ) );
    psmrts::ProductFeature bool_feat(psmrts::ProductFeature::from_pvl( c2 ) );
    psmrts::ProductFeature int_feat(psmrts::ProductFeature::from_pvl( c3 ) );
    //psmrts::ProductFeature dbl2_feat(psmrts::ProductFeature::from_pvl( c4 ) );
    CHECK_NOTHROW( product1.add_feature( dbl_feat ) );
    CHECK_NOTHROW( product1.add_feature( bool_feat ) );
    CHECK_NOTHROW( product1.add_feature( int_feat ) );
    //CHECK_NOTHROW( product1.add_feature( dbl2_feat ) );

    psmrts::ProductConfiguration config1;
    config1.add_option(psmrts::ProductOption("double", 3.14));
    config1.add_option(psmrts::ProductOption("boolean", true));
    config1.add_option(psmrts::ProductOption("integer", 42));
    config1.add_option(psmrts::ProductOption("double2", 6.28));

    psmrts::PsmrtsTranslations tls;

}

TEST_CASE( "ProductSpecification Configuration Test", "[product][specification][configuration]") {

  psmrts::PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  trans_t.add_parameter("objdir", "obj/data");

  std::string objfile = psmrts_shapes_path( trans_t.translate_path( "$objdir/bennu_20facets.obj" ) );
  psmrts::PsmrtsShape shape_t( objfile );
  psmrts::ProductSpecification specs_t = shape_t.specs();
  psmrts::ProductConfiguration config_t = shape_t.config();
  psmrts::ProductCart cart_t( specs_t, config_t );
  CHECK( config_t.size() == 3 );
  psmrts::ProductProcessing process_t( trans_t );
  psmrts::ProductOrder order_t = process_t.process_cart( cart_t );
  CHECK( order_t.isvalid()          == true );
  CHECK( order_t.error_count()      == 0 );
  CHECK( order_t.errors_to_string() == "" );
  CHECK( order_t.config().size()    == 3 );
  CHECK( order_t.residual().size()  == 0 );

  trans_t.add_environment("PLYDIR", "ply/data");
  std::string plyfile = psmrts_shapes_path( "$PLYDIR/Bennu_Radar.ply" );
  psmrts::PsmrtsShape shape_p( trans_t.translate_path( plyfile ) );
  psmrts::ProductSpecification specs_p  = shape_p.specs();
  psmrts::ProductConfiguration config_p = shape_p.config();
  psmrts::ProductCart cart_p( specs_p, config_p );
  // CHECK( specs_p.to_json().dump(2) == "" );

  CHECK( config_p.size() == 2 );
  psmrts::ProductOrder order_p = process_t.process_cart( cart_p );
  CHECK( order_p.isvalid()          == true );
  CHECK( order_p.error_count()      == 0 );
  CHECK( order_p.errors_to_string() == "" );
  CHECK( order_p.config().size()    == 2 );
  CHECK( order_p.residual().size()  == 0 );

  std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
  psmrts::PsmrtsShape shape_d( dskfile );
  psmrts::ProductSpecification specs_d  = shape_d.specs();
  psmrts::ProductConfiguration config_d = shape_d.config();
  psmrts::ProductCart cart_d( specs_d, config_d );

  CHECK( config_d.size() == 3 );
  psmrts::ProductOrder order_d = process_t.process_cart( cart_d );
  CHECK( order_d.isvalid()          == true );
  CHECK( order_d.error_count()      == 0 );
  CHECK( order_d.errors_to_string() == "" );
  CHECK( order_d.config().size()    == 4 );
  CHECK( order_d.residual().size()  == 0 );
  // CHECK( order_d.to_json() == "" );
}



#if 0  

TEST_CASE( "ProductSpecification Values Test", "[product][specification][values]") {

    psmrts::ProductSpecification product1("A", "B");

    CHECK( product1.name() == "A" );
    CHECK( product1.product() == "B" );

    psmrts::ProductFeature prod1specs( product1.json_specs() );

    ordered_json result;
    result["name"] = "A";
    result["type"] = "B";
    CHECK( prod1specs.size()               == 2     );
    CHECK( prod1specs.contains("required") == false );
    CHECK( prod1specs.specs()         == result);

    CHECK_NOTHROW( product1.required() );
    CHECK_NOTHROW( product1.optional() );

    ordered_json options;
    char reqText[] = R"( {
        "filename": "test.cpp",
        "data_type": "char",
        "extra": "not_needed",
        "status": "required",
        "optional": ["extra"],
        "features": [
            {
              "name": "obj_file",
              "type": "file",
              "description": "Name of OBJ file to read",
              "status": "required",
              "aliases": ["file", "obj_mesh", "mesh_file"],
              "file_suffixes": [ "obj", "OBJ" ]
            } 
        ]
    }
    )";
    options = psmrts::json_utils::parse_json_string(reqText);

    psmrts::ProductSpecification product2("C", "D", options);
    psmrts::ProductFeature prod2specs( product2.json_specs() );

    CHECK( product2.name()                 == "C"  );
    CHECK( product2.product()             == "D"  );
    CHECK( prod2specs.size()               == 8    );
    CHECK( prod2specs.contains("status")   == true );

    std::vector<std::string> req = product2.required();
    std::vector<std::string> opt = product2.optional();
    CHECK( req.size() == 1           );
    CHECK( opt.size() == 0           );
    CHECK( req[0]     == "obj_file"  );
   

    ordered_json options2;
    char reqCheck[] = R"({
        "filename": "test.cpp",
        "extra": "not_needed",
        "status": "required",
        "optional": ["extra"],

    }
    )";

    psmrts::ProductSpecification product3("E", "F", options2);
    CHECK( product3.matches(product2) == false );
}

#endif
