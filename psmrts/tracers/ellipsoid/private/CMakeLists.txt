# Add ellipsoid classes to PSRMTS target

# This currently relies on NAIF 
target_sources(psmrts PRIVATE EllipsoidTracer.cpp EllipsoidTracerModel.hpp )
