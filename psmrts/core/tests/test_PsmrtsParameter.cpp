#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <string>
#include <memory>
#include <exception>
#include <iostream>

#include <psmrts/core/PsmrtsParameters.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>

// using namespace psmrts::json_utils;

#include <string>
#include <vector>

TEST_CASE ( "PsmrtsParameters JSON Util Tests", "[parameters][json][basics]") {

  std::vector<double> vd3( { 1.0, 2.0, 3.0 } );
  json j_vd3 =  vd3;
  auto j_vd3_t = json::array( { 1.0, 2.0, 3.0 } );
  CHECK( j_vd3 == j_vd3_t );

  std::vector<int> vi3( { 1, 2, 3 } );
  json j_vi3 =  vi3;
  auto j_vi3_t = json::array( { 1, 2, 3 } );
  CHECK( j_vi3 == j_vi3_t );

  std::vector<std::string> vs3( { "one", "two", "three" } );
  json j_vs3 = vs3;
  json j_vs3_t = json::array( { "one", "two", "three" } );
  CHECK( j_vs3 == j_vs3_t );

  Eigen::Vector3d evd3( {1.0, 2.0, 3.0 } );
  json j_evd3 =  evd3;
  CHECK( j_evd3 == j_vd3_t );


  // This should be done with a simple variable assignment, not a direct call!!
  // This shows it does work as intended, however.
  Eigen::Vector3d e_evd3_t;
  Eigen::from_json(j_evd3, e_evd3_t );
  CHECK( j_evd3 == evd3 );

  Eigen::Vector3i evi3( vi3.data() );
  json j_evi3 = evi3;
  CHECK( j_evi3 == j_vi3_t );

#if defined(INCLUDE_EIGEN_JSON_TYPES)
  Eigen::Quaterniond qd( { 1.0, 2.0, 3.0, 4.0 } );
  json j_qd = qd;
  CHECK( j_qd["w"] == 1.0 );
  CHECK( j_qd["x"] == 2.0 );
  CHECK( j_qd["y"] == 3.0 );
  CHECK( j_qd["z"] == 4.0 );
#endif

}

TEST_CASE ( "PsmrtsParameters Parameter Tests", "[parameters][json]") {
  psmrts::PsmrtsParameters v_parms( "psmrts" );

  CHECK( v_parms.contains( "name" ) );
  CHECK( v_parms.get_string_parameter( "name" )  == "psmrts");

  v_parms.add_parameter( "one_i", 1 );
  v_parms.add_parameter( "two_d", 2.0 );
  v_parms.add_parameter( "three_s", "three" );

  std::vector<double> v_ds = { 1.0, 2.0, 3.0 };
  v_parms.add_parameter( "double_d", v_ds );

  std::string config_s = "{\"name\":\"psmrts\",\"one_i\":1,\"two_d\":2.0,\"three_s\":\"three\",\"double_d\":[1.0,2.0,3.0]}";
  CHECK( v_parms.config( ) == config_s );

  std::string config_p = "{\"parameters\":{\"name\":\"psmrts\",\"one_i\":1,\"two_d\":2.0,\"three_s\":\"three\",\"double_d\":[1.0,2.0,3.0]}}";
  CHECK( v_parms.config( "parameters") == config_p );


}
