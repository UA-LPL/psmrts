#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsJson.hpp>

TEST_CASE( "PsmrtsJson Eigen Default / Values Test", "[json][eigen][default]") {
    // Quaterniond
    Eigen::Quaterniond quat(1.0, 2.0, 3.0, 4.0);

    nlohmann::json q_to_j;

    Eigen::to_json(q_to_j, quat);

    CHECK( q_to_j["w"] == quat.w() );
    CHECK( q_to_j["x"] == quat.x() );
    CHECK( q_to_j["y"] == quat.y() );
    CHECK( q_to_j["z"] == quat.z() );

    nlohmann::json j_to_q;
    j_to_q["w"] = 1.0;
    j_to_q["x"] = 2.0;
    j_to_q["y"] = 3.0;
    j_to_q["z"] = 4.0;

    Eigen::Quaterniond j_quat;

    Eigen::from_json(j_to_q, j_quat);
    CHECK( j_quat.w() == 1.0 );
    CHECK( j_quat.x() == 2.0 );
    CHECK( j_quat.y() == 3.0 );
    CHECK( j_quat.z() == 4.0 );

    // Vector3d
    Eigen::Vector3d v3(123.456, -789.123, 456.789012345678901234 );

    nlohmann::json vd_to_j;

    Eigen::to_json(vd_to_j, v3);
    CHECK( vd_to_j[0] ==  123.456 );
    CHECK( vd_to_j[1] == -789.123 );
    CHECK( vd_to_j[2] ==  456.789012345678901234  );

    nlohmann::json j_to_vd( {1.0, 0.0, 3.1234567890123456 } );
    
    Eigen::Vector3d r_v3;

    Eigen::from_json(j_to_vd, r_v3);
    CHECK( r_v3[0] == 1.0 );
    CHECK( r_v3[1] == 0.0 );
    CHECK( r_v3[2] == 3.1234567890123456 );

    // Vector3i
    Eigen::Vector3i v3i(1, -2, 0);

    nlohmann::json vi_to_j;

    Eigen::to_json(vi_to_j, v3i);
    CHECK( vi_to_j[0] ==  1);
    CHECK( vi_to_j[1] == -2);
    CHECK( vi_to_j[2] ==  0);

    nlohmann::json j_to_vi( {0, -1, std::numeric_limits<int>::max()} );

    Eigen::Vector3i r_v3i;
    
    Eigen::from_json(j_to_vi, r_v3i);
    CHECK( r_v3i[0] ==  0 );
    CHECK( r_v3i[1] == -1 );
    CHECK( r_v3i[2] == std::numeric_limits<int>::max() );
}

TEST_CASE( "PsmrtsJson Utility Functions Test", "[json][utilities][default]") {
    CHECK( psmrts::json_utils::null() == "null" );

    nlohmann::json j_null = psmrts::json_utils::json_null();
    CHECK( j_null.dump() == "null" );
    CHECK( j_null.size() == 0 );

    CHECK( psmrts::json_utils::json_bool( true )  == "true" );
    CHECK( psmrts::json_utils::json_bool( false ) == "false" );

    std::string j_string1 = R"({"a": true, "b": 123.456, "x":"one", "y":-42, "z": 0})";
    nlohmann::ordered_json s_json1 = psmrts::json_utils::parse_json_string(j_string1);
    CHECK( s_json1["a"]   == true );
    CHECK( s_json1["b"]   == 123.456 );
    CHECK( s_json1["x"]   == "one" );
    CHECK( s_json1["y"]   == -42 );
    CHECK( s_json1["z"]   == 0 );
    CHECK( s_json1.size() == 5 );

    std::string j_string2 = R"({
                                // this is a test
                                "x": 42,
                                "y": 0.0
                                /* another comment */
                            })";

    CHECK_THROWS( psmrts::json_utils::parse_json_string(j_string2, false) );

    nlohmann::ordered_json s_json2 = psmrts::json_utils::parse_json_string(j_string2, true);
    CHECK( s_json2["x"]   == 42 ); 
    CHECK( s_json2["y"]   == 0.0 );
    CHECK( s_json2.size() == 2 );

    CHECK( psmrts::json_utils::dump_json_string(s_json2, -1) == R"({"x":42,"y":0.0})" );

    nlohmann::ordered_json j_insert = psmrts::json_utils::insert_object("test1", s_json2 );
    CHECK( j_insert.size() == 1 );
    CHECK( j_insert["test1"] == nlohmann::ordered_json({{"x",42},{"y",0.0}}) );

}
