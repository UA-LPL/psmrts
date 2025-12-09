#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsVector3.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>

#include "../private/BulletSystemModel.hpp"
#include "../private/PsmrtsBulletMeshMap.hpp"

#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>

TEST_CASE ( "Bullet Mesh Map Test - Default Constructor", "[default][bullet][mesh]" ) {
    psmrts::bullet::PsmrtsBulletMeshMap b_map;

    CHECK ( b_map.isValid() == false );
    CHECK ( b_map.name() == "bullet" );
    CHECK ( b_map.id() == 0 );
    CHECK ( b_map.mesh() == nullptr );
}

TEST_CASE ( "Bullet Mesh Map Test - Small Dataset", "[bullet][mesh]" ) {

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::PsmrtsShape b_shape{ objfile };    
    psmrts::bullet::PsmrtsBulletMeshMap bt_data( b_shape.get_mesh(), objfile, 0);

    CHECK( bt_data.nfacets()  ==  36 );
    CHECK( bt_data.nvectors() ==  20 );

    psmrts::bullet::PsmrtsBulletMeshMap bt_mesh( bt_data, objfile, 0, 0 );
    CHECK ( bt_mesh.isValid() == true );
    CHECK ( bt_mesh.name()    == bt_data.name() );
    CHECK ( bt_mesh.id()      == 0 );

    CHECK ( bt_mesh.mesh_type() == "bullet" );

    CHECK ( bt_mesh.mesh() != nullptr );

    std::unique_ptr<btBvhTriangleMeshShape> bt_shape( bt_mesh.create_collision_shape() );
    CHECK ( bt_shape.get() != nullptr );

    REQUIRE( bt_mesh.mesh()      != nullptr );

    CHECK ( bt_mesh.mesh()->getNumSubParts()             == 1 );
    CHECK ( bt_mesh.mesh()->getIndexedMeshArray().size() == 1 );

    CHECK( bt_mesh.minimum_radius() == 0.28306500000004281 );
    CHECK( bt_mesh.maximum_radius() == 0.28306500000006685 );

    auto mesh_min = bt_mesh.minimum_radius();
    auto mesh_max = bt_mesh.maximum_radius();
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
    CHECK( bt_minaabb[0] == bt_mesh.axis_mins()[0] );
    CHECK( bt_minaabb[1] == bt_mesh.axis_mins()[1] );
    CHECK( bt_minaabb[2] == bt_mesh.axis_mins()[2] );

    CHECK( bt_maxaabb[0] == bt_mesh.axis_maxs()[0] );
    CHECK( bt_maxaabb[1] == bt_mesh.axis_maxs()[1] );
    CHECK( bt_maxaabb[2] == bt_mesh.axis_maxs()[2] );

}

// Test to compare both the read/process of OBJ read, OBJ exports in
// PsmrtsVector3::get_indexes() and get_vectors(), creation of
// PsmrtsBulletMeshMap with the DSK data.
TEST_CASE ( "Bullet Mesh Map OBJ/DSK Comparison - Bullet == NaifDSK ", "[bullet][obj][dsk]" ) {
    auto tolerance = 1.0e-6;
    // @TODO Setup for DSK ( this should be a fixture!)
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system

    CHECK ( naif::KernelFileSystem::kernel_count() == 0 ); // Should be zero, as we have yet to load any
    CHECK ( naif::KernelFileSystem::size() == 0 );         // No cached files either...

    // Files to compare    
    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    std::string dskfile = psmrts_tracers_path( "naifdsk/data/bennu_20facets.bds" );

    // Load the OBJ
    psmrts::PsmrtsShape b_shape{ objfile };    
    psmrts::bullet::PsmrtsBulletMeshMap bt_data( b_shape.get_mesh(), b_shape.name(), 0);    

    // Load the DSK compare size
    naif::DskKernelModel dsk( dskfile );
    CHECK_NOTHROW( naif::check_naif_errors() );
    naif::DskSegment segment = dsk.segment();
    CHECK_NOTHROW( naif::check_naif_errors() );
    CHECK( segment.n_plates() == bt_data.nfacets() );
    CHECK( segment.n_vertices() == bt_data.nvectors() );

    auto obj_indexes = bt_data.indexes();
    auto obj_vectors = bt_data.vectors().double_vectors();
    
    auto dsk_indexes = dsk.load_facet_indexes(); 
    auto dsk_vectors = dsk.load_facet_vectors();

    CHECK ( obj_indexes.size()            == dsk_indexes.size() ); 
    CHECK ( obj_indexes.stride_size()     == dsk_indexes.stride_size() );
    CHECK ( obj_indexes.vector_size()     == dsk_indexes.vector_size() );
    CHECK ( obj_indexes.volume_size()     == dsk_indexes.volume_size() );

    CHECK ( obj_vectors.size()            == dsk_vectors.size() );
    CHECK ( obj_vectors.stride_size()     == dsk_vectors.stride_size() );
    CHECK ( obj_vectors.vector_size()     == dsk_vectors.vector_size() );
    CHECK ( obj_vectors.volume_size()     == dsk_vectors.volume_size() );

    // check each value of indexes and vectors individually, not comparing
    psmrts::PsmrtsMeshData  obj_data{  obj_indexes, obj_vectors };
    psmrts::PsmrtsMeshData  dsk_data{  dsk_indexes, dsk_vectors };

    psmrts::bullet::PsmrtsBulletMeshMap obj_mesh( obj_data, objfile, 0, 0  );
    psmrts::bullet::PsmrtsBulletMeshMap dsk_mesh( dsk_data, dskfile, 0, 0  );

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
    CHECK_THAT( obj_mesh.minimum_radius(), Catch::Matchers::WithinAbs( dsk_mesh.minimum_radius(), 1.0E-9 ) );
    CHECK ( obj_mesh.nvectors()       == dsk_mesh.nvectors() );
    CHECK ( obj_mesh.nfacets()        == dsk_mesh.nfacets() );

    // Compare indexes
    CHECK ( obj_mesh.get_index(0)  == dsk_mesh.get_index(0) );
    CHECK ( obj_mesh.get_index(17) == dsk_mesh.get_index(17) );
    CHECK ( obj_mesh.get_index(35) == dsk_mesh.get_index(35) );
    
    // Compare vectors
    CHECK ( obj_mesh.get_vector(0)[0] == dsk_mesh.get_vector(0)[0] );
    CHECK ( obj_mesh.get_vector(0)[1] == dsk_mesh.get_vector(0)[1] );
    CHECK ( obj_mesh.get_vector(0)[2] == dsk_mesh.get_vector(0)[2] );

    CHECK ( obj_mesh.get_vector(10)[0] == dsk_mesh.get_vector(10)[0] );
    CHECK ( obj_mesh.get_vector(10)[1] == dsk_mesh.get_vector(10)[1] );
    CHECK ( obj_mesh.get_vector(10)[2] == dsk_mesh.get_vector(10)[2] );

    CHECK ( obj_mesh.get_vector(19)[0] == dsk_mesh.get_vector(19)[0] );
    CHECK ( obj_mesh.get_vector(19)[1] == dsk_mesh.get_vector(19)[1] );
    CHECK ( obj_mesh.get_vector(19)[2] == dsk_mesh.get_vector(19)[2] );

    // ...part of the fixture destructor!
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK ( naif::KernelFileSystem::size() == 0 );    

}