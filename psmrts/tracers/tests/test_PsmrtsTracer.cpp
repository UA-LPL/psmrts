
#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>
#include <psmrts/tracers/bullet/BulletTracer.hpp>
#include <psmrts/tracers/naifdsk/NaifDskTracer.hpp>

TEST_CASE("PsmrtsTracer Default / Validity Test", "[tracer][default]") {
    psmrts::PsmrtsTracer tracer;
    CHECK( tracer.isValid()      == false );

    psmrts::PsmrtsTracer sphere = psmrts::PsmrtsTracer::sphere( 1.0 );
    CHECK( sphere.isValid()      == true  );

    psmrts::PsmrtsTracer spheroid = psmrts::PsmrtsTracer::spheroid( 1.0, 2.0 );
    CHECK( spheroid.isValid()    == true  );

    psmrts::PsmrtsTracer ellipsoid = psmrts::PsmrtsTracer::ellipsoid( 1.0, 2.0, 3.0 ); 
    CHECK( ellipsoid.isValid()   == true  );

    Eigen::Vector3d vec({123.0, 456.0, 789.0}); 
    psmrts::PsmrtsTracer ellipsoid_v = psmrts::PsmrtsTracer::ellipsoid( vec );
    CHECK( ellipsoid_v.isValid() == true  );

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
    std::string plyfile = psmrts_shapes_path( "ply/data/Bennu_Radar.ply"    );

    psmrts::PsmrtsTracer bullet_obj = psmrts::PsmrtsTracer::bullet( objfile );
    CHECK( bullet_obj.isValid()  == true  );

    psmrts::PsmrtsTracer bullet_dsk = psmrts::PsmrtsTracer::bullet( dskfile );
    CHECK( bullet_dsk.isValid()  == true  );

    psmrts::PsmrtsTracer bullet_ply = psmrts::PsmrtsTracer::bullet( plyfile );
    CHECK( bullet_ply.isValid()  == true  );

    psmrts::PsmrtsTracer naifdsk = psmrts::PsmrtsTracer::naifdsk( dskfile );
    CHECK( naifdsk.isValid()     == true  );
}
