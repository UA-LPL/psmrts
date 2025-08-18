
#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/tracers/naifdsk/private/NaifUtilities.hpp>
#include <psmrts/tracers/naifdsk/private/KernelFileSystem.hpp>



TEST_CASE ( "Kernel File System Test - kernel_info description return", "[kernel][info]" ) {
    std::string file = psmrts_tracers_path( "/naifdsk/data/orx_ocams_v07.ti" );
    
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK_NOTHROW ( naif::load_kernel(file) ); // Load the file/directory to be used
    CHECK_NOTHROW( naif::check_naif_errors() ); // Check for loading errors

    auto kdscr = naif::KernelFileSystem::kernel_info(file);

    CHECK ( kdscr.m_source_file == "" );
    CHECK ( kdscr.m_kernel_file == file );
    CHECK ( kdscr.m_handle == 0 );
    CHECK ( kdscr.m_found != 0 );

    CHECK ( kdscr.isValid() == true ); 
    CHECK ( kdscr.m_handle == kdscr.handle() ); 
    CHECK ( kdscr.filename() == file );

    CHECK_NOTHROW ( naif::KernelFileSystem::close_kernel( file )); // for Code Coverage *****
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::get_shared_descriptor( file )); // for Code Coverage *****
}

TEST_CASE ( "Kernel File System Test - kernel_count return", "[kernel][directory][count]" ) {
    std::string file = psmrts_tracers_path( "/naifdsk/data/orx_ocams_v07.ti");
    
    CHECK_NOTHROW ( naif::initKernelSystem() ); // Initializes the kernel system
    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK_NOTHROW ( naif::load_kernel(file) ); // Load the file/directory to be used
    CHECK_NOTHROW( naif::check_naif_errors() ); // Check for loading errors

    CHECK ( naif::KernelFileSystem::kernel_count() == 1  ); // Ensure that single file is what was loaded

    // Datatype checks
    CHECK ( naif::KernelFileSystem::kernel_count("IK") == 0  ); // File is of IK type (Kernels must include KPL/ ---  <--type)/ IK NOT A VALID TYPE
    CHECK ( naif::KernelFileSystem::kernel_count("TEXT") == 1  ); // File is also of text type
    CHECK ( naif::KernelFileSystem::kernel_count("SPK") == 0  );
    CHECK ( naif::KernelFileSystem::kernel_count("CK") == 0  );
    CHECK ( naif::KernelFileSystem::kernel_count("PCK") == 0  );
    CHECK ( naif::KernelFileSystem::kernel_count("DSK") == 0  );
    CHECK ( naif::KernelFileSystem::kernel_count("EK") == 0  );
    CHECK ( naif::KernelFileSystem::kernel_count("META") == 0  );
    CHECK ( naif::KernelFileSystem::kernel_count("ALL") == 1  ); // Checks for all types / total

    auto file_info = naif::KernelFileSystem::kernel_info( file );
    CHECK ( file_info.found() == true );

    CHECK ( naif::KernelFileSystem::safe_disposal_of( file ) == true ); // For Code Coverage ***** - needs to be a dsk file .bds, cannot be a .ti
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

}

TEST_CASE ( "Kernel File System Test - kernel_filetype_info return", "[kernel][filetype]") {
    std::string file = psmrts_tracers_path( "naifdsk/data/orx_ocams_v07.ti" ); 

    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK_NOTHROW ( naif::load_kernel(file) ); // Load the file/directory to be used
    CHECK_NOTHROW( naif::check_naif_errors() ); // Check for loading errors

    auto k_info = naif::KernelFileSystem::kernel_filetype_info( "ALL" );

    CHECK ( k_info.size() == 1 );
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

}