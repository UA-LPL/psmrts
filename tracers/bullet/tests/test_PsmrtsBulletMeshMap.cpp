
#include <psmrts_catch2_environment.hpp>

#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>
#include <PsmrtsBulletMeshMap.hpp>
#include <PsmrtsOBJAsset.hpp>

#include <DskKernelModel.hpp>


typedef psmrts::bullet::BulletInternalMeshModel              BulletMeshModel;
typedef psmrts::bullet::PsmrtsBulletMeshMap<BulletMeshModel> BulletShape;

TEST_CASE ( "Bullet Mesh Map Test - Default Constructor", "[default][bullet][mesh]" ) {
    psmrts::bullet::NativeBulletMesh b_map;

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

    REQUIRE( bt_mesh.mesh()      != nullptr );

    CHECK ( bt_mesh.mesh()->getNumSubParts()             == 1 );
    CHECK ( bt_mesh.mesh()->getIndexedMeshArray().size() == 1 );

    CHECK( bt_mesh.data().minimum_radius() == 0.28306500000004281 );
    CHECK( bt_mesh.data().maximum_radius() == 0.28306500000006685 );

    REQUIRE ( bt_mesh.shape() != nullptr );

    btTransform tr;
    tr.setIdentity();
    btVector3 bt_minaabb,bt_maxaabb;
    bt_mesh.shape()->getAabb(tr,bt_minaabb,bt_maxaabb);

    // There is two tests that fail here. Please check
    // PsmrtsMeshData::init( indexes, vectors ) where the PSMRTS
    // range is computed.
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

    // @TODO Setup for DSK ( this should be a fixture!)
    typedef psmrts::PsmrtsShapeTracerAdapter<naif::DskKernelModel>   NaifShapeTracer;

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

    // Complete tests below...
    const bool NotImplemented = true;
    REQURE( false == NotImplemented );  // Remove when implemented

    // Compare sizes

    // Compare each cooresponding method 

    // Compare indexes

    // Compare vectors

    // ...part of the fixture destructor!
    CHECK_NOTHROW ( naif::DskKernelModel::reset_dsk_system() ); // Reset/Initialize the kernel system
    CHECK_NOTHROW ( naif::KernelFileSystem::reset_kernel_system() ); // Reset/Initialize the kernel system
    CHECK ( naif::KernelFileSystem::size() == 0 );    

}