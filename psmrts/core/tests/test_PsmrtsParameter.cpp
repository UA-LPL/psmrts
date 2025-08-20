#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <string>
#include <memory>
#include <exception>
#include <iostream>

#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsParameter.hpp>

#include <string>
#include <vector>

TEST_CASE ( "PsmrtsParameter JSON Util Tests", "[parameter][json][basics]") {

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


TEST_CASE ( "PsmrtsParameter Parameter Tests", "[parameter][json]") {
  psmrts::PsmrtsParameter v_parm( "radii", { 1.0, 2.0, 3.0 } );
  CHECK( v_parm.size() == 3 );

  std::vector<double> v_ds = { 1.0, 2.0, 3.0 };
  psmrts::PsmrtsParameter v_radii( "radii", v_ds );
  CHECK( v_radii.size() == 3 );

  CHECK( v_radii.to_string( ) == v_parm.to_string() );
  CHECK( v_radii.to_json( )   == v_parm.to_json() );

  psmrts::PsmrtsParameter iv("integer", 2);
  CHECK( iv.to_string() == "2" );
  CHECK( iv.size() == 1 );

  psmrts::PsmrtsParameter s_array("strings", {"one","two","three"} );
  CHECK( s_array.size() == 3 );
  CHECK( s_array.to_string() == R"(["one","two","three"])" );

}


TEST_CASE( "PsmrtsParameter Parameter Constructor and Default Tests", "[parameter][constructor][default]") {
  CHECK(psmrts::PsmrtsParameter("bool", true).to_string() == "true");
  CHECK(psmrts::PsmrtsParameter("int", 42).to_string() == "42");
  CHECK(psmrts::PsmrtsParameter("double", 3.14).to_string().substr(0, 4) == "3.14");
  CHECK(psmrts::PsmrtsParameter("string", std::string("test")).to_string() == "\"test\"");
  CHECK(psmrts::PsmrtsParameter("intarray", {1, 2, 3}).to_string() == "[1,2,3]");
  CHECK(psmrts::PsmrtsParameter("dblarray", {1.1, 2.2, 3.3}).to_string().find("2.2") != std::string::npos);
  CHECK(psmrts::PsmrtsParameter("strarray", {"one", "two"}).to_string() == R"(["one","two"])");

  ordered_json j = { {"key", "val"} };
  CHECK(psmrts::PsmrtsParameter("json", j).to_json()["json"] == j);

  psmrts::PsmrtsParameter p;
  CHECK(p.name() == "false");
  CHECK(p.to_string() == "false");
  CHECK(p.size() == 1);

  psmrts::PsmrtsParameter empty_array("empty", std::vector<int>{});
  CHECK( empty_array.name() == "empty" );
  CHECK(empty_array.to_string() == "[]");
  CHECK(empty_array.size() == 0);
}


TEST_CASE( "PsmrtsParameter Parameter Values Tests", "[parameter][values]") {
  psmrts::PsmrtsParameter a("Bool", true);
  psmrts::PsmrtsParameter b("boOl", true); 
  CHECK(a.name() == b.name());
  CHECK(a.to_string() == b.to_string());
  CHECK(a.to_json() == b.to_json());
  CHECK(a.size() == b.size());

  psmrts::PsmrtsParameter c("Integer", 42);
  psmrts::PsmrtsParameter d("integer", 42);
  CHECK(c.name() == d.name());
  CHECK(c.to_string() == d.to_string());
  CHECK(c.to_json() == d.to_json());
  CHECK(c.size() == d.size());

  psmrts::PsmrtsParameter e("Pi", 3.14);
  psmrts::PsmrtsParameter f("pi", 3.14);
  CHECK(e.name() == f.name());
  CHECK(e.to_string() == f.to_string());
  CHECK(e.to_json() == f.to_json());
  CHECK(e.size() == f.size());

  psmrts::PsmrtsParameter g("Hello", "world");
  psmrts::PsmrtsParameter h("heLlO", std::string("world"));
  CHECK(g.name() == h.name());
  CHECK(g.to_string() == h.to_string());
  CHECK(g.to_json() == h.to_json());
  CHECK(g.size() == h.size());

  std::vector<int> vals = {1, 2, 3};
  psmrts::PsmrtsParameter i("Numbers", vals);
  psmrts::PsmrtsParameter j("numBers", {1, 2, 3});
  CHECK(i.name() == j.name());
  CHECK(i.to_string() == j.to_string());
  CHECK(i.to_json() == j.to_json());
  CHECK(i.size() == j.size());

  std::vector<double> dvals = {1.1, 2.2, 3.3};
  psmrts::PsmrtsParameter k("Coords", dvals);
  psmrts::PsmrtsParameter l("coords", {1.1, 2.2, 3.3});
  CHECK(k.name() == l.name());
  CHECK(k.to_string() == l.to_string());
  CHECK(k.to_json() == l.to_json());
  CHECK(k.size() == l.size());

  std::vector<std::string> svals = {"a", "b", "c"};
  psmrts::PsmrtsParameter m("Letters", svals);
  psmrts::PsmrtsParameter n("letters", {"a", "b", "c"});
  CHECK(m.name() == n.name());
  CHECK(m.to_string() == n.to_string());
  CHECK(m.to_json() == n.to_json());
  CHECK(m.size() == n.size());

  ordered_json jvals = {{"key1", "value1"}, {"key2", 2}};
  psmrts::PsmrtsParameter o("JsonTest", jvals);
  psmrts::PsmrtsParameter p("jsontest", jvals);
  CHECK(o.name() == p.name());
  CHECK(o.to_string() == p.to_string());
  CHECK(o.to_json() == p.to_json());
  CHECK(o.size() == p.size());

  Eigen::Vector3d v3dvals(1.0, 2.0, 3.0);
  psmrts::PsmrtsParameter q("Vec3D", v3dvals);
  psmrts::PsmrtsParameter r("vec3d", {1.0, 2.0, 3.0});
  CHECK(q.name() == r.name());
  CHECK(q.to_string() == r.to_string());
  CHECK(q.to_json() == r.to_json());
  CHECK(q.size() == r.size());

  Eigen::Vector3i v3ivals(4, 5, 6);
  psmrts::PsmrtsParameter s("Vec3I", v3ivals);
  psmrts::PsmrtsParameter t("vec3i", {4, 5, 6});
  CHECK(s.name() == t.name());
  CHECK(s.to_string() == t.to_string());
  CHECK(s.to_json() == t.to_json());
  CHECK(s.size() == t.size());

  std::vector<int> large_array(1000, 1);
  psmrts::PsmrtsParameter large_params("many", large_array);
  CHECK(p.size() == 1000);
  CHECK(p.to_json()["many"].size() == 1000);

}
