
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

    typedef psmrts::bullet::BulletInternalMeshModel              BulletMeshModel;
    typedef psmrts::bullet::PsmrtsBulletMeshMap<BulletMeshModel> BulletShape;

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    psmrts::PsmrtsOBJAsset t_loader( objfile );
    CHECK( t_loader.nIndexes()  == 36 );
    CHECK( t_loader.nVertexes() == 20 );

    // BulletShape bt_mesh( t_loader );
    auto indexes =  t_loader.get_indexes<int>();
    auto vectors = t_loader.get_vectors<double>();

    CHECK( indexes.size()   == 36 );
    CHECK( vectors.size()  == 20 );
    BulletMeshModel bt_data( indexes, vectors );
    CHECK( bt_data.nfacets()  ==  36 );
    CHECK( bt_data.nvectors() ==  20 );

    BulletShape bt_mesh( bt_data, objfile, 0, 0  );
    CHECK ( bt_mesh.isValid() == true );
    CHECK ( bt_mesh.name()    == t_loader.obj_source() );
    CHECK ( bt_mesh.id()      == 0 );

    CHECK ( bt_mesh.mesh_type() == "Bullet" );

    CHECK ( bt_mesh.mesh() != nullptr );
    CHECK ( bt_mesh.shape() != nullptr );
}
