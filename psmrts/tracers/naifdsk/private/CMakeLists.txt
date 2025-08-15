# Add Bullet classes to PSRMTS target

# This currently relies on NAIF 
target_sources(psmrts PRIVATE 	DskSegment.hpp
                                NaifDskTracerModel.hpp
                                NaifUtilities.hpp
                                DskKernelModel.hpp
                                KernelFileSystem.hpp
                                NaifEllipsoidShape.hpp
)
