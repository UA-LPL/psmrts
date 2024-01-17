#define CATCH_CONFIG_MAIN
#include <psmrts_catch2_environment.hpp>

#include <NaifEllipsoidShape.hpp>

TEST_CASE( "NaifEllipsoidShape Default Test", "[naif][ellipsiod][default]") {

  naif::NaifEllipsoidShape t_ellipse;

  CHECK( t_ellipse.a()            == 1.0 );
  CHECK( t_ellipse.b()            == 1.0 );
  CHECK( t_ellipse.c()            == 1.0 );

  Eigen::Vector3d radii = t_ellipse.radii();
  CHECK( t_ellipse.a()            == radii[0] );
  CHECK( t_ellipse.b()            == radii[1] );
  CHECK( t_ellipse.c()            == radii[1] );
}
