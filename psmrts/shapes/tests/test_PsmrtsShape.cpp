
#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>

#include <psmrts/shapes/dsk/DskShape.hpp>
#include <psmrts/shapes/obj/ObjShape.hpp>
#include <psmrts/shapes/ply/PlyShape.hpp>

#include <psmrts/shapes/PsmrtsShape.hpp>

TEST_CASE("PsmrtsShape Default Test", "[shape][default]") {

    CHECK( sizeof( psmrts::PsmrtsShape) <= 1390 );

    psmrts::PsmrtsShape test_shape;
    CHECK( test_shape.isValid() == false );

    psmrts::PsmrtsMeshData test_data = test_shape.get_mesh();
    CHECK( test_data.nvectors() == 0 );
    CHECK( test_data.nfacets()  == 0 );

    // This test is not the same on all OSes but tracks the size PSMRTS shapes!
    // CHECK( sizeof( psmrts::PsmrtsShape ) == 1288 );
}

TEST_CASE("NAIF DSK Shape Test", "[naif][shape]") {
    std::string dskfile = psmrts_shapes_path( "dsk/data/bennu_20facets.bds" );
    psmrts::DskShape dsk( dskfile );
    psmrts::PsmrtsShape dsk_shape( dsk );

    CHECK( dsk_shape.isValid() == true );

    psmrts::PsmrtsMeshData dsk_mesh = dsk_shape.get_mesh();

    CHECK( dsk_mesh.isValid() == true );
    CHECK( dsk_mesh.nvectors() == 20 );
    CHECK( dsk_mesh.nfacets() == 36 );
}

TEST_CASE("OBJ Shape Test", "[obj][shape]") {
    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::ObjShape obj( objfile );
    psmrts::PsmrtsShape obj_shape( obj );

    CHECK( obj_shape.isValid() == true );

    psmrts::PsmrtsMeshData obj_mesh = obj_shape.get_mesh();

    CHECK( obj_mesh.isValid() == true );
    CHECK( obj_mesh.nvectors() == 20 );
    CHECK( obj_mesh.nfacets() == 36 );
}

TEST_CASE("PLY Shape Test", "[ply][shape]") {
    std::string plyfile = psmrts_shapes_path( "ply/data/Bennu_Radar.ply"  );
    psmrts::PlyShape ply( plyfile );
    psmrts::PsmrtsShape ply_shape( ply );

    CHECK( ply_shape.isValid() == true );

    psmrts::PsmrtsMeshData ply_mesh = ply_shape.get_mesh();

    CHECK( ply_mesh.isValid() == true );
    CHECK( ply_mesh.nvectors() == 1348  );
    CHECK( ply_mesh.nfacets() == 2692 );
}

TEST_CASE("Direct Constructor Shape Test", "[direct][shape]") {
    class MyShapeClass {
      public:
        MyShapeClass() {}
        MyShapeClass( const psmrts::PsmrtsShape &shape ) {
          m_shape = shape;
        }

      inline const std::string &shape_name(  ) {
        return (m_shape.name() );
      }

    psmrts::PsmrtsShape m_shape;
    };

    std::string plyfile = psmrts_shapes_path( "ply/data/Bennu_Radar.ply"  );
    psmrts::PsmrtsShape direct_shape( plyfile );

    CHECK( direct_shape.isValid() == true );
    MyShapeClass myshape( direct_shape );
    CHECK( direct_shape.name() == myshape.shape_name( ) );

    MyShapeClass myshape2( psmrts::PsmrtsShape{ plyfile } );
    CHECK( direct_shape.name() == myshape2.shape_name( ) );

    psmrts::PsmrtsMeshData ply_mesh = direct_shape.get_mesh();

    CHECK( ply_mesh.isValid() == true );
    CHECK( ply_mesh.nvectors() == 1348  );
    CHECK( ply_mesh.nfacets() == 2692 );
}
