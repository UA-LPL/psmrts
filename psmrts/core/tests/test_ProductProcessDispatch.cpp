#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/ProductProcessDispatch.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>


TEST_CASE( "ProductProcessDispatch Default / MissingProcessRequestHandler", "[ProductProcessDispatch][Default]") {
    psmrts::ProductProcessDispatch<psmrts::MissingProcessRequestHandler> dispatch;

    psmrts::PRQRayTrace ray_req;

    CHECK( dispatch.process(ray_req) == false );
    CHECK( ray_req.was_invoked() == false ); // ?
    CHECK( ray_req.process_status() == false );
    CHECK( ray_req.error_count() > 0 );

    struct BadProduct {
        bool process(psmrts::PRQVersion &request) const {
            throw std::runtime_error("Test Error");
        }
    };

    BadProduct prod;
    psmrts::ProductProcessDispatch<BadProduct> dispatch2( prod );
    psmrts::PRQVersion request;

    CHECK( dispatch2.process(request) == false );
    CHECK( request.error_count()      == 1 );
    CHECK( request.errors_to_string() == "std::exception\n" );
    CHECK( request.was_invoked()      == true );
    CHECK( request.process_status()   == false );
}

TEST_CASE( "ProductProcessDispatch Values Test", "[ProductProcessDispatch][values]") {
    double tolerance = 1.0e-6;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::ObjShape obj_m( objfile );

    psmrts::ProductProcessDispatch<psmrts::ObjShape> shape( obj_m );

    psmrts::PRQFeatures feat;

    CHECK( shape.process(feat) == true );
    CHECK( feat.name() == "PRQFeatures" );
    CHECK( feat.error_count() == 0 );

    psmrts_json feat_json = feat.config();
    CHECK( feat_json.size() == 1 );

    /** 
    Eigen::Vector3d obs;
    double radius = 1.0;
    double obs_long = psmrts::degrees_to_radians( 45.0 );
    double obs_lat = psmrts::degrees_to_radians( 45.0 );
    latrec_c ( radius, obs_long, obs_lat, obs.data() );
    obs = obs * 10.0;

    Eigen::Vector3d surf;
    double surf_lon = 45.0 * rpd_c();
    double surf_lat = 50.0 * rpd_c();
    latrec_c ( radius, surf_lon, surf_lat, surf.data() );

    Eigen::Vector3d surf_obs = surf * (0.28 + 1.5);
    psmrts::PRQRayTrace prq_ray(surf_obs, -surf_obs );

    // Now compute expected/precise look vector from observer to surface intercept point
    Eigen::Vector3d lookdir = prq_ray.trace().xyz() - obs;
    */

    psmrts::PRQRayTrace ray;

    CHECK( shape.process(ray) == false );

}
