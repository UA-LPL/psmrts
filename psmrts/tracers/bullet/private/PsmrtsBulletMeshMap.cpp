#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <memory>

#include <psmrts/core/PsmrtsMeshData.hpp>
#include "BulletSystemModel.hpp"
#include "PsmrtsBulletMeshMap.hpp"


namespace psmrts::bullet {

  /** Default constructor */
  PsmrtsBulletMeshMap::PsmrtsBulletMeshMap( ) {
    this->init_mesh_map( );
  }

  /** Construct an array of values */
  PsmrtsBulletMeshMap::PsmrtsBulletMeshMap( const PsmrtsMeshData &mesh,
                                            const std::string &name, 
                                            const int mesh_id,
                                            const int partno ) {
    
    if ( !mesh.isValid() || ( mesh.nvectors() == 0 ) ) {
      std::string mess = "PsmrtsBulletMeshMap mesh " + name + 
                         " contains no data or is invalid";
      throw std::runtime_error( mess );          
    }

    this->init_mesh_map( name, mesh_id, partno );

    if ( true == mesh.isVectorDouble() ) {
      this->init_mesh( mesh.indexes(), mesh.vectors().double_vectors() );
    }
    else {
      this->init_mesh( mesh.indexes(), mesh.vectors().float_vectors() );
    }
    m_bullet_mesh.reset( this->create_map_mesh( ) );

  }
  
  /** Returns mesh name, ie. bullet */
  const std::string &PsmrtsBulletMeshMap::name() const {
    return ( m_name );
  }

  /** Returns mesh's shape identifier */
  int PsmrtsBulletMeshMap::id() const {
    return ( m_shape_id );
  }

  /** Returns mesh part number */
  int PsmrtsBulletMeshMap::partno() const {
    return ( m_part_no );
  }          

  /** Returns the mesh's type - bullet */
  std::string PsmrtsBulletMeshMap::mesh_type() const {
    return ( std::string( "bullet" ) );
  }

  /** Returns mesh's data name, type, shape id, and part number */
  std::string PsmrtsBulletMeshMap::mesh_data_id() const {
    std::string dataname = name();
    if ( dataname.length() == 0 ) dataname = "StridingMesh";
    std::string mesh_id_str( dataname + 
                              "::" + this->mesh_type() + 
                              "::" + std::to_string( this->id() ) +
                              "::" + std::to_string( this->partno() ) );
    return ( mesh_id_str );
  }

  /** Returns a pointer to the mesh data */
  const btTriangleIndexVertexArray *PsmrtsBulletMeshMap::mesh() const {
    return ( m_bullet_mesh.get() );
  }

  /** Returns vector of partitioned shapes in the mesh */
  const std::vector<PsmrtsVector3i> &PsmrtsBulletMeshMap::shapes() const {
    return ( m_shapes );
  }

  /** Creates a triangle mesh mapping with a designated max number of parts */
  btTriangleIndexVertexArray *PsmrtsBulletMeshMap::create_map_mesh( const size_t maxparts ) const {
    ZoneScopedN( "psmrts::PsmrtsBulletMeshMap::create_map_mesh" );
    
    // Set mesh state
    std::unique_ptr<btTriangleIndexVertexArray> bt_mesh;
    
    // Process if valid
    if ( 0 < this->nfacets() ) {

      // Allocate a new mesh map
      bt_mesh.reset( new btTriangleIndexVertexArray() );

      size_t nfacets_t = this->nfacets();

      const PsmrtsStridingBuffer &vector_m = this->vectors().buffer();
      const PsmrtsStridingBuffer &index_m  = this->indexes().buffer();

      PHY_ScalarType bt_vector_type = PHY_DOUBLE;
      PHY_ScalarType bt_index_type  = PHY_INTEGER;
      if ( false == this->isVectorDouble() ) bt_vector_type = PHY_FLOAT;

      size_t n_mapped_t = 0;
      for ( size_t index_t = 0 ; index_t < nfacets_t ; index_t += n_mapped_t ) {

        size_t n_indexes = nfacets_t - index_t;
        if ( n_indexes > bt_MaxTrianglesPerPart() ) n_indexes = bt_MaxTrianglesPerPart();

        btIndexedMesh mesh_t;

        // Set up access to range of triangle mesh indexes
        mesh_t.m_numTriangles        = n_indexes;
        mesh_t.m_triangleIndexStride = index_m.stride_size();
        mesh_t.m_triangleIndexBase   = index_m.get( index_t );
        mesh_t.m_indexType           = bt_index_type;

        // The full number of vertexs are used for every part
        mesh_t.m_numVertices         = vector_m.size();
        mesh_t.m_vertexBase          = vector_m.get( 0 );
        mesh_t.m_vertexStride        = vector_m.stride_size();
        mesh_t.m_vertexType          = bt_vector_type;

        // Add this offset
        bt_mesh->addIndexedMesh( mesh_t );
        n_mapped_t = n_indexes;
      }
    }

    return ( bt_mesh.release() );
  }

  /** [Static?] Create a specialized BVH static triangle mesh for ray tracing */
  btBvhTriangleMeshShape *PsmrtsBulletMeshMap::create_collision_shape( btTriangleIndexVertexArray *mesh,
                                                                const bool useCompression,
                                                                const bool buildBvh) {
    ZoneScopedN( "psmrts::PsmrtsBulletMeshMap::create_collision_shape(mesh)" );

    // Sanity check on pointer
    if ( nullptr == mesh ) {
      std::string mess = "PsmrtsBulletMeshMap::create_collision_body - provided mesh is invalid/null";
      throw std::runtime_error( mess );
    }

    // Creates compressed and optimized Bullet meshes
    return ( new btBvhTriangleMeshShape( mesh, useCompression, buildBvh ) );
  }

  /** Creates a triangle mesh based on internal mesh data */
    btBvhTriangleMeshShape *PsmrtsBulletMeshMap::create_collision_shape( const bool useCompression,
                                                                         const bool buildBvh ) const {
    ZoneScopedN( "psmrts::PsmrtsBulletMeshMap::create_collision_shape" );

    // Creates compressed and optimized Bullet meshes
    return ( this->create_collision_shape( m_bullet_mesh.get(), useCompression, buildBvh ) );
  }

  void PsmrtsBulletMeshMap::init_mesh_map(const std::string &name, 
                            const int shapeid, const int partno ) {
    m_name      = name;
    m_shape_id  = shapeid;
    m_part_no   = partno;
    m_shapes.clear();
    m_bullet_mesh.reset();
  }

} // namespace psmrts:bullet
