#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsUID.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/products/ProductModelDispatch.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>


TEST_CASE( "ProductModelDispatch Default / MissingProcessRequestHandler", "[ProductModelDispatch][Default]") {
    class BadProduct : public psmrts::MissingProcessRequestHandler {
      public:
        BadProduct() = default;
        ~BadProduct() = default;
        using UIDType = psmrts::PsmrtsUID::UIDType;

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
    psmrts::ProductModelDispatch<BadProduct> dispatch2( prod );
    psmrts::PsmrtsRequest request;

    CHECK( dispatch2.process(request) == false );
    CHECK( request.error_count()      == 1 );
    CHECK( request.errors_to_string() == "void::process(PsmrtsRequest) is not implemented/available!\n" );
    CHECK( request.was_invoked()      == false );
    CHECK( request.process_status()   == false );
}
