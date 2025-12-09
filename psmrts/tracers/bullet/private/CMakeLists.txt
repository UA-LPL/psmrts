# Add Bullet classes to PSRMTS target

# This currently relies on NAIF 
target_sources(psmrts PRIVATE PsmrtsBulletClosestRayCallback.cpp
                              BulletTracerModel.cpp
                              PsmrtsBulletMeshMap.cpp
                              PsmrtsBulletAllHitsRayCallback.cpp
                              PsmrtsBulletWorldModel.cpp
)
                              