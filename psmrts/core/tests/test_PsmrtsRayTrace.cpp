#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <Eigen/Geometry>
#include <cspice/SpiceUsr.h>

#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>
#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>


// Add to readme for testing: (conditions for testing)
// doing any higher level raytrace testing, we're going to use ellipse as a baseline
// due to it being the simplest, and most commonly relevant, shape object to 
// test raytracing on. It is also ensured to be available at all levels of
// psmrts/testing.

TEST_CASE( "PsmrtsRayTrace Default Test", "[ray][trace][default]") { 
  // No parameters
  psmrts::PsmrtsRayTrace ray_basic;

  CHECK( ray_basic.hasHit()         == false );
  CHECK( ray_basic.observer()       == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ray_basic.lookdir()        == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ray_basic.normal()         == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ray_basic.raypt()         == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ray_basic.xyz()            == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ray_basic.radius()         == 0.0 );
  CHECK( ray_basic.slant_distance() == 0.0 );
  CHECK( psmrts::isnull( ray_basic.emission() ) == true ); 
  CHECK( ray_basic.plateid()        == -1 );
  CHECK( ray_basic.segment_number() == -1 );
  CHECK( ray_basic.datum().hasHit() == false );

}

TEST_CASE( "PsmrtsRayTrace Facet Test", "[ray][trace][facet][default]") {
  psmrts::PsmrtsRayTrace::FacetDatum d_facet;
  CHECK( d_facet.isValid() == false ); 
  CHECK( d_facet.m_indexes == Eigen::Vector3i { -1, -1, -1 } );
  CHECK( d_facet.m_vector1 == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_facet.m_vector2 == Eigen::Vector3d { 0.0, 0.0, 0.0 } ); 
  CHECK( d_facet.m_vector3 == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_facet.m_normal  == Eigen::Vector3d { 0.0, 0.0, 0.0 } );

  psmrts::PsmrtsRayTrace::ray_trace_datum d_ray;
  CHECK( d_ray.hasHit()   == false );
  CHECK( d_ray.m_observer == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_lookdir  == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_xyz      == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_normal   == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_plateid  == -1 );
  CHECK( d_ray.m_segment  == -1 );

  Eigen::Vector3d obs  = { 2.0, 2.0, 2.0 };
  Eigen::Vector3d surf = { 0.0, 0.5, 0.0 };
  Eigen::Vector3d lkdr = -obs + surf;
  // Reset datum with values
  d_ray.reset( obs, lkdr );
  CHECK( d_ray.hasHit()   == false );
  CHECK( d_ray.m_observer == Eigen::Vector3d { 2.0, 2.0, 2.0 } );
  CHECK( d_ray.m_lookdir  == (-obs + surf) );
  CHECK( d_ray.m_xyz      == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_normal   == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_plateid  == -1 );
  CHECK( d_ray.m_segment  == -1 );
  // Second reset without values, ensure default
  d_ray.reset();
  CHECK( d_ray.hasHit()   == false );
  CHECK( d_ray.m_observer == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_lookdir  == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_xyz      == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_normal   == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( d_ray.m_plateid  == -1 );
  CHECK( d_ray.m_segment  == -1 );
}

// Basic instantiation of a ray trace itself, making sure default conditions are set
// create another instantiation with values, then use that to create another to ensure each constructor works

// 2 more tests, one is for facet datum - default case, constructors, 
// second is for ray trace datum, constructors (obs, lkdir), reset, then retest (should no longer be valid)

// These are to be done later:
// For each tracer (ellipsoid), then check for facet in ellipsoid - should be false.
// Any mathematically based models should not have a facet.
// Then naifdsk tracers now with valid facets, pull naif mesh, and see if you can get same facet from dsk
// (can do similar with bullet, as well)

TEST_CASE( "PsmrtsRayTrace Baseline Values Test - Ellipsoid", "[ray][trace][values][ellipse]" ) {
  const double tolerance = 1.0e-6;

  psmrts::EllipsoidTracer e_tracer; // a, b, c default to 1.0 

  Eigen::Vector3d obs;
  double radius   = 1.0;  /// FIX THIS!!  e_tracer.maximum_radius();
  CHECK( radius   == 1.0 );
  double obs_long = psmrts::degrees_to_radians(45.0);
  double obs_lat  = psmrts::degrees_to_radians(45.0);
  latrec_c( radius, obs_long, obs_lat, obs.data() );
  obs = obs * 2.0;
  CHECK_THAT( obs[0], Catch::Matchers::WithinAbs( 1.00000000000000022, tolerance ));
  CHECK_THAT( obs[1], Catch::Matchers::WithinAbs( 1.0, tolerance ));
  CHECK_THAT( obs[2], Catch::Matchers::WithinAbs( 1.41421356237309492, tolerance ));

  Eigen::Vector3d surf;
  double surf_long = psmrts::degrees_to_radians(45.0);
  double surf_lat  = psmrts::degrees_to_radians(45.0);
  latrec_c ( radius, surf_long, surf_lat, surf.data() );

  Eigen::Vector3d lkdr = obs - surf;

  psmrts::PsmrtsRayTrace ellipse_ray;

  CHECK( e_tracer.ray_trace( -obs, lkdr, ellipse_ray) == true );
  CHECK( ellipse_ray.hasHit()   == true ); // If valid, other functions should be as well.
  CHECK( ellipse_ray.observer() == -obs );
  CHECK( ellipse_ray.lookdir()  == lkdr );
  CHECK( ellipse_ray.raypt()   == ellipse_ray.xyz() + obs ); // minus -obs?
  CHECK_THAT( ellipse_ray.normal()[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( ellipse_ray.normal()[1], Catch::Matchers::WithinAbs( -0.5, tolerance )); 
  CHECK_THAT( ellipse_ray.normal()[2], Catch::Matchers::WithinAbs( -0.707107, tolerance )); 
  CHECK_THAT( ellipse_ray.xyz()[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( ellipse_ray.xyz()[1], Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( ellipse_ray.xyz()[2], Catch::Matchers::WithinAbs( -0.707107, tolerance ));
  CHECK_THAT( ellipse_ray.radius(), Catch::Matchers::WithinAbs( 1.0, tolerance ));
  CHECK_THAT( ellipse_ray.slant_distance(), Catch::Matchers::WithinAbs( 1.0, tolerance ));
  CHECK_THAT( ellipse_ray.emission(), Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK( ellipse_ray.plateid()        == -1 );
  CHECK( ellipse_ray.segment_number() == -1 ); 
  CHECK( psmrts::isnull(ellipse_ray.emission()) ==  false );   

  // Constructor initialized with another ray as parameter
  psmrts::PsmrtsRayTrace second_ray( ellipse_ray );

  CHECK( second_ray.hasHit()   == true ); 
  CHECK( second_ray.observer() == -obs );
  CHECK( second_ray.lookdir()  == lkdr ); // NAN (I/O)
  CHECK( second_ray.raypt()   == ellipse_ray.xyz() + obs ); // minus -obs?
  CHECK_THAT( second_ray.normal()[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( second_ray.normal()[1], Catch::Matchers::WithinAbs( -0.5, tolerance )); 
  CHECK_THAT( second_ray.normal()[2], Catch::Matchers::WithinAbs( -0.707107, tolerance )); 
  CHECK_THAT( second_ray.xyz()[0], Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( second_ray.xyz()[1], Catch::Matchers::WithinAbs( -0.5, tolerance ));
  CHECK_THAT( second_ray.xyz()[2], Catch::Matchers::WithinAbs( -0.707107, tolerance ));
  CHECK_THAT( second_ray.radius(), Catch::Matchers::WithinAbs( 1.0, tolerance )); 
  CHECK_THAT( second_ray.slant_distance(), Catch::Matchers::WithinAbs( 1.0, tolerance ));
  CHECK_THAT( second_ray.emission(), Catch::Matchers::WithinAbs( 0.0, tolerance ));
  CHECK( second_ray.plateid()        == -1 );
  CHECK( second_ray.segment_number() == -1 ); 
  CHECK( psmrts::isnull(second_ray.emission()) ==  false );   

  // Reset with new obs / lkdr values
  Eigen::Vector3d obs_reset;
  double obs_long_r = psmrts::degrees_to_radians(44.5);
  double obs_lat_r = psmrts::degrees_to_radians(44.5);
  latrec_c( radius, obs_long_r, obs_lat_r, obs_reset.data() );
  obs_reset = obs_reset * 2.0;

  Eigen::Vector3d surf_reset;
  double surf_long_r = psmrts::degrees_to_radians(44.5);
  double surf_lat_r = psmrts::degrees_to_radians(44.5);
  latrec_c( radius, surf_long_r, surf_lat_r, surf_reset.data());
  Eigen::Vector3d lkdr_reset = obs_reset - surf_reset;

  CHECK_NOTHROW( ellipse_ray.reset( -obs_reset, lkdr_reset ) );
  psmrts::PsmrtsRayTrace reset_ray;
  e_tracer.ray_trace( ellipse_ray.observer(), ellipse_ray.lookdir(), reset_ray);
  CHECK( reset_ray.hasHit()   == true ); 
  CHECK( reset_ray.observer() == -obs_reset );
  CHECK( reset_ray.lookdir()  == lkdr_reset );
  CHECK_THAT( reset_ray.normal()[0], Catch::Matchers::WithinAbs( -0.508726, tolerance ));
  CHECK_THAT( reset_ray.normal()[1], Catch::Matchers::WithinAbs( -0.499924, tolerance )); 
  CHECK_THAT( reset_ray.normal()[2], Catch::Matchers::WithinAbs( -0.700909, tolerance )); 
  CHECK_THAT( reset_ray.xyz()[0], Catch::Matchers::WithinAbs( -0.508726, tolerance ));
  CHECK_THAT( reset_ray.xyz()[1], Catch::Matchers::WithinAbs( -0.499924, tolerance ));
  CHECK_THAT( reset_ray.xyz()[2], Catch::Matchers::WithinAbs( -0.700909, tolerance ));
  CHECK( reset_ray.raypt()          == reset_ray.xyz() + obs_reset );
  CHECK( reset_ray.radius()         == 1.0 ); 
  CHECK( reset_ray.slant_distance() == 1.0 ); 
  CHECK( reset_ray.emission()       == 0.0 ); 
  CHECK( reset_ray.plateid()        == -1 );
  CHECK( reset_ray.segment_number() == -1 );
  CHECK( psmrts::isnull(reset_ray.emission()) ==  false ); 


  // Full reset to default
  CHECK_NOTHROW( ellipse_ray.reset() );
  CHECK( ellipse_ray.hasHit()   == false );
  CHECK( ellipse_ray.observer() == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ellipse_ray.lookdir()  == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ellipse_ray.normal()   == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ellipse_ray.raypt()    == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ellipse_ray.xyz()      == Eigen::Vector3d { 0.0, 0.0, 0.0 } );
  CHECK( ellipse_ray.radius()   == 0.0 );
  CHECK( ellipse_ray.slant_distance() == 0.0 );
  CHECK( psmrts::isnull( ellipse_ray.emission() ) == true ); 
  CHECK( ellipse_ray.plateid()        == -1 );
  CHECK( ellipse_ray.segment_number() == -1 );
  CHECK( ellipse_ray.datum().hasHit() == false );
}   


TEST_CASE("Ray Trace - Basic DSK Equivalent.", "[ray][trace][dsk]") {
  const double tolerance = 1.0e-6;
  std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
  naif::DskKernelModel dsk( dskfile );
  naif::DskSegment segment = dsk.segment();
  psmrts::PsmrtsRayTrace dsk_ray;

  Eigen::Vector3d obs;
  double radius = segment.maximum_radius();
  double obs_long = 90.0 * rpd_c();
  double obs_lat = 45.0 * rpd_c();
  latrec_c( radius, obs_long, obs_lat, obs.data() );
  obs = obs * 10;

  Eigen::Vector3d surf;
  double surf_long = 90.0 * rpd_c();
  double surf_lat = 45.0 * rpd_c();
  latrec_c( radius, surf_long, surf_lat, surf.data() );

  Eigen::Vector3d lkdr = -obs + surf;
  auto ray_trace = dsk.ray_trace( obs, lkdr, dsk_ray ); 
  
  REQUIRE ( dsk_ray.hasHit() == true );
  CHECK( dsk_ray.observer() == obs );
  CHECK ( dsk_ray.lookdir() == lkdr );

  CHECK_THAT ( dsk_ray.normal()[0], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
  CHECK_THAT ( dsk_ray.normal()[1], Catch::Matchers::WithinAbs( 0.5257310809, tolerance ) );
  CHECK_THAT ( dsk_ray.normal()[2], Catch::Matchers::WithinAbs( 0.8506508276, tolerance ) );

  CHECK_THAT ( dsk_ray.raypt()[0], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
  CHECK_THAT ( dsk_ray.raypt()[1], Catch::Matchers::WithinAbs( -1.8381441539, tolerance ) );
  CHECK_THAT ( dsk_ray.raypt()[2], Catch::Matchers::WithinAbs( -1.8381441539, tolerance ) );

  CHECK_THAT ( dsk_ray.xyz()[0], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
  CHECK_THAT ( dsk_ray.xyz()[1], Catch::Matchers::WithinAbs( 0.1634276563, tolerance ) );
  CHECK_THAT ( dsk_ray.xyz()[2], Catch::Matchers::WithinAbs( 0.1634276563, tolerance ) );

  CHECK ( dsk_ray.radius() == dsk_ray.xyz().norm() );
  CHECK_THAT ( dsk_ray.radius(), Catch::Matchers::WithinAbs( 0.231121608, tolerance) ); // DSK Radii known to have inherent bug complications

  CHECK_THAT ( dsk_ray.slant_distance(), Catch::Matchers::WithinAbs( 2.599528392, tolerance ) );
  CHECK ( dsk_ray.slant_distance() == dsk_ray.raypt().norm() );


  Eigen::Vector3d obs2;
  double obs2_long = 90.5 * rpd_c();
  double obs2_lat = 44.5 * rpd_c(); 
  latrec_c( radius, obs2_long, obs2_lat, obs2.data() );

  Eigen::Vector3d surf2;
  double surf2_long = 91.0 * rpd_c();
  double surf2_lat = 46.0 * rpd_c();
  latrec_c( radius, surf2_long, surf2_lat, surf2.data() );

  Eigen::Vector3d lkdr2 = -obs + surf2;
  psmrts::PsmrtsRayTrace dsk_ray2;
  auto ray_trace2 = dsk.ray_trace(obs, lkdr2, dsk_ray2);

  REQUIRE( dsk_ray2.hasHit() == true );
  CHECK_THAT( dsk_ray.distance(dsk_ray2), Catch::Matchers::WithinAbs( 0.0062732157, tolerance ) );
  CHECK_THAT ( dsk_ray.separation_angle(surf, surf2), Catch::Matchers::WithinAbs( vsep_c(surf.data(), surf2.data()), tolerance ));

  CHECK ( dsk_ray.isNear(dsk_ray2) == false );

  CHECK_THAT ( dsk_ray.incidence(dsk_ray2), Catch::Matchers::WithinAbs( 0.2337750681, tolerance ) ); 

  CHECK_THAT (dsk_ray.emission(), Catch::Matchers::WithinAbs( 0.2318238412, tolerance) );

  double emission_angle = dpr_c() * 0.2318238412;
  CHECK_THAT ( emission_angle, Catch::Matchers::WithinAbs( 13.2825276913, tolerance ) );

}
