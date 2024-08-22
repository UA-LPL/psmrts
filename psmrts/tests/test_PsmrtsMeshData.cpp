#include <psmrts_catch2_environment.hpp>

#include <PsmrtsMeshData.hpp>
#include <Eigen/Geometry>

TEST_CASE( "PsmrtsMeshData Default Test", "[mesh][data][default]") {
    psmrts::PsmrtsMeshData my_mesh;

    CHECK( my_mesh.isValid() == false );
    CHECK( my_mesh.nvectors() == 0 );
    CHECK( my_mesh.vector_type() == psmrts::PsmrtsMeshData::PsmrtsDataType::PsmrtsNullType);
    CHECK( my_mesh.isVectorDouble() == false );

    CHECK ( my_mesh.vectors().isValid() == false );
    CHECK ( my_mesh.vectors().size() == 0 );
    CHECK ( my_mesh.vectors().type() == psmrts::PsmrtsMeshData::PsmrtsDataType::PsmrtsNullType);
    CHECK ( my_mesh.vectors().isDouble() == false );
    CHECK ( my_mesh.vectors().isFloat() == false );
   
    // CHECK ( my_mesh.vectors().buffer() );
    // CHECK ( my_mesh.vectors().double_vectors() );
    // CHECK ( my_mesh.vectors().float_vectors() );
}

TEST_CASE( "PsmrtsMeshData Double Test", "[mesh][data][double]") {
    typedef psmrts::PsmrtsVector3<double> ObjVecDouble;
    const size_t n_data = 100;
    auto v_vertex = ObjVecDouble( n_data );

    typedef psmrts::PsmrtsVector3<int> ObjVecIndex;
    const size_t n_data2 = 10;
    auto v_index = ObjVecIndex( n_data2 );

    psmrts::PsmrtsMeshData my_mesh( v_index, v_vertex ); 

    CHECK( my_mesh.isValid() == true );
    CHECK( my_mesh.nvectors() == 100 );
    CHECK( my_mesh.vector_type() == psmrts::PsmrtsMeshData::PsmrtsDataType::PsmrtsDouble );
    CHECK( my_mesh.isVectorDouble() == true );
    CHECK( my_mesh.nfacets() == 10 );
    CHECK( my_mesh.get_index(0) == v_index(0) );
    CHECK( my_mesh.get_vector(0) == v_vertex(0) );

    CHECK( my_mesh.axis_mins()[0] == 0.0 );
    CHECK( my_mesh.axis_mins()[1] == 0.0 );
    CHECK( my_mesh.axis_mins()[2] == 0.0 );

    CHECK( my_mesh.axis_maxs()[0] == 0.0 );
    CHECK( my_mesh.axis_maxs()[1] == 0.0 );
    CHECK( my_mesh.axis_maxs()[2] == 0.0 );

    CHECK( my_mesh.minimum_radius() == 0.0 );
    CHECK( my_mesh.maximum_radius() == 0.0 );

}

TEST_CASE( "PsmrtsMeshData Float Test", "[mesh][data][float]") {
    const double tolerance = 1.0e-12;

    typedef psmrts::PsmrtsVector3<float> ObjVecFloat;
    const size_t n_data = 100;
    auto v_vertex = ObjVecFloat( n_data );

    typedef psmrts::PsmrtsVector3<int> ObjVecIndex;
    const size_t n_data2 = 10;
    auto v_index = ObjVecIndex( n_data2 );

    psmrts::PsmrtsMeshData my_mesh( v_index, v_vertex ); 

    CHECK( my_mesh.isValid() == true );
    CHECK( my_mesh.nvectors() == 100 );
    CHECK( my_mesh.vector_type() == psmrts::PsmrtsMeshData::PsmrtsDataType::PsmrtsFloat );
    CHECK( my_mesh.isVectorDouble() == false );
    CHECK( my_mesh.nfacets() == 10 );
    CHECK( my_mesh.get_index(0) == v_index(0) );
    
    // for floats, must use following workaround
    const psmrts::PsmrtsVector3f &f_vertex = my_mesh.vectors().float_vectors();
    CHECK( f_vertex(0) == v_vertex(0) ); 

    CHECK_THAT( my_mesh.axis_mins()[0], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
    CHECK_THAT( my_mesh.axis_mins()[1], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
    CHECK_THAT( my_mesh.axis_mins()[2], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
    
    CHECK_THAT( my_mesh.axis_mins()[0], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
    CHECK_THAT( my_mesh.axis_mins()[1], Catch::Matchers::WithinAbs( 0.0, tolerance ) );
    CHECK_THAT( my_mesh.axis_mins()[2], Catch::Matchers::WithinAbs( 0.0, tolerance ) );

    CHECK_THAT( my_mesh.minimum_radius(), Catch::Matchers::WithinAbs( 0.0, tolerance ) );
    CHECK_THAT( my_mesh.maximum_radius(), Catch::Matchers::WithinAbs( 0.0, tolerance ) );
}


TEST_CASE( "PsmrtsMeshData Double / Value Test", "[mesh][data][double][values]") {
    typedef psmrts::PsmrtsVector3<double> ObjVecDouble;
    const size_t n_data = 15;
    auto v_vertex = ObjVecDouble( n_data );

    typedef psmrts::PsmrtsVector3<int> ObjVecIndex;
    const size_t n_data2 = 5;
    auto v_index = ObjVecIndex( n_data2 );

    double value = 1.0;
    for (int i = 0; i < v_vertex.size(); i++) {
        ObjVecDouble::vector_reference data_t = v_vertex( i );
        for (int j = 0; j < data_t.size(); j++ ) {
            data_t[j] = value++;
        }
    }

    psmrts::PsmrtsMeshData my_mesh( v_index, v_vertex ); 

    CHECK( my_mesh.isValid() == true );
    CHECK( my_mesh.nvectors() == 15 );
    CHECK( my_mesh.vector_type() == psmrts::PsmrtsMeshData::PsmrtsDataType::PsmrtsDouble );
    CHECK( my_mesh.isVectorDouble() == true );
    CHECK( my_mesh.nfacets() == 5 );
    CHECK( my_mesh.get_index(0) == v_index(0) );
    CHECK( my_mesh.get_vector(0) == v_vertex(0) );

    CHECK( my_mesh.axis_mins()[0] == 1.0 );
    CHECK( my_mesh.axis_mins()[1] == 2.0 );
    CHECK( my_mesh.axis_mins()[2] == 3.0 );

    CHECK( my_mesh.axis_maxs()[0] == 43.0 );
    CHECK( my_mesh.axis_maxs()[1] == 44.0 );
    CHECK( my_mesh.axis_maxs()[2] == 45.0 );

    CHECK( my_mesh.minimum_radius() == 3.74165738677394133 );
    CHECK( my_mesh.maximum_radius() == 76.22335600063802019 );

}