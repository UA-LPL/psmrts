# Add Bullet classes to PSRMTS target

# This currently relies on NAIF 
target_sources(psmrts PRIVATE PsmrtsBulletClosestRayCallback.cpp
                              BulletTracer.cpp
                              BulletTracerModel.cpp
                              PsmrtsBulletMeshMap.cpp
                              PsmrtsBulletWorldModel.cpp
                              BulletSystemModel.hpp			
                              PsmrtsBulletClosestRayCallback.hpp
                              BulletTracerModel.hpp
                              PsmrtsBulletMeshMap.hpp
                              PsmrtsBulletWorldModel.hpp
)
