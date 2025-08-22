# Add Bullet classes to PSRMTS target

# This currently relies on NAIF 
target_sources(psmrts PRIVATE 	DskSegment.hpp
                                NaifUtilities.hpp
                                DskKernelModel.hpp
                                KernelFileSystem.hpp
)
