#include <psmrts_catch2_environment.hpp>

#include <string>
#include <memory>
#include <iostream>
#include <string>

#include <ProductSpecification.hpp>
#include <PsmrtsParameters.hpp>
#include <PsmrtsUtilities.hpp>


TEST_CASE ( "ProductSpecification Constructor / Base Function Test", "[product][specification][constructor][base]") {
    psmrts::ProductSpecification product1;

    CHECK( product1.name() == "None" );
    CHECK( product1.type() == "None" );
    
    psmrts::PsmrtsParameters prodspecs = product1.specs();

    ordered_json result;
    result["name"] = "None";
    result["type"] = "None";
    CHECK( prodspecs.size()               == 2      );
    CHECK( prodspecs.contains("Required") == false  );
    CHECK( prodspecs.parameters()         == result );

    CHECK_THROWS( product1.required() );
    CHECK_THROWS( product1.optional() );

}

TEST_CASE( "ProductSpecification Values Test", "[product][specification][values]") {
    psmrts::ProductSpecification product1("A", "B");

    CHECK( product1.name() == "A" );
    CHECK( product1.type() == "B" );

    psmrts::PsmrtsParameters prod1specs = product1.specs();

    ordered_json result;
    result["name"] = "A";
    result["type"] = "B";
    CHECK( prod1specs.size()               == 2     );
    CHECK( prod1specs.contains("required") == false );
    CHECK( prod1specs.parameters()         == result);

    CHECK_THROWS( product1.required() );
    CHECK_THROWS( product1.optional() );


    ordered_json options;
    char reqText[] = R"( {
        "filename": "test.cpp",
        "data_type": "char",
        "extra": "not_needed",
        "required": ["filename", "data_type"],
        "optional": ["extra"]
    }
    )";
    options = psmrts::json_utils::parse_json_string(reqText);

    psmrts::ProductSpecification product2("C", "D", options);
    psmrts::PsmrtsParameters prod2specs = product2.specs();

    CHECK( product2.name()                 == "C"  );
    CHECK( product2.type()                 == "D"  );
    CHECK( prod2specs.size()               == 7    );
    CHECK( prod2specs.contains("required") == true );

    std::vector<std::string> req = product2.required();
    std::vector<std::string> opt = product2.optional();
    CHECK( req.size() == 2           );
    CHECK( opt.size() == 1           );
    CHECK( req[0]     == "filename"  );
    CHECK( req[1]     == "data_type" );
    CHECK( opt[0]     == "extra"     );

    ordered_json options2;
    char reqCheck[] = R"({
        "filename": "test.cpp",
        "extra": "not_needed",
        "required": ["filename", "data_type"],
        "optional": ["extra"]
    }
    )";

    psmrts::ProductSpecification product3("E", "F", options2);
    CHECK( product3.matches(product2) == false );
}