
#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsFactory.hpp>

#include <psmrts/tracers/naifdsk/private/NaifUtilities.hpp>
#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>
#include <psmrts/tracers/naifdsk/NaifDskTracer.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>

#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/PsmrtsPriorityTracer.hpp>

TEST_CASE("PsmrtsPriorityTracer Default Test", "[priority][tracer][default]") {
  psmrts::PsmrtsFactory().liquidate();

  psmrts::PsmrtsPriorityTracer test_tracer;
  std::vector<std::string> myString;

  // This sizeof() is not the same on all OSes, tracks sizes priority tracers
  // CHECK( sizeof( psmrts::PsmrtsPriorityTracer ) == 336 );
  
  CHECK( test_tracer.isValid()                == false );
  CHECK( test_tracer.size()                   == 0 );
  psmrts::PsmrtsFactory().liquidate();

}

// Test Default constructor for DskKernelModel
TEST_CASE ( "NAIF DSK Priority Tracer Test", "[naif][shape][priority][tracer]") {
  psmrts::PsmrtsFactory().liquidate();

  std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
  
  psmrts::PsmrtsTracer dsk_tracer( psmrts::PsmrtsTracer::naifdsk( dskfile ) );
  psmrts::PsmrtsPriorityTracer p_tracer( dsk_tracer );
  CHECK( p_tracer.isValid() == true );
  CHECK( p_tracer.size()    == 1 );

  Eigen::Vector3d obs;
  double radius = 1.0;
  double obs_long = 45.0 * rpd_c();
  double obs_lat = 45.0 * rpd_c();
  latrec_c( radius, obs_long, obs_lat, obs.data() );
  obs = obs * 10.0;

  Eigen::Vector3d surf;
  double surf_long = 45.0 * rpd_c();
  double surf_lat = 50.0 * rpd_c();
  latrec_c( radius, surf_long, surf_lat, surf.data() );

  Eigen::Vector3d lkdr = surf - obs;
  psmrts::PRQRayTrace ray(obs, lkdr);
  CHECK( p_tracer.process( ray ) == true );
  psmrts::PsmrtsFactory().liquidate();

}


TEST_CASE( "Naif Priority Tracer Default Test", "[priority][tracer][naif]") {
  const double tolerance = 1.0e-6;
  psmrts::PsmrtsFactory().liquidate();

  psmrts::EllipsoidTracer s_ellipse( 1.0, 2.0, 3.0, "small" );
  psmrts::EllipsoidTracer l_ellipse( 10.0, 20.0, 30.0, "large" );

  Eigen::Vector3d obs;
  double radius   = 1.0;
  double obs_long = 45.0 * rpd_c();
  double obs_lat  = 45.0 * rpd_c();
  latrec_c( radius, obs_long, obs_lat, obs.data() );
  obs = obs * 10.0;

  Eigen::Vector3d surf;
  double surf_long = 45.0 * rpd_c();
  double surf_lat  = 50.0 * rpd_c();
  latrec_c( radius, surf_long, surf_lat, surf.data() );

  Eigen::Vector3d lkdr = surf - obs;
  psmrts::PsmrtsRayTrace small_spt;
  psmrts::PsmrtsRayTrace large_spt;
  REQUIRE( s_ellipse.ray_trace(obs, lkdr, small_spt) == true );
  REQUIRE( l_ellipse.ray_trace(obs, lkdr, large_spt) == true );

  psmrts::PsmrtsPriorityTracer test_tracers;
  CHECK( test_tracers.isValid() == false );

  test_tracers.add_tracer( psmrts::PsmrtsTracer( s_ellipse ) );
  test_tracers.add_tracer( psmrts::PsmrtsTracer( l_ellipse ) );

  CHECK( test_tracers.isValid() == true );
  CHECK( test_tracers.size()    == 2 );

  test_tracers.clear();
  REQUIRE( test_tracers.size() == 0 );

  psmrts::PsmrtsFactory().liquidate();
}

TEST_CASE( "Priority Tracer Ray Trace Test", "[priority][tracer][dsk][naif]") {

  psmrts::PsmrtsFactory().liquidate();

  // Objects added to Tracer
  psmrts::EllipsoidTracer small_ellipsoid( 0.1 ); //Small body radius: 0.1 KM

  std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" ); // Avg Radius: 0.25 KM
  naif::DskKernelModel dsk( dskfile );

  Eigen::Vector3d obs = {0.0, 10.0, 0.0};
  /*
  double radius = 0.1;
  double obs_long = 45.0 * rpd_c();
  double obs_lat = 45.0 * rpd_c();
  latrec_c ( radius, obs_long, obs_lat, obs.data() );
  obs = obs * 10.0;   
  */
  Eigen::Vector3d lkdr = -obs + Eigen::Vector3d({0.0, 0.0, 0.10});
  
  double lat, lon, rad;
  reclat_c(obs.data(), &rad, &lon, &lat); 
  lon = lon * dpr_c(); 
  lat = lat * dpr_c();
  CHECK( rad == 10 );
  CHECK( lon == 90 );
  CHECK( lat == 0 );

  // Tracer settup
  psmrts::PsmrtsPriorityTracer test_tracers;
  test_tracers.add_tracer( psmrts::PsmrtsTracer( small_ellipsoid ));
  test_tracers.add_tracer( psmrts::PsmrtsTracer( dsk ) );

  psmrts::PRQRayTrace ray( obs, lkdr);

  bool success = test_tracers.process(ray);
  CHECK( success       == true );
  CHECK( ray.isValid() == true );
  
  // loop through cases, if z value in lkdr for any values greater than ellipsoid radius (0.1)
  // should return ptr at bennu facet.
  Eigen::Vector3d observer;
  double radius = 0.1;
  double obs_long = 90.0 * rpd_c();// Direct View
  double obs_lat = 0.0 * rpd_c();
  latrec_c ( radius, obs_long, obs_lat, observer.data() );
  observer = observer * 10.0;  
  
  std::vector<double> z_list = {0.05, 0.08, 0.10, 0.11, 0.12, 0.15, 0.20};

  psmrts::PsmrtsPriorityTracer next_tracer;
  next_tracer.add_tracer( psmrts::PsmrtsTracer( small_ellipsoid ) );
  next_tracer.add_tracer( psmrts::PsmrtsTracer( dsk ) );

  for ( auto z_value: z_list ) {
      Eigen::Vector3d lkdr = -observer + Eigen::Vector3d({0.0, 0.0, z_value});
      psmrts::PRQRayTrace raytrace( observer, lkdr);
      bool success = next_tracer.process(raytrace);
      CHECK( success            == true);
      CHECK( raytrace.isValid() == true);
  }

  next_tracer.clear();
  CHECK ( next_tracer.size() == 0 );
  next_tracer.add_tracer( psmrts::PsmrtsTracer( dsk ) );
  next_tracer.add_tracer( psmrts::PsmrtsTracer( small_ellipsoid ) );
  
  for ( auto const &z_value: z_list ) {
      Eigen::Vector3d lkdr = -observer + Eigen::Vector3d({0.0, 0.0, z_value});
      psmrts::PRQRayTrace raytrace( observer, lkdr);
      bool success = next_tracer.process(raytrace);
      CHECK( success            == true);
      CHECK( raytrace.isValid() == true);
      // Order of adding the tracer matters, should always intercept the dsk in this case.
      // We can check this by getting the fact and checking for existance
      psmrts::PRQFacet facet( raytrace );
      success = next_tracer.process( facet );
      CHECK (facet.isValid()  == true ); 

  }

  next_tracer.clear();
  REQUIRE ( next_tracer.size() == 0);

  // Check for empty tracer - no priority
  for ( auto const &z_value: z_list ) {
      Eigen::Vector3d lkdr = -observer + Eigen::Vector3d({0.0, 0.0, z_value});
      psmrts::PRQRayTrace raytrace( observer, lkdr);
      bool success = next_tracer.process(raytrace);
      CHECK( success            == false);
      CHECK( raytrace.isValid() == false);
  }
  
  psmrts::PsmrtsFactory().liquidate();
}
