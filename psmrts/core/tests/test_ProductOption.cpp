#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <string>
#include <memory>
#include <exception>
#include <iostream>

#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>

#include <string>
#include <vector>

TEST_CASE ( "ProductOption JSON Util Tests", "[parameter][json][basics]") {

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


TEST_CASE ( "ProductOption Parameter Tests", "[parameter][json]") {
  psmrts::ProductOption v_parm( "radii", { 1.0, 2.0, 3.0 } );
  CHECK( v_parm.size() == 3 );

  std::vector<double> v_ds = { 1.0, 2.0, 3.0 };
  psmrts::ProductOption v_radii( "radii", v_ds );
  CHECK( v_radii.size() == 3 );

  psmrts::StringVisitor double_c( v_radii );
  CHECK( double_c.size() == 3 );
  CHECK( double_c.name() == "radii" );
  CHECK( double_c.type() == psmrts::ProductOption::PsmrtsDoubleArray );
  CHECK( double_c.get(0) == "1.000000000" );
  CHECK( double_c.get(1) == "2.000000000" );
  CHECK( double_c.get(2) == "3.000000000" );

  CHECK( v_radii.to_string( ) == v_parm.to_string() );
  CHECK( v_radii.to_json( )   == v_parm.to_json() );

  psmrts::ProductOption iv("integer", 2);
  CHECK( iv.to_string() == "2" );
  CHECK( iv.size() == 1 );

  psmrts::ProductOption s_array("strings", {"one","two","three"} );
  CHECK( s_array.size() == 3 );
  CHECK( s_array.to_string() == R"(["one","two","three"])" );

}


TEST_CASE( "ProductOption Parameter Constructor and Default Tests", "[parameter][constructor][default]") {
  CHECK(psmrts::ProductOption("bool", true).to_string() == "true");
  CHECK(psmrts::ProductOption("int", 42).to_string() == "42");
  CHECK(psmrts::ProductOption("double", 3.14).to_string().substr(0, 4) == "3.14");
  CHECK(psmrts::ProductOption("string", std::string("test")).to_string() == "test");
  CHECK(psmrts::ProductOption("intarray", {1, 2, 3}).to_string() == "[1,2,3]");
  CHECK(psmrts::ProductOption("dblarray", {1.1, 2.2, 3.3}).to_string().find("2.2") != std::string::npos);
  CHECK(psmrts::ProductOption("strarray", {"one", "two"}).to_string() == R"(["one","two"])");

  ordered_json j = { {"key", "val"} };
  CHECK(psmrts::ProductOption("json", j).to_json()["json"] == j);

  psmrts::ProductOption p;
  CHECK(p.name() == "false");
  CHECK(p.to_string() == "false");
  CHECK(p.size() == 1);

  psmrts::ProductOption empty_array("empty", std::vector<int>{});
  CHECK( empty_array.name() == "empty" );
  CHECK(empty_array.to_string() == "[]");
  CHECK(empty_array.size() == 0);
}


TEST_CASE( "ProductOption Parameter Values Tests", "[parameter][values]") {
  psmrts::ProductOption a("Bool", true);
  psmrts::ProductOption b("boOl", true); 
  CHECK(a.name()        == b.name());
  CHECK(a.to_string()   == b.to_string());
  CHECK(a.to_json()     == b.to_json());
  CHECK(a.size()        == b.size());
  psmrts::StringVisitor boolval(a);
  CHECK(boolval.name()  == a.name());
  CHECK(boolval.type()  == psmrts::ProductOption::PsmrtsBoolean);
  CHECK(boolval.size()  == 1 );
  CHECK(boolval.get(0)  == "true" );
  CHECK(boolval.array() == std::vector<std::string>({"true"}));
  CHECK(std::string(boolval.data()[0]) == "true");

  psmrts::ProductOption c("Integer", 42);
  psmrts::ProductOption d("integer", 42);
  CHECK(c.name()       == d.name());
  CHECK(c.to_string()  == d.to_string());
  CHECK(c.to_json()    == d.to_json());
  CHECK(c.size()       == d.size());
  psmrts::StringVisitor intval(c);
  CHECK(intval.name()  == c.name());
  CHECK(intval.type()  == psmrts::ProductOption::PsmrtsInteger);
  CHECK(intval.size()  == 1 );
  CHECK(intval.get(0)  == "42" );
  CHECK(intval.array() == std::vector<std::string>({"42"}));
  CHECK(std::string(intval.data()[0]) == "42");

  psmrts::ProductOption e("Pi", 3.14);
  psmrts::ProductOption f("pi", 3.14);
  CHECK(e.name()       == f.name());
  CHECK(e.to_string()  == f.to_string());
  CHECK(e.to_json()    == f.to_json());
  CHECK(e.size()       == f.size());
  psmrts::StringVisitor dblval(e);
  CHECK(dblval.name()  == e.name());
  CHECK(dblval.type()  == psmrts::ProductOption::PsmrtsDouble);
  CHECK(dblval.size()  == 1 );
  CHECK(dblval.get(0)  == "3.140000000" );
  CHECK(dblval.array() == std::vector<std::string>({"3.140000000"}));
  CHECK(std::string(dblval.data()[0]) == "3.140000000");

  psmrts::ProductOption g("Hello", "world");
  psmrts::ProductOption h("heLlO", std::string("world"));
  CHECK(g.name()       == h.name());
  CHECK(g.to_string()  == h.to_string());
  CHECK(g.to_json()    == h.to_json());
  CHECK(g.size()       == h.size());
  psmrts::StringVisitor strval(g);
  CHECK(strval.name()  == g.name());
  CHECK(strval.type()  == psmrts::ProductOption::PsmrtsString);
  CHECK(strval.size()  == 1 );
  CHECK(strval.get(0)  == "world" );
  CHECK(strval.array() == std::vector<std::string>({"world"}));
  CHECK(std::string(strval.data()[0]) == "world");

  std::vector<int> vals = {1, 2, 3};
  psmrts::ProductOption i("Numbers", vals);
  psmrts::ProductOption j("numBers", {1, 2, 3});
  CHECK(i.name()       == j.name());
  CHECK(i.to_string()  == j.to_string());
  CHECK(i.to_json()    == j.to_json());
  CHECK(i.size()       == j.size());
  psmrts::StringVisitor intvec(i);
  CHECK(intvec.name()  == i.name());
  CHECK(intvec.type()  == psmrts::ProductOption::PsmrtsIntegerArray);
  CHECK(intvec.size()  == 3 );
  CHECK(intvec.get(0)  == "1" );
  CHECK(intvec.array() == std::vector<std::string>({"1","2","3"}));
  CHECK(std::string(intvec.data()[0]) == "1");

  std::vector<double> dvals = {1.1, 2.2, 3.3};
  psmrts::ProductOption k("Coords", dvals);
  psmrts::ProductOption l("coords", {1.1, 2.2, 3.3});
  CHECK(k.name()       == l.name());
  CHECK(k.to_string()  == l.to_string());
  CHECK(k.to_json()    == l.to_json());
  CHECK(k.size()       == l.size());
  psmrts::StringVisitor dblvec(k);
  CHECK(dblvec.name()  == k.name());
  CHECK(dblvec.type()  == psmrts::ProductOption::PsmrtsDoubleArray);
  CHECK(dblvec.size()  == 3 );
  CHECK(dblvec.get(0)  == "1.100000000" );
  CHECK(dblvec.array() == std::vector<std::string>({"1.100000000","2.200000000","3.300000000"}));
  CHECK(std::string(dblvec.data()[0]) == "1.100000000");

  std::vector<std::string> svals = {"a", "b", "c"};
  psmrts::ProductOption m("Letters", svals);
  psmrts::ProductOption n("letters", {"a", "b", "c"});
  CHECK(m.name()       == n.name());
  CHECK(m.to_string()  == n.to_string());
  CHECK(m.to_json()    == n.to_json());
  CHECK(m.size()       == n.size());
  psmrts::StringVisitor strvec(m);
  CHECK(strvec.name()  == m.name());
  CHECK(strvec.type()  == psmrts::ProductOption::PsmrtsStringArray);
  CHECK(strvec.size()  == 3 );
  CHECK(strvec.get(0)  == "a" );
  CHECK(strvec.array() == std::vector<std::string>({"a","b","c"}));
  CHECK(std::string(strvec.data()[0]) == "a");

  ordered_json jvals = {{"key1", "value1"}, {"key2", 2}};
  psmrts::ProductOption o("JsonTest", jvals);
  psmrts::ProductOption p("jsontest", jvals);
  CHECK(o.name() == p.name());
  CHECK(o.to_string() == p.to_string());
  CHECK(o.to_json() == p.to_json());
  CHECK(o.size() == p.size());
  psmrts::StringVisitor jsonvec(o);
  CHECK(jsonvec.name()  == o.name());
  CHECK(jsonvec.type()  == psmrts::ProductOption::PsmrtsJsonObject);
  CHECK(jsonvec.size()  == 1 );
  CHECK(jsonvec.get(0)  == "{\"key1\":\"value1\",\"key2\":2}" );
  CHECK(jsonvec.array() == std::vector<std::string>({ "{\"key1\":\"value1\",\"key2\":2}" }));
  CHECK(std::string(jsonvec.data()[0]) == "{\"key1\":\"value1\",\"key2\":2}");

  Eigen::Vector3d v3dvals(1.0, 2.0, 3.0);
  psmrts::ProductOption q("Vec3D", v3dvals);
  psmrts::ProductOption r("vec3d", {1.0, 2.0, 3.0});
  CHECK(q.name()       == r.name());
  CHECK(q.to_string()  == r.to_string());
  CHECK(q.to_json()    == r.to_json());
  CHECK(q.size()       == r.size());
  psmrts::StringVisitor v3dvec(q);
  CHECK(v3dvec.name()  == q.name());
  CHECK(v3dvec.type()  == psmrts::ProductOption::PsmrtsDoubleArray);
  CHECK(v3dvec.size()  == 3 );
  CHECK(v3dvec.get(0)  == "1.000000000" );
  CHECK(v3dvec.array() == std::vector<std::string>({"1.000000000","2.000000000","3.000000000"}));
  CHECK(std::string(v3dvec.data()[0]) == "1.000000000");

  Eigen::Vector3i v3ivals(4, 5, 6);
  psmrts::ProductOption s("Vec3I", v3ivals);
  psmrts::ProductOption t("vec3i", {4, 5, 6});
  CHECK(s.name()       == t.name());
  CHECK(s.to_string()  == t.to_string());
  CHECK(s.to_json()    == t.to_json());
  CHECK(s.size()       == t.size());
  psmrts::StringVisitor v3ivec(s);
  CHECK(v3ivec.name()  == s.name());
  CHECK(v3ivec.type()  == psmrts::ProductOption::PsmrtsIntegerArray);
  CHECK(v3ivec.size()  == 3 );
  CHECK(v3ivec.get(0)  == "4" );
  CHECK(v3ivec.array() == std::vector<std::string>({"4","5","6"}));
  CHECK(std::string(v3ivec.data()[0]) == "4");

  std::vector<int> large_array(1000, 1);
  psmrts::ProductOption large_params("many", large_array);
  CHECK(large_params.size() == large_array.size() );
  CHECK(large_params.size() == 1000);
  CHECK(large_params.to_json()["many"].size() == 1000);

}
