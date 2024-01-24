
#include <psmrts_catch2_environment.hpp>

#include <NaifEllipsoidShape.hpp>

TEST_CASE( "NaifEllipsoidShape Default Test", "[naif][ellipsoid][default]") {

  const double tolerance = 1.0e-6;

  naif::NaifEllipsoidShape t_ellipse;

  CHECK( t_ellipse.a()            == 1.0 );
  CHECK( t_ellipse.b()            == 1.0 );
  CHECK( t_ellipse.c()            == 1.0 );

  Eigen::Vector3d radii = t_ellipse.radii();
  CHECK( t_ellipse.a()            == radii[0] );
  CHECK( t_ellipse.b()            == radii[1] );
  CHECK( t_ellipse.c()            == radii[2] );

  Eigen::Vector3d vec;
  double radius    = 1.0;
  double latitude  = 45.0 * rpd_c();
  double longitude = 20.0 * rpd_c();
  latrec_c( radius, longitude, latitude, vec.data() );

  CHECK_THAT( vec[0], Catch::Matchers::WithinAbs(0.6644630244, tolerance ) );
  CHECK_THAT( vec[1], Catch::Matchers::WithinAbs(0.2418447626, tolerance ) );
  CHECK_THAT( vec[2], Catch::Matchers::WithinAbs( 0.7071067812, tolerance ) );

  double n_lat;
  double n_long;
  double n_rad;
  reclat_c( vec.data(), &n_rad, &n_long, &n_lat );
  CHECK_THAT( n_lat, Catch::Matchers::WithinAbs(latitude, tolerance));
  CHECK_THAT( n_long, Catch::Matchers::WithinAbs(longitude, tolerance ));
  CHECK_THAT( n_rad, Catch::Matchers::WithinAbs(radius, tolerance ));
}

TEST_CASE( "GENERATE Output Test", "[generate]") {
  
  const double tolerance = 1.0e-6;

  double lat_val = GENERATE( -90.0, -45.0, 0.0, 45.0, 90.0);
  double long_val = GENERATE( -180.0, -90.0, 0.0, 45.0, 90.0, 180.0, 359.999);

  if ( long_val > 180 ) { 
    long_val -= 360.0;
  }

  naif::NaifEllipsoidShape t_ellipse;
  CHECK( t_ellipse.a()            == 1.0 );
  CHECK( t_ellipse.b()            == 1.0 );
  CHECK( t_ellipse.c()            == 1.0 );

  Eigen::Vector3d radii = t_ellipse.radii();
  CHECK( t_ellipse.a()            == radii[0] );
  CHECK( t_ellipse.b()            == radii[1] );
  CHECK( t_ellipse.c()            == radii[2] );

  INFO( "Lon/Lat = " << long_val << ", " << lat_val );

  CHECK ( long_val == long_val );
  CHECK ( lat_val == lat_val );

  Eigen::Vector3d vec;
  double radius = 1.0;
  double latitude = lat_val * rpd_c();
  double longitude = long_val * rpd_c();
  latrec_c( radius, longitude, latitude, vec.data() );

  
  double n_lat;
  double n_long;
  double n_rad;
  reclat_c( vec.data(), &n_rad, &n_long, &n_lat );
  CHECK_THAT( n_lat, Catch::Matchers::WithinAbs(latitude, tolerance));
  CHECK_THAT( n_long, Catch::Matchers::WithinAbs(longitude, tolerance ));
  CHECK_THAT( n_rad, Catch::Matchers::WithinAbs(radius, tolerance ));
}