#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <string>
#include <memory>
#include <iostream>

#include <psmrts/core/ProductParameter.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>

TEST_CASE( "ProductParameter Constructor / Base Function Tests", "[product][parameter][base]") {
    psmrts::ProductParameter p_param;

    CHECK( p_param.size()                 == 0 );
    CHECK( p_param.contains( "required" ) == false );
    CHECK( p_param.name()                 == "" );
    CHECK( p_param.type()                 == "string" );
    CHECK( p_param.description()          == "parameter" );
    CHECK( p_param.status()               == "required" );
    CHECK( p_param.keywords()             == std::vector<std::string> {} );
    CHECK( p_param.aliases()              == std::vector<std::string> {} );
    CHECK( p_param.file_suffixes()        == std::vector<std::string> {} );
    CHECK( p_param.specs().size()         == 0 );
    CHECK( p_param.is_required()          == true );

    char vals[] = R"({
        "name": "default",
        "type": "constructor"
    })";

    CHECK( p_param.difference( psmrts::json_utils::parse_json_string(vals) ) ==
     psmrts::json_utils::parse_json_string("[{\"op\":\"replace\",\"path\":\"\",\"value\":{\"name\":\"default\",\"type\":\"constructor\"}}]") );
}

TEST_CASE( "ProductParameter Values Test", "[product][parameter][values]") {
    char vals[] = R"({
        "type": "test_type",
        "description": "values to test",
        "status": "required",
        "aliases": ["test", "test_data"]
    })";
    psmrts::ProductParameter p_param1( "name", "test_name", psmrts::json_utils::parse_json_string(vals));

    CHECK( p_param1.size()                           == 5 );
    CHECK( p_param1.contains("description")          == true );
    CHECK( p_param1.contains("required")             == false );
    CHECK( p_param1.value("status", std::string("")) == "required" );

    CHECK_NOTHROW( p_param1.add_key("default", "string"));
    CHECK( p_param1.size()                            == 6 );
    CHECK( p_param1.value("default", std::string("")) == "string" );
    CHECK( p_param1.value("bad_key", std::string("")) == "" ); 

    CHECK( p_param1.name()          == "test_name" );
    CHECK( p_param1.type()          == "test_type" );
    CHECK( p_param1.description()   == "values to test" );
    CHECK( p_param1.status()        == "required" );
    CHECK( p_param1.keywords()      == std::vector<std::string>{"aliases", "description", "status", "type", "name", "default"} );
    CHECK( p_param1.aliases()       == std::vector<std::string>{"test", "test_data"} );
    CHECK( p_param1.file_suffixes() == std::vector<std::string> {} );
    CHECK( p_param1.is_required()   == true );

    CHECK( p_param1.isa_alias( "test" )      == true );
    CHECK( p_param1.isa_alias( "test_data" ) == true );
    CHECK( p_param1.isa_alias( "bad_alias" ) == false );

    char specs[] = R"({
        "name": "test_name",
        "type": "test_type",
        "description": "values to test",
        "status": "required",
        "aliases": ["test", "test_data"],
        "default": "string"
    })";
    ordered_json jspecs = psmrts::json_utils::parse_json_string( specs );
    CHECK( json::diff( p_param1.specs(), jspecs ).empty() == true );
    CHECK( p_param1.difference( jspecs ).empty()          == true );


    char vals2[] = R"({
        "name": "name_dupe",
        "type": "test_type",
        "status": "optional",
        "aliases": ["test_name", "test_data"],
        "default": "hieroglyphics",
        "file_suffixes": ["txt", "TXT"],
        "bad_key": "error"
    })";
    psmrts::ProductParameter p_param2( "name", "test", psmrts::json_utils::parse_json_string(vals2));

    CHECK( p_param2.size()                           == 7 );
    CHECK( p_param2.contains("unrelated_key")        == false );
    CHECK( p_param2.contains("description")          == false );
    CHECK( p_param2.description()                    == "parameter");
    CHECK( p_param2.value("status", std::string("")) == "optional" );

    CHECK_NOTHROW( p_param2.add_key("default", "string")); 
    CHECK( p_param2.size()                            == 7 );
    CHECK( p_param2.value("default", std::string("")) == "string" ); 
    CHECK( p_param2.value("bad_key", std::string("")) == "error" ); 

    CHECK( p_param2.name()          == "test" );
    CHECK( p_param2.type()          == "test_type" );
    CHECK( p_param2.description()   == "parameter" );
    CHECK( p_param2.status()        == "optional" );
    CHECK( p_param2.keywords()      == std::vector<std::string>{"aliases", "bad_key", "default", "file_suffixes", "name", "status", "type"} );
    CHECK( p_param2.aliases()       == std::vector<std::string>{"test_name", "test_data"} );
    CHECK( p_param2.file_suffixes() == std::vector<std::string> {"txt", "TXT"} );
    CHECK( p_param2.is_required()   == false );

    CHECK( p_param2.isa_alias( "test" )      == false );
    CHECK( p_param2.isa_alias( "test_data" ) == true );
    CHECK( p_param2.isa_alias( "bad_alias" ) == false );
}

TEST_CASE( "ProductParamater Validate Test", "[product][parameter][validate]") {
    char vals1[] = R"({
        "name": "obj_file",
        "type": "test",
        "description": "Name of OBJ file to read",
        "status": "required",
        "aliases": ["file", "obj_mesh", "mesh_file"],
        "file_suffixes": [ "obj", "OBJ" ]
    })";
    psmrts::ProductParameter param1( psmrts::json_utils::parse_json_string( vals1 ) );

    CHECK( param1.size()                   == 6 );
    CHECK( param1.contains( "name" )       == true );
    CHECK( param1.contains( "obj" )        == false );
    CHECK( param1.name()                   == "obj_file" );
    CHECK( param1.type()                   == "test" );
    CHECK( param1.description()            == "Name of OBJ file to read" );
    CHECK( param1.status()                 == "required" );
    CHECK( param1.keywords()               == std::vector<std::string> {"aliases","description","file_suffixes","name","status","type"});
    CHECK( param1.aliases()                == std::vector<std::string> {"file","obj_mesh","mesh_file"});
    CHECK( param1.file_suffixes()          == std::vector<std::string> {"obj", "OBJ"});
    CHECK( param1.is_required()            == true );
    CHECK( param1.isa_alias( "mesh_file" ) == true );
    CHECK( param1.isa_alias( "OBJ" )       == false );

    char vals2[] = R"({
        "name": "file",
        "type": "test",
        "status": "required",
        "aliases": ["file", "obj_mesh", "mesh_file"],
        "file_suffixes": [ "obj", "OBJ" ],
        "file": "test_file.obj"
    })";
    psmrts::ProductParameter param2( psmrts::json_utils::parse_json_string( vals2 ) );
    
    CHECK( param1.validate( param2 ) == true ); 

    REQUIRE( param2.contains(param2.name()) == true );
    CHECK( psmrts::psmrts_file_extension( "test_file.obj" ) == "obj" );   
}

TEST_CASE( "ProductParameter Validate File Test", "[product][parameter][file]") {
    char vals1[] = R"({
        "name": "obj_file",
        "type": "file",
        "description": "Name of OBJ file to read",
        "status": "required",
        "aliases": ["file", "obj_mesh", "mesh_file"],
        "file_suffixes": [ "obj", "OBJ" ]
    })";
    psmrts::ProductParameter param( psmrts::json_utils::parse_json_string( vals1 ) );

    // Check that extension matches a suffix
    char vals2[] = R"({
        "name": "obj_file",
        "type": "file",
        "status": "required",
        "obj_file": "test_file.obj"
    })";
    psmrts::ProductParameter check1( psmrts::json_utils::parse_json_string( vals2 ) );
    REQUIRE( psmrts::psmrts_file_extension( check1.specs()[check1.name()] ) == "obj" );
    CHECK( param.validate( check1 ) == true );

    // Check for bad extension
    char bad_val[] = R"({
        "name": "obj_file",
        "type": "file",
        "status": "required",
        "obj_file": "bad_file.ply"
    })";
    psmrts::ProductParameter check2( psmrts::json_utils::parse_json_string( bad_val ) );
    REQUIRE( psmrts::psmrts_file_extension( check2.specs()[check2.name()] ) != "obj" );
    CHECK( param.validate( check2 ) == false );
}

TEST_CASE( "ProductParameter Validate Alias Test", "[product][parameter][alias]") {
    char vals[] = R"({
        "name": "obj_file",
        "type": "other",
        "description": "Name of OBJ file to read",
        "status": "required",
        "aliases": ["file", "obj_mesh", "mesh_file"],
        "file_suffixes": [ "obj", "OBJ" ]
    })";
    psmrts::ProductParameter param( psmrts::json_utils::parse_json_string( vals ) );

    char vals1[] = R"({
        "name": "obj_mesh",
        "type": "other",
        "status": "required"
    })";
    psmrts::ProductParameter check1( psmrts::json_utils::parse_json_string( vals1 ) );
    CHECK( param.validate( check1 ) == true );

    char vals2[] = R"({
        "name": "bad_file",
        "type": "other",
        "status": "required"
    })";
    psmrts::ProductParameter check2( psmrts::json_utils::parse_json_string( vals2 ) );
    CHECK( param.validate( check2 ) == false );


    // One item alias - note: even single aliases must be in array structure format 
    char vals3[] = R"({
        "name": "obj_file",
        "type": "other",
        "description": "Name of OBJ file to read",
        "status": "required",
        "aliases": ["mesh_file"],
        "file_suffixes": [ "obj", "OBJ" ]
    })";
    psmrts::ProductParameter param2( psmrts::json_utils::parse_json_string( vals3 ) );

    char vals4[] = R"({
        "name": "mesh_file",
        "type": "other",
        "status": "required"
    })";
    psmrts::ProductParameter check3( psmrts::json_utils::parse_json_string( vals4 ) );

    CHECK( param2.validate(check3) == true ); 
}

TEST_CASE( "ProductParameter Vaidate String Test", "[product][parameter][string]") {
    char vals[] = R"({
        "name": "obj_string",
        "type": "string",
        "description": "Format-compatible string containing contents of an OBJ file",
        "status": "optional",
        "aliases": ["obj_mesh_string"],
        "obj_string": ["test_value"]
    })";
    psmrts::ProductParameter param( psmrts::json_utils::parse_json_string(vals) );

    char vals1[] = R"({
        "name": "obj_string",
        "type": "string",
        "status": "optional",
        "obj_string": "test_value"
    })";
    psmrts::ProductParameter check1( psmrts::json_utils::parse_json_string( vals1 ) );

    CHECK( param.validate( check1 ) == true );

    char bad_vals[] = R"({
        "name": "obj_string",
        "type": "string",
        "status": "optional",
        "obj_string": "bad_value"
    })";
    psmrts::ProductParameter check2( psmrts::json_utils::parse_json_string( bad_vals ) );

    CHECK( param.validate( check2 ) == false );

    char miss_vals[] = R"({
        "name": "obj_string",
        "type": "string",
        "status": "optional"
    })";
    psmrts::ProductParameter check3( psmrts::json_utils::parse_json_string( bad_vals ) );

    CHECK( param.validate( check3 ) == false );
}


TEST_CASE( "ProductParameter From PVL Test", "[product][parameter][pvl]") {
    std::string empty = "";
    // Parameters require a name..
    REQUIRE_THROWS( psmrts::ProductParameter::from_pvl( empty ) );
    
    std::string test = "name=pvl;type=string;status=required;";

    REQUIRE_NOTHROW( psmrts::ProductParameter::from_pvl( test ) );
    psmrts::ProductParameter param = psmrts::ProductParameter::from_pvl( test );

    CHECK( param.name()   == "pvl" );
    CHECK( param.type()   == "string" );
    CHECK( param.status() == "required" );

    const char *c_test = "name=pvl;type=string;status=optional";

    REQUIRE_NOTHROW( psmrts::ProductParameter::from_pvl( c_test ) );
    psmrts::ProductParameter c_param = psmrts::ProductParameter::from_pvl( c_test );

    CHECK( c_param.name()   == "pvl" );
    CHECK( c_param.type()   == "string" );
    CHECK( c_param.status() == "optional" );

    std::string newline = "name=pvl\ntype=string\nstatus=optional\n";
    psmrts::ProductParameter n_param = psmrts::ProductParameter::from_pvl( newline );

    CHECK( n_param.name()   == "pvl" );
    CHECK( n_param.type()   == "string" );
    CHECK( n_param.status() == "optional" );

    std::string mix = "name=both\ntype=string;status=required\n";
    psmrts::ProductParameter m_param = psmrts::ProductParameter::from_pvl( mix );

    CHECK( m_param.name()   == "both" );
    CHECK( m_param.type()   == "string");
    CHECK( m_param.status() == "required");

}
