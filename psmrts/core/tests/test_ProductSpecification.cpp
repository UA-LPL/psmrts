#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <string>
#include <memory>
#include <iostream>
#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductFeature.hpp>
#include <psmrts/core/ProductSpecification.hpp>


TEST_CASE ( "ProductSpecification Constructor / Base Function Test", "[product][specification][constructor][base]") {
    psmrts::ProductSpecification product1;

    CHECK( product1.name() == "null" );
    CHECK( product1.type() == ""     );
    
    // No constructor to create based on ProductFeature, seemingly must use json
    psmrts::ProductFeature prodspecs( product1.json_specs() );

    ordered_json result;
    result["name"] = "null";
    //result["type"] = "None";
    CHECK( prodspecs.size()               == 1      );
    CHECK( prodspecs.contains("Required") == false  );
    CHECK( prodspecs.specs()         == result );

    
    CHECK_NOTHROW( product1.required() );
    CHECK_NOTHROW( product1.optional() );

    CHECK( product1.required().size() == 0 );
    CHECK( product1.optional().size() == 0 );
}

TEST_CASE( "ProductSpecification Values Test", "[product][specification][values]") {
    psmrts::ProductSpecification product1("A", "B");

    CHECK( product1.name() == "A" );
    CHECK( product1.type() == "B" );

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
    CHECK( product2.type()                 == "D"  );
    CHECK( prod2specs.size()               == 8    );
    CHECK( prod2specs.contains("status")   == true );

    std::vector<std::string> req = product2.required();
    std::vector<std::string> opt = product2.optional();
    CHECK( req.size() == 1           );
    CHECK( opt.size() == 0           );
    CHECK( req[0]     == "obj_file"  );
   
    std::vector<std::string> p_names = product2.get_parameter_names();
    CHECK( p_names == std::vector<std::string>{ "obj_file" } );

    psmrts::ProductFeature target = product2.get_parameter( "obj_file" );
    CHECK( target.name() == "obj_file" );

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