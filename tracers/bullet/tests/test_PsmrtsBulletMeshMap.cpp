
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsBulletMeshMap.hpp>
#include <PsmrtsOBJAsset.hpp>


TEST_CASE ( "Bullet Mesh Map Test - Default Constructor", "[default][bullet][mesh]" ) {
    psmrts::bullet::DefaultBulletMesh b_map;

    CHECK ( b_map.isValid() == false );
    CHECK ( b_map.name() == "BulletMesh" );
    CHECK ( b_map.id() == 0 );
   // CHECK ( b_map.data()[0][0] == 0 );
   CHECK ( b_map.mesh() == nullptr );
   CHECK ( b_map.toBullet()[0] == 0 );

}

TEST_CASE ( "Bullet Mesh Map Test - Small Dataset", "[bullet][mesh]" ) {

    typedef psmrts::bullet::BulletInternalMeshModel BulletMeshData;
    typedef psmrts::bullet::PsmrtsBulletMeshMap<BulletMeshData> BulletShape;

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    psmrts::PsmrtsOBJAsset t_loader( objfile );

    BulletShape bt_mesh( t_loader );
    CHECK ( bt_mesh.isValid() == true );
    CHECK ( bt_mesh.name()    == t_loader.obj_source() );
    CHECK ( bt_mesh.id()      == 0 );

    CHECK ( bt_mesh.mesh_type() == "Bullet" );

    CHECK ( bt_mesh.mesh() != nullptr );
    CHECK ( bt_mesh.shape() != nullptr );
}
