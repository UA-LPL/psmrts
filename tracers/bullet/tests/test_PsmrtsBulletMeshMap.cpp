
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsBulletMeshMap.hpp>

TEST_CASE ( "Bullet Mesh Map Test - Default Constructor", "[default][bullet][mapping]" ) {
    psmrts::bullet::PsmrtsBulletMeshMap b_map;

    CHECK ( b_map.isValid() == false );
    CHECK ( b_map.name() == "BulletMesh" );
    CHECK ( b_map.id() == 0 );
   // CHECK ( b_map.data()[0][0] == 0 );
   CHECK ( b_map.mesh() == nullptr );
   CHECK ( b_map.toBullet()[0] == 0 );

}