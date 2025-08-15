# Add Bullet classes to PSRMTS target

# This currently relies on NAIF 
target_sources(psmrts PRIVATE BulletSystemModel.hpp
		                          PsmrtsBulletClosestRayCallback.hpp
                              BulletTracerModel.hpp
                              PsmrtsBulletMeshMap.hpp
                              PsmrtsBulletAllHitsRayCallback.hpp
                              PsmrtsBulletWorldModel.hpp )
                              