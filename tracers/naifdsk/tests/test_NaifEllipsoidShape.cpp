
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

  double lat_val = GENERATE( -90.0, -45.0, 0.0, 45.0, 90.0 );
  double long_val = GENERATE( -180.0, -90.0, 0.0, 45.0, 90.0, 180.0, 359.999 );

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

TEST_CASE( "NaifEllipsoidShape Maximum/Minimum Radius Test", "[naif][radius]" ) {
  const double tolerance = 1.0e-6;

  naif::NaifEllipsoidShape t_ellipse(1.0, 2.0, 3.0);

  CHECK_THAT ( t_ellipse.a() , Catch::Matchers::WithinAbs(1.0, tolerance));
  CHECK_THAT ( t_ellipse.b() , Catch::Matchers::WithinAbs(2.0, tolerance));
  CHECK_THAT ( t_ellipse.c() , Catch::Matchers::WithinAbs(3.0, tolerance));

  CHECK ( t_ellipse.c() == t_ellipse.maximum_radius() );
  CHECK ( t_ellipse.a() == t_ellipse.minimum_radius() );
}


TEST_CASE ( "NAIFEllipsoidShape Ray Trace Value Check", "[naif][ellipsoid][raytrace]" ) {

const double tolerance = 1.0e-6;

naif::NaifEllipsoidShape t_ellipse;

Eigen::Vector3d obs;
double radius = 1.0;
double obs_long = 45.0 * rpd_c();
double obs_lat = 45.0 * rpd_c();
latrec_c ( radius, obs_long, obs_lat, obs.data() );
obs = obs * 10.0;


Eigen::Vector3d surf;
double surf_long = 45.0 * rpd_c();
double surf_lat = 50.0 * rpd_c();
latrec_c ( radius, surf_long, surf_lat, surf.data() );


Eigen::Vector3d lkdr = obs - surf;

Eigen::Vector3d spt( { 0, 0, 0 } ) ;

bool good = t_ellipse.ray_trace(-obs, lkdr, spt);

CHECK ( good == true ); 
CHECK_THAT ( spt[0] , Catch::Matchers::WithinAbs(-surf[0], tolerance));
CHECK_THAT ( spt[1] , Catch::Matchers::WithinAbs(-surf[1], tolerance));
CHECK_THAT ( spt[2] , Catch::Matchers::WithinAbs(-surf[2], tolerance));

}

TEST_CASE ( "NAIFEllipsoidShape Ray Trace Value-Range Test", "[naif][raytrace][observer]") {
  const double tolerance = 1.0e-6;

  naif::NaifEllipsoidShape t_ellipse;

  double long_val = GENERATE( -180.0, -140.0, -90.0, -45.0, 0.0, 45.0, 90.0, 140.0, 180.0); // create range - should check for 360? -180 - 180
  double lat_val = GENERATE( -90.0, -45.0, 0.0, 45.0, 90.0 );// lat val -90 - 90.
  // Need to use 

  Eigen::Vector3d obs;
  double radius = 1.0;
  double obs_long = long_val * rpd_c();
  double obs_lat = lat_val * rpd_c();
  latrec_c ( radius, obs_long, obs_lat, obs.data() );
  obs = obs * 10;

  Eigen::Vector3d surf;
  double surf_long = 45.0 * rpd_c();
  double surf_lat = 50.0 * rpd_c(); 
  latrec_c ( radius, surf_long, surf_lat, surf.data() );

  Eigen::Vector3d lkdr = obs - surf;

  Eigen::Vector3d spt ( { 0, 0, 0, } );

  bool good = t_ellipse.ray_trace(-obs, lkdr, spt);
  // call surfpt_c w/ parameters, declare bool - true vs 0. Found == 0, not found / or 1 if found 

  Eigen::Vector3d naif_spt ( { 0, 0, 0, } );
  Eigen::Vector3d observer = -obs;

  SpiceBoolean found; 
  surfpt_c( observer.data(), lkdr.data(), t_ellipse.a(), t_ellipse.b(), t_ellipse.c(), naif_spt.data(), &found );


  CHECK ( good == (found == SPICETRUE ));

  INFO( "Lon/Lat = " << long_val << ", " << lat_val );

  CHECK ( good == true ); 
  if ( good ) {
    CHECK_THAT ( spt[0] , Catch::Matchers::WithinAbs( -surf[0], tolerance )); 
    CHECK_THAT ( spt[1] , Catch::Matchers::WithinAbs( -surf[1], tolerance ));
    CHECK_THAT ( spt[2] , Catch::Matchers::WithinAbs( -surf[2], tolerance ));
  }
  

}