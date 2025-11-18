# Add Bullet classes to PSRMTS target

# This currently relies on NAIF 
target_sources(psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES BulletSystemModel.hpp
		                          PsmrtsBulletClosestRayCallback.hpp
                              BulletTracerModel.hpp
                              PsmrtsBulletMeshMap.hpp
                              PsmrtsBulletAllHitsRayCallback.hpp
                              PsmrtsBulletWorldModel.hpp )
                              