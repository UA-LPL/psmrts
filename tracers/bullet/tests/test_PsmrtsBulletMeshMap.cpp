
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>
#include <PsmrtsBulletMeshMap.hpp>
#include <PsmrtsOBJAsset.hpp>


TEST_CASE ( "Bullet Mesh Map Test - Default Constructor", "[default][bullet][mesh]" ) {
    psmrts::bullet::NativeBulletMesh b_map;

    CHECK ( b_map.isValid() == false );
    CHECK ( b_map.name() == "BulletMesh" );
    CHECK ( b_map.id() == 0 );
   // CHECK ( b_map.data()[0][0] == 0 );
   CHECK ( b_map.mesh() == nullptr );
}

TEST_CASE ( "Bullet Mesh Map Test - Small Dataset", "[bullet][mesh]" ) {

    typedef psmrts::bullet::BulletInternalMeshModel             BulletMeshData;
    typedef psmrts::bullet::PsmrtsBulletMeshMap<BulletMeshData> BulletShape;

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    psmrts::PsmrtsOBJAsset t_loader( objfile );
    CHECK( t_loader.nIndexes()  == 36 );
    CHECK( t_loader.nVertexes() == 20 );

    // BulletShape bt_mesh( t_loader );
    BulletShape bt_mesh( BulletMeshData( t_loader.get_indexes<int>(), t_loader.get_vectors<double>() ), 
                         "bennu_20facets.obj", 0, 0  );
    CHECK ( bt_mesh.isValid() == true );
    CHECK ( bt_mesh.name()    == t_loader.obj_source() );
    CHECK ( bt_mesh.id()      == 0 );

    CHECK ( bt_mesh.mesh_type() == "Bullet" );

    CHECK ( bt_mesh.mesh() != nullptr );
    CHECK ( bt_mesh.shape() != nullptr );
}
