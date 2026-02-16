#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductProcessDispatch.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>
#include <psmrts/core/ProductSpecification.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>


TEST_CASE( "ProductProcessDispatch Default / MissingProcessRequestHandler", "[ProductProcessDispatch][Default]") {
#if 0  
    psmrts::ProductProcessDispatch<psmrts::MissingProcessRequestHandler> dispatch;

    psmrts::PRQRayTrace ray_req;

    CHECK( dispatch.process(ray_req) == false );
    CHECK( ray_req.was_invoked() == false ); // ?
    CHECK( ray_req.process_status() == false );
    CHECK( ray_req.error_count() > 0 );
#endif
    struct BadProduct {
        using UIDType = psmrts::PsmrtsUID::UIDType;
        bool process(psmrts::PRQVersion &request) const {
            throw std::runtime_error("Test Error");
        }

        inline const std::string &name() const {
          static std::string m_name("BadProduct");
          return ( m_name );
        }

        inline const std::string type() const {
          static std::string m_type("bandone");
          return ( m_type );
        }

        inline const psmrts::PsmrtsUID::UIDType &uid() const {
          return ( psmrts::PsmrtsUID::null_uid() );
        }
    };

    BadProduct prod;
    psmrts::ProductProcessDispatch<BadProduct> dispatch2( prod );
    psmrts::PRQVersion request;

    CHECK( dispatch2.process(request) == false );
    CHECK( request.error_count()      == 1 );
    CHECK( request.errors_to_string() == "Test Error\n" );
    CHECK( request.was_invoked()      == true );
    CHECK( request.process_status()   == false );
}

#if 0
TEST_CASE( "ProductProcessDispatch Values Test", "[ProductProcessDispatch][values]") {
    double tolerance = 1.0e-6;

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::PsmrtsShape shape_s(  objfile );

    // psmrts::ProductProcessDispatch<psmrts::PsmrtsShape> shape( shape_s );

    psmrts::PRQFeatures feat;

    CHECK( shape_s.process(feat) == true );
    CHECK( feat.name() == "PRQFeatures" );
    CHECK( feat.error_count() == 0 );

    psmrts_json feat_json = feat.config();
    CHECK( feat_json.size() != 0 );

    // Uninitialized ray trace should fail.
    psmrts::PRQRayTrace ray;
    // CHECK( shape_s.process(ray) == false );

}
#endif