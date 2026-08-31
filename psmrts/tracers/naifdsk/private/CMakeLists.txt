# Add Bullet classes to PSRMTS target

# This currently relies on NAIF 
target_sources(psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES DskSegment.hpp
                                 DskKernelModel.hpp
                                 KernelFileSystem.hpp
)
