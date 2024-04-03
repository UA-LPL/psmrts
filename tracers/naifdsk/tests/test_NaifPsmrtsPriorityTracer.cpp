
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsTracerModel.hpp>
#include <PsmrtsShapeTracerAdapter.hpp>
#include <PsmrtsPriorityTracer.hpp>

#include <DskKernelModel.hpp>

// Test Default constructor for DskKernelModel
TEST_CASE ( "NAIF DSK Priority Tracer Test", "[naif][shape][priority][tracer][adapter]") {
    typedef psmrts::PsmrtsShapeTracerAdapter<naif::DskKernelModel>   NaifShapeTracer;

    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );
    
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK ( naif::KernelFileSystem::size()         == 0 ); // Should be zero, as we have yet to load any

    naif::DskKernelModel dsk( dskfile );
    std::shared_ptr<NaifShapeTracer> dsk_adapter(new NaifShapeTracer( dsk ) );
    CHECK_NOTHROW( naif::check_naif_errors() ); 
    REQUIRE( dsk_adapter.get() != nullptr );

    psmrts::PsmrtsPriorityTracer tracer_p( dsk_adapter->clone() );
    CHECK( dsk_adapter->use_count() == 5 );

    REQUIRE( tracer_p.find_model_by_id( dsk_adapter->shape_tracer_id() ) != nullptr );
    CHECK( dsk_adapter.use_count() == 1 );

    auto priority_t = tracer_p.find_model_by_id( dsk_adapter->shape_tracer_id() );
    REQUIRE( priority_t != nullptr );

    CHECK( dsk_adapter->tracer_model_type()  == priority_t->tracer_model_type() );
    CHECK( dsk_adapter->shape_tracer_id()    == priority_t->shape_tracer_id() );

    CHECK( dsk_adapter->tracer_model_name()  == priority_t->tracer_model_name() );
    CHECK( dsk_adapter->tracer_model_type()  == priority_t->tracer_model_type() );
    CHECK( dsk_adapter->shapefile()          == priority_t->shapefile() );

    CHECK( dsk_adapter->plate_count()        == priority_t->plate_count() );
    CHECK( dsk_adapter->vertex_count()       == priority_t->vertex_count() );


    // Release all the memory and reset NAIF
    dsk_adapter.reset();
    CHECK( dsk.use_count() == 4 );

    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK ( naif::KernelFileSystem::kernel_count() == 0 );
    CHECK ( naif::KernelFileSystem::size()         == 0 );
}