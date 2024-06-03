
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>
#include <PsmrtsBulletMeshMap.hpp>
#include <PsmrtsBulletWorldModel.hpp>


typedef psmrts::bullet::BulletInternalMeshModel              BulletMeshModel;
typedef psmrts::bullet::PsmrtsBulletMeshMap<BulletMeshModel> BulletShape;

TEST_CASE ( "Bullet World Test - Default Constructor", "[default][bullet][world]" ) {
    psmrts::bullet::PsmrtsBulletWorldModel b_world;

    CHECK ( b_world.isValid() == false );
    CHECK ( b_world.name() == "Body-Fixed-Coordinate-System" );
}

// Load a mesh and inspect contents of Bullet world

// Load mesh and run some traces