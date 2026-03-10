#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsTagSearch.hpp>
#include <psmrts/core/PsmrtsMetakernel.hpp>

TEST_CASE( "PsmrtsMetakernel Default Test", "[metakernel][default]") {
    psmrts::PsmrtsMetakernel mk("../psmrts/core/tests/data/orx_kernels.mk");

    // Counts
    CHECK( mk.getPathMap().size() == 3 );
    CHECK( mk.getKernels().size() == 9 );

    // Path map — symbol → resolved path
    CHECK( mk.getPathMap().at("data")      == "/opt/isis3/data"                   );
    CHECK( mk.getPathMap().at("osirisrex") == "/opt/isis3/data/osirisrex"         );
    CHECK( mk.getPathMap().at("apophis")   == "/opt/isis3/data/osirisrex/apophis" );

    // Kernels — spot-check first, last, and a middle entry
    CHECK( mk.getKernels().front() == "$osirisrex/kernels/lsk/naif0012.tls"        );
    CHECK( mk.getKernels().back()  == "$osirisrex/kernels/spk/orx_struct_v04.bsp"  );
    CHECK( mk.getKernels()[3]      == "$osirisrex/kernels/fk/orx_v14.tf"           );

    // Confirm commented-out kernel is NOT in the list
    CHECK( std::find(mk.getKernels().begin(), mk.getKernels().end(),
                     "$apophis/kernels/tspk/de440s.bsp") == mk.getKernels().end() );
}
