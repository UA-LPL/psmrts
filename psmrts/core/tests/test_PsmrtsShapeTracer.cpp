#include <psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>

#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>


TEST_CASE("PsmrtsTracer Default Test", "[tracer][default]") {
    const double tolerance_r = 1.0E-13;
    CHECK( sizeof( psmrts::PsmrtsTracer::Tracer ) <= 768 );

    psmrts::PsmrtsTracer tracer_t( psmrts::PsmrtsTracer::ellipsoid( { 0.283065,0.271215,0.249720 }, "Bennu" ) );
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );

    Eigen::Vector3d obs;
    double radius = 10.0;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );

    Eigen::Vector3d lookdir = -obs;
    psmrts::PRQRayTrace ray_t( obs, lookdir );
    CHECK( ray_t.name()           == "PRQRayTrace" );
    CHECK( ray_t.run_count()      == 0 );
    CHECK( ray_t.was_invoked()    == false );
    CHECK( ray_t.process_status() == false );
    CHECK( ray_t.error_count()    == 0 );

    // CHECK( ray_t.tracker().runtime_s()  == 0.0 );
    // CHECK( ray_t.tracker().runtime_ms()  == 0.0 );
    // CHECK( ray_t.tracker().runtime_ns()  == 0.1 );
    
    // Run a trace!
    bool status = tracer_t.process( ray_t );
    // CHECK( ray_t.tracker().runtime_ns()  == 0.1 );

    CHECK( ray_t.run_count()      == 1 );
    CHECK( ray_t.was_invoked()    == true );
    CHECK( ray_t.process_status() == true );
    CHECK( ray_t.error_count()    == 0 );

    CHECK( status                 == true );
    CHECK( ray_t.isValid()        == true );
    CHECK( ray_t.trace().hasHit() == true );

    CHECK_THAT( ray_t.trace().incidence( ray_t.trace() ), Catch::Matchers::WithinAbs(0.10622974872501688, tolerance_r));
    CHECK_THAT( ray_t.trace().emission(), Catch::Matchers::WithinAbs(0.10622974872501688, tolerance_r ));
    CHECK( ray_t.trace().phase( ray_t.trace() )      == 0.0 );

    psmrts::PRQFacet facet_t;
    status = tracer_t.process( facet_t );
    CHECK( facet_t.was_invoked()      == false );
    CHECK( status                     == false );
    CHECK( facet_t.process_status()   == false );    
    CHECK( facet_t.error_count()      == 1 );
    CHECK( facet_t.errors_to_string() == facet_t.errors_to_string() );
    CHECK_THROWS( facet_t.throw_errors() );

  }

  TEST_CASE("PsmrtsTracer Default Test", "[tracer][bullet][naifdsk]") {
    const double tolerance_r = 1.0E-13;

    CHECK( sizeof( psmrts::PsmrtsTracer::Tracer ) <= 768 );

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    psmrts::PsmrtsTracer bullet_t( psmrts::PsmrtsTracer::bullet( dskfile ) );
    psmrts::PsmrtsTracer naifdsk_t( psmrts::PsmrtsTracer::naifdsk( dskfile ) );

    Eigen::Vector3d obs;
    double radius = 10.0;
    double obs_long = 45.0 * rpd_c();
    double obs_lat = 45.0 * rpd_c();
    latrec_c ( radius, obs_long, obs_lat, obs.data() );

    Eigen::Vector3d lookdir = -obs;
    psmrts::PRQRayTrace ray_b( obs, lookdir );
    CHECK( ray_b.name()           == "PRQRayTrace" );
    CHECK( ray_b.run_count()      == 0 );
    CHECK( ray_b.process_status() == false );
    CHECK( ray_b.was_invoked()    == false );
    CHECK( ray_b.error_count()    == 0 );

    // Copy for bullet and naifdsk
    psmrts::PRQRayTrace ray_d = ray_b;

    // Run a trace!
    bool status_b = bullet_t.process( ray_b );
    // CHECK( ray_b.tracker().runtime_ns()  == 0.1 );

    CHECK( ray_b.run_count()      == 1 );
    CHECK( ray_b.process_status() == true );
    CHECK( ray_b.was_invoked()    == true );
    CHECK( ray_b.error_count()    == 0 );

    CHECK( status_b                 == true );
    CHECK( ray_b.isValid()        == true );
    CHECK( ray_b.trace().hasHit() == true );

    CHECK_THAT( ray_b.trace().incidence( ray_b.trace() ), Catch::Matchers::WithinAbs( 0.52690706564731504, tolerance_r ));
    CHECK_THAT( ray_b.trace().emission(),                 Catch::Matchers::WithinAbs( 0.52690706564731504, tolerance_r ));
    CHECK( ray_b.trace().phase( ray_b.trace() )            == 0.0 );


    bool status_d = naifdsk_t.process( ray_d );
    CHECK( status_d               ==  status_b );
    CHECK( ray_d.isValid()        == ray_b.isValid());
    CHECK( ray_d.trace().hasHit() == ray_b.trace().hasHit() );

    CHECK( ray_d.trace().incidence( ray_d.trace() )  == ray_b.trace().incidence( ray_b.trace() ) );
    CHECK( ray_d.trace().emission()                  == ray_b.trace().emission() );
    CHECK( ray_d.trace().phase( ray_d.trace() )      == ray_b.trace().phase( ray_b.trace() ));


    psmrts::PRQFacet facet_b ( ray_b.trace() );
    status_b = bullet_t.process( facet_b );
    CHECK( status_b                   == true );
    CHECK( facet_b.was_invoked()      == true );
    CHECK( facet_b.process_status()   == true );
    CHECK( facet_b.error_count()      == 0);
    CHECK_NOTHROW( facet_b.throw_errors() );

    psmrts::PRQFacet facet_d ( ray_d.trace() );
    status_d = naifdsk_t.process( facet_d );
    CHECK( status_d                   == true );
    CHECK( facet_d.was_invoked()      == true );
    CHECK( facet_d.process_status()   == true );
    CHECK( facet_d.error_count()      == 0);
    CHECK_NOTHROW( facet_d.throw_errors() );

    CHECK(facet_d.facet().m_indexes == facet_b.facet().m_indexes );
    CHECK(facet_d.facet().m_vector1 == facet_b.facet().m_vector1 );
    CHECK(facet_d.facet().m_vector2 == facet_b.facet().m_vector2 );
    CHECK(facet_d.facet().m_vector3 == facet_b.facet().m_vector3 );
    CHECK(facet_d.facet().m_normal == facet_b.facet().m_normal );

  }
