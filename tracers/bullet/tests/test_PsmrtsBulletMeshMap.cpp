
#include <psmrts_catch2_environment.hpp>

#include <BulletSystemModel.hpp>
#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>
#include <PsmrtsOBJAsset.hpp>
#include <PsmrtsBulletMeshMap.hpp>

#include <DskKernelModel.hpp>


TEST_CASE ( "Bullet Mesh Map Test - Default Constructor", "[default][bullet][mesh]" ) {
    psmrts::bullet::NativeBulletMeshMap b_map;

    CHECK ( b_map.isValid() == false );
    CHECK ( b_map.name() == "BulletMesh" );
    CHECK ( b_map.id() == 0 );
    CHECK ( b_map.mesh() == nullptr );
}

TEST_CASE ( "Bullet Mesh Map Test - Small Dataset", "[bullet][mesh]" ) {

    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    psmrts::PsmrtsOBJAsset t_loader( objfile );
    CHECK( t_loader.nIndexes()  == 36 );
    CHECK( t_loader.nVertexes() == 20 );

    // BulletShape bt_mesh( t_loader );
    auto indexes =  t_loader.get_indexes<int>();
    auto vectors = t_loader.get_vectors<double>();

    CHECK( indexes.size()   == 36 );
    CHECK( vectors.size()  == 20 );
    psmrts::bullet::BulletNativeMeshData bt_data( indexes, vectors );
    CHECK( bt_data.nfacets()  ==  36 );
    CHECK( bt_data.nvectors() ==  20 );

    psmrts::bullet::NativeBulletMeshMap bt_mesh( bt_data, objfile, 0, 0  );
    CHECK ( bt_mesh.isValid() == true );
    CHECK ( bt_mesh.name()    == t_loader.obj_source() );
    CHECK ( bt_mesh.id()      == 0 );

    CHECK ( bt_mesh.mesh_type() == "Bullet" );

    CHECK ( bt_mesh.mesh() != nullptr );

    std::unique_ptr<btBvhTriangleMeshShape> bt_shape( bt_mesh.create_collision_shape() );
    CHECK ( bt_shape.get() != nullptr );

    REQUIRE( bt_mesh.mesh()      != nullptr );

    CHECK ( bt_mesh.mesh()->getNumSubParts()             == 1 );
    CHECK ( bt_mesh.mesh()->getIndexedMeshArray().size() == 1 );

    CHECK( bt_mesh.data().minimum_radius() == 0.28306500000004281 );
    CHECK( bt_mesh.data().maximum_radius() == 0.28306500000006685 );

    auto mesh_min = bt_mesh.data().minimum_radius();
    auto mesh_max = bt_mesh.data().maximum_radius();
    CHECK( psmrts::bullet::bt_type_code( &mesh_min ) == PHY_DOUBLE );
    CHECK( psmrts::bullet::bt_type_code( &mesh_max ) == PHY_DOUBLE );

    CHECK( sizeof( mesh_min ) == sizeof( double ) );
    CHECK( sizeof( mesh_max ) != sizeof( float ) );

    REQUIRE ( bt_shape.get() != nullptr );

    btTransform tr;
    tr.setIdentity();
    btVector3 bt_minaabb,bt_maxaabb;
    bt_shape->getAabb(tr,bt_minaabb,bt_maxaabb);

    // There is two tests that fail here. Please check
    // PsmrtsMeshData::init( indexes, vectors ) where the PSMRTS
    // range is computed. - (PsmrtsMeshData.hpp, line 275?)
    CHECK( bt_minaabb[0] == bt_mesh.data().axis_mins()[0] );
    CHECK( bt_minaabb[1] == bt_mesh.data().axis_mins()[1] );
    CHECK( bt_minaabb[2] == bt_mesh.data().axis_mins()[2] );

    CHECK( bt_maxaabb[0] == bt_mesh.data().axis_maxs()[0] );
    CHECK( bt_maxaabb[1] == bt_mesh.data().axis_maxs()[1] );
    CHECK( bt_maxaabb[2] == bt_mesh.data().axis_maxs()[2] );

}

// Test to compare both the read/process of OBJ read, OBJ exports in
// PsmrtsDataModel::get_indexes() and get_vectors(), creation of
// PsmrtsBulletMeshMap with the DSK data.
TEST_CASE ( "Bullet Mesh Map OBJ/DSK Comparison - Bullet == NaifDSK ", "[bullet][obj][dsk]" ) {
    auto tolerance = 1.0e-6;
    // @TODO Setup for DSK ( this should be a fixture!)
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK ( naif::KernelFileSystem::size() == 0 );         // No cached files either...

    // Files to compare    
    std::string objfile = psmrts_formats_path( "obj/data/bennu_20facets.obj" );
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );

    // Load the OBJ
    psmrts::PsmrtsOBJAsset t_loader( objfile );
    CHECK( t_loader.nIndexes()  == 36 );
    CHECK( t_loader.nVertexes() == 20 );

    // Load the DSK compare size
    naif::DskKernelModel dsk( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() );
    naif::DskSegment segment = dsk.segment();
    CHECK_NOTHROW( naif::check_naif_errors() );
    CHECK( segment.n_plates() == t_loader.nIndexes() );
    CHECK( segment.n_vertices() == t_loader.nVertexes() );

    auto obj_indexes = t_loader.get_indexes<int>();
    auto obj_vectors = t_loader.get_vectors<double>();
    
    auto dsk_indexes = dsk.load_facet_indexes(); 
    auto dsk_vectors = dsk.load_facet_vectors();

    CHECK ( obj_indexes.size()            == dsk_indexes.size() ); 
    CHECK ( obj_indexes.data_size()       == dsk_indexes.data_size() );
    CHECK ( obj_indexes.scalar_size()     == dsk_indexes.scalar_size() );
    CHECK ( obj_indexes.total_allocated() == dsk_indexes.total_allocated() );

    CHECK ( obj_vectors.size()            == dsk_vectors.size() );
    CHECK ( obj_vectors.data_size()       == dsk_vectors.data_size() );
    CHECK ( obj_vectors.scalar_size()     == dsk_vectors.scalar_size() );
    CHECK ( obj_vectors.total_allocated() == dsk_vectors.total_allocated() );

    // check each value of indexes and vectors individually, not comparing
    psmrts::bullet::BulletNativeMeshData  obj_data( obj_indexes, obj_vectors );
    psmrts::bullet::BulletNativeMeshData  dsk_data( dsk_indexes, dsk_vectors );

    psmrts::bullet::NativeBulletMeshMap obj_mesh( obj_data, objfile, 0, 0  );
    psmrts::bullet::NativeBulletMeshMap dsk_mesh( dsk_data, dskfile, 0, 0  );

    REQUIRE ( obj_mesh.isValid() == true );
    REQUIRE ( dsk_mesh.isValid() == true );

    // Compare sizes
    auto obj_array = obj_mesh.mesh()->getIndexedMeshArray();
    auto dsk_array = dsk_mesh.mesh()->getIndexedMeshArray();
    CHECK ( obj_array.size() == dsk_array.size() ); 
    
    // Compare each cooresponding method   
    CHECK ( obj_mesh.name()                  != dsk_mesh.name() );
    CHECK ( obj_mesh.id()                    == 0 );
    CHECK ( dsk_mesh.id()                    == 0 );
    CHECK ( obj_mesh.mesh_type()             == dsk_mesh.mesh_type() );
    CHECK ( obj_mesh.data().minimum_radius() == dsk_mesh.data().minimum_radius() ); 
    CHECK ( obj_mesh.data().nvectors()       == dsk_mesh.data().nvectors() );
    CHECK ( obj_mesh.data().nfacets()        == dsk_mesh.data().nfacets() );
    CHECK ( obj_mesh.data().base_index()     == dsk_mesh.data().base_index() );

    // Compare indexes
    CHECK ( obj_mesh.data().get_index(0)  == dsk_mesh.data().get_index(0) );
    CHECK ( obj_mesh.data().get_index(17) == dsk_mesh.data().get_index(17) );
    CHECK ( obj_mesh.data().get_index(35) == dsk_mesh.data().get_index(35) );
    
    // Compare vectors
    CHECK ( obj_mesh.data().get_vector(0)[0] == dsk_mesh.data().get_vector(0)[0] );
    CHECK ( obj_mesh.data().get_vector(0)[1] == dsk_mesh.data().get_vector(0)[1] );
    CHECK ( obj_mesh.data().get_vector(0)[2] == dsk_mesh.data().get_vector(0)[2] );

    CHECK ( obj_mesh.data().get_vector(10)[0] == dsk_mesh.data().get_vector(10)[0] );
    CHECK ( obj_mesh.data().get_vector(10)[1] == dsk_mesh.data().get_vector(10)[1] );
    CHECK ( obj_mesh.data().get_vector(10)[2] == dsk_mesh.data().get_vector(10)[2] );

    CHECK ( obj_mesh.data().get_vector(19)[0] == dsk_mesh.data().get_vector(19)[0] );
    CHECK ( obj_mesh.data().get_vector(19)[1] == dsk_mesh.data().get_vector(19)[1] );
    CHECK ( obj_mesh.data().get_vector(19)[2] == dsk_mesh.data().get_vector(19)[2] );

    // ...part of the fixture destructor!
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK ( naif::KernelFileSystem::size() == 0 );    

}