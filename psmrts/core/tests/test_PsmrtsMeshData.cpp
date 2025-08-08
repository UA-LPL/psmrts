#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsMeshData.hpp>
#include <Eigen/Geometry>
#include <typeinfo>

TEST_CASE( "PsmrtsMeshData Default Test", "[mesh][data][default]") {
    psmrts::PsmrtsMeshData my_mesh;

    CHECK( my_mesh.isValid()        == false );
    CHECK( my_mesh.nvectors()       == 0 );
    CHECK( my_mesh.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDataType::PsmrtsNullType);
    CHECK( my_mesh.isVectorDouble() == false );

    CHECK( my_mesh.vectors().isValid()  == false );
    CHECK( my_mesh.vectors().size()     == 0 );
    CHECK( my_mesh.vectors().type()     == psmrts::PsmrtsMeshData::PsmrtsDataType::PsmrtsNullType);
    CHECK( my_mesh.vectors().isDouble() == false );
    CHECK( my_mesh.vectors().isFloat()  == false );

    CHECK( my_mesh.vectors().buffer().size()                == 0 );
    CHECK( my_mesh.vectors().double_vectors().vector_size() == 3 );
    CHECK( my_mesh.vectors().float_vectors().vector_size()  == 3 ); 
}

TEST_CASE( "PsmrtsMeshData Double Test", "[mesh][data][double]") {
    typedef psmrts::PsmrtsVector3<double> ObjVecDouble;
    const size_t n_data = 100;
    auto v_vertex = ObjVecDouble( n_data );
    double v_value = 1.0;
    for (int i = 0; i < v_vertex.size(); i++) {
        ObjVecDouble::vector_reference data_v = v_vertex( i );
        for (int j = 0; j < data_v.size(); j++ ) {
            data_v[j] = v_value++;
        }
    }

    typedef psmrts::PsmrtsVector3<int> ObjVecIndex;
    const size_t n_data2 = 10;
    auto v_index = ObjVecIndex( n_data2 );
    int i_value = 1;
    for (int n = 0; n < v_index.size(); n++) {
        ObjVecIndex::vector_reference data_i = v_index( n );
        for (int m = 0; m < data_i.size(); m++) {
            data_i[m] = i_value++;
        }
    }

    psmrts::PsmrtsMeshData my_mesh( v_index, v_vertex ); 

    CHECK( my_mesh.isValid()        == true );
    CHECK( my_mesh.nvectors()       == 100 );
    CHECK( my_mesh.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDataType::PsmrtsDouble );
    CHECK( my_mesh.isVectorDouble() == true );
    CHECK( my_mesh.nfacets()        == 10 );
    CHECK( my_mesh.get_index(0)     == v_index(0) );
    CHECK( my_mesh.get_vector(0)    == v_vertex(0) );

    int test_index = 1;
    for (int k = 0; k < my_mesh.get_index().size(); k++ ) {
        for (int l = 0; l < my_mesh.get_index(k).size(); l++) {
            CHECK( my_mesh.get_index(k)[l] == test_index++ );
        }
    }

    double test_vertex = 1.0;
    for (int p = 0; p < my_mesh.get_vector().size(); p++ ) {
        for (int q = 0; q < my_mesh.get_vector(p).size(); q++ ) {
            CHECK( my_mesh.get_vector(p)[q] == test_vertex++ );
        }
    }

    CHECK( my_mesh.axis_mins()[0] == 1.0 );
    CHECK( my_mesh.axis_mins()[1] == 2.0 );
    CHECK( my_mesh.axis_mins()[2] == 3.0 );

    CHECK( my_mesh.axis_maxs()[0] == 298.0 );
    CHECK( my_mesh.axis_maxs()[1] == 299.0 );
    CHECK( my_mesh.axis_maxs()[2] == 300.0 );

    CHECK( my_mesh.minimum_radius() == 3.74165738677394133 );
    CHECK( my_mesh.maximum_radius() == 517.88512239684973792 );

}

TEST_CASE( "PsmrtsMeshData Float Test", "[mesh][data][float]") {
    const double tolerance = 1.0e-12;

    typedef psmrts::PsmrtsVector3<float> ObjVecFloat;
    const size_t n_data = 100;
    auto v_vertex = ObjVecFloat( n_data );
    float v_value = 1.0;
    for (int i = 0; i < v_vertex.size(); i++) {
        ObjVecFloat::vector_reference data_v = v_vertex( i );
        for (int j = 0; j < data_v.size(); j++ ) {
            data_v[j] = v_value++;
        }
    }

    typedef psmrts::PsmrtsVector3<int> ObjVecIndex;
    const size_t n_data2 = 10;
    auto v_index = ObjVecIndex( n_data2 );
    int i_value = 1;
    for (int n = 0; n < v_index.size(); n++) {
        ObjVecIndex::vector_reference data_i = v_index( n );
        for (int m = 0; m < data_i.size(); m++) {
            data_i[m] = i_value++;
        }
    }

    psmrts::PsmrtsMeshData my_mesh( v_index, v_vertex ); 

    CHECK( my_mesh.isValid()        == true );
    CHECK( my_mesh.nvectors()       == 100 );
    CHECK( my_mesh.vector_type()    == psmrts::PsmrtsMeshData::PsmrtsDataType::PsmrtsFloat );
    CHECK( my_mesh.isVectorDouble() == false );
    CHECK( my_mesh.nfacets()        == 10 );
    CHECK( my_mesh.get_index(0)     == v_index(0) );
    
    // for floats, must use following workaround
    const psmrts::PsmrtsVector3f &f_vertex = my_mesh.vectors().float_vectors();
    CHECK( f_vertex(0) == v_vertex(0) ); 

    int test_index = 1;
    for (int k = 0; k < my_mesh.get_index().size(); k++ ) {
        for (int l = 0; l < my_mesh.get_index(k).size(); l++) {
            CHECK( my_mesh.get_index(k)[l] == test_index++ );
        }
    }

    float test_vertex = 1.0;
     for (int p = 0; p < my_mesh.get_vector().size(); p++ ) {
        for (int q = 0; q < my_mesh.get_vector(p).size(); q++ ) {
            CHECK( my_mesh.get_vector(p)[q] == test_vertex++ );
        }
    }

    CHECK_THAT( my_mesh.axis_mins()[0], Catch::Matchers::WithinAbs( 1.0, tolerance ) );
    CHECK_THAT( my_mesh.axis_mins()[1], Catch::Matchers::WithinAbs( 2.0, tolerance ) );
    CHECK_THAT( my_mesh.axis_mins()[2], Catch::Matchers::WithinAbs( 3.0, tolerance ) );
    
    CHECK_THAT( my_mesh.axis_maxs()[0], Catch::Matchers::WithinAbs( 298.0, tolerance ) );
    CHECK_THAT( my_mesh.axis_maxs()[1], Catch::Matchers::WithinAbs( 299.0, tolerance ) );
    CHECK_THAT( my_mesh.axis_maxs()[2], Catch::Matchers::WithinAbs( 300.0, tolerance ) );

    CHECK_THAT( my_mesh.minimum_radius(), Catch::Matchers::WithinAbs( 3.74165738677394133, tolerance ) );
    CHECK_THAT( my_mesh.maximum_radius(), Catch::Matchers::WithinAbs( 517.88512239684973792, tolerance ) );
}
