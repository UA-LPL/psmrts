#include <psmrts_catch2_environment.hpp>

#include <string>
#include <memory>
#include <exception>
#include <iostream>

#include <PsmrtsParameters.hpp>
#include <PsmrtsUtilities.hpp>

using namespace psmrts::json_translations;

#include <string>
#include <vector>

TEST_CASE ( "PsmrtsParameters JSON Util Tests", "[parameters][json][basics]") {

  std::vector<double> vd3( { 1.0, 2.0, 3.0 } );
  ordered_json j_vd3 =  vd3;
  auto j_vd3_t = ordered_json::array( { 1.0, 2.0, 3.0 } );
  CHECK( j_vd3 == j_vd3_t );

  std::vector<int> vi3( { 1, 2, 3 } );
  ordered_json j_vi3 =  vi3;
  auto j_vi3_t = ordered_json::array( { 1, 2, 3 } );
  CHECK( j_vi3 == j_vi3_t );

  std::vector<std::string> vs3( { "one", "two", "three" } );
  ordered_json j_vs3 = vs3;
  ordered_json j_vs3_t = ordered_json::array( { "one", "two", "three" } );
  CHECK( j_vs3 == j_vs3_t );

  Eigen::Vector3d evd3( vd3.data() );
  ordered_json j_evd3 =  evd3;
  CHECK( j_evd3 == j_vd3_t );

  Eigen::Vector3i evi3( vi3.data() );
  ordered_json j_evi3 = evi3;
  CHECK( j_evi3 == j_vi3_t );

#if 0
  Eigen::Quaterniond qd( { 1.0, 2.0, 3.0, 4.0 } );
  json j_qd = qd;
  CHECK( j_qd["w"] == 1.0 );
  CHECK( j_qd["x"] == 2.0 );
  CHECK( j_qd["y"] == 3.0 );
  CHECK( j_qd["z"] == 4.0 );
#endif
}
