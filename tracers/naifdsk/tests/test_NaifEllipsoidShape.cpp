#define CATCH_CONFIG_MAIN
#include <psmrts_catch2_environment.hpp>

#include <NaifEllipsoidShape.hpp>

TEST_CASE( "NaifEllipsoidShape Default Test", "[naif][ellipsiod][default]") {

  const double tolerance = 1.0e-6;

  naif::NaifEllipsoidShape t_ellipse;

  CHECK( t_ellipse.a()            == 1.0 );
  CHECK( t_ellipse.b()            == 1.0 );
  CHECK( t_ellipse.c()            == 1.0 );

  Eigen::Vector3d radii = t_ellipse.radii();
  CHECK( t_ellipse.a()            == radii[0] );
  CHECK( t_ellipse.b()            == radii[1] );
  CHECK( t_ellipse.c()            == radii[1] );

  Eigen::Vector3d vec;
  double radius    = 1.0;
  double latitude  = 45.0 * rpd_c();
  double longitude = 20.0 * rpd_c();
  latrec_c( radius, longitude, latitude, vec.data() );

  CHECK_THAT( vec[0], Catch::Matchers::WithinAbs(0.6644630244, tolerance ) );
  CHECK_THAT( vec[1], Catch::Matchers::WithinAbs(0.2418447626, tolerance ) );
  CHECK_THAT( vec[2], Catch::Matchers::WithinAbs( 0.7071067812, tolerance ) );

}
