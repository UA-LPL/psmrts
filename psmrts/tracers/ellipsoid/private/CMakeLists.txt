# Add ellipsoid classes to PSRMTS target

# This currently relies on NAIF 
target_sources(psmrts PRIVATE EllipsoidTracerModel.cpp)