#ifndef PsmrtsBulletMeshMap_hpp
#define PsmrtsBulletMeshMap_hpp

#include <string>
#include <memory>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>

#include "BulletSystemModel.hpp"


namespace psmrts::bullet {

  /**! Special definitions of the Bullet configuration */
  typedef int            BulletNativeIndexType; // Typo Fix - remove prior to push
  typedef btScalar       BulletNativeVertexType;

  typedef PsmrtsVector3<BulletNativeIndexType>       BulletIndexArray;
  typedef PsmrtsVector3<BulletNativeVertexType>      BulletVectorArray;

/**
 * @brief PsmrtsBulletMeshMap provides a Bullet mesh interface
 * 
 * This class is designed to be Bullet-proof! in that the pointer to the 
 * Bullet mesh must be valid for the life of the tracer instance of the
 * Bullet system. This class is designed to be copyable and still
 * retain valid references.
 * 
 * Use this class to create the Bullet mesh partitioning system of large
 * data sets. You can maintain a copyable array of the Bullet mesh
 * partition map.
 * 
 * Note it is up to the caller to ensure the mesh data buffer sizes do
 * not exceed any Bullet limits, e.g., facets/part.
 * 
 * This class is a template that is build upon the PsmrtsMeshData data buffer
 * API that is itself templated. 
 * 
 * See https://github.com/bulletphysics/bullet3/blob/master/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h
 * for a description of the Bullet mesh API used here.
 *
 * @author Kris J. Becker, University of Arizona
 * @history 2024-05-15 Kris J. Becker  Original Version
 */
  class PsmrtsBulletMeshMap : public PsmrtsMeshData {
    public:
      typedef PsmrtsMeshData::PsmrtsDataType    PsmrtsDataType;

      /** Default constructor */
      PsmrtsBulletMeshMap( ) {
        this->init_mesh_map( );
      }

      /** Construct an array of values */
      PsmrtsBulletMeshMap( const PsmrtsMeshData &mesh,
                           const std::string &name, 
                           const int mesh_id,
                           const int partno = 0 ) {
        
        this->init_mesh_map( name, mesh_id, partno );

        if ( true == mesh.isVectorDouble() ) {
          this->init_mesh( mesh.indexes(), mesh.vectors().double_vectors() );
        }
        else {
          this->init_mesh( mesh.indexes(), mesh.vectors().float_vectors() );
        }
        m_bullet_mesh.reset( this->create_map_mesh( ) );

      }

      PsmrtsBulletMeshMap( const PsmrtsOBJFormat &obj_t,
                           const PsmrtsDataType v_data_type = PsmrtsDouble,
                           const int mesh_id = 0, 
                           const int partno = 0 ) {

        this->init_mesh_map( obj_t.obj_source(), mesh_id, partno );

        //Check the requested type
        PsmrtsVector3i mesh_indexes = obj_t.get_indexes();
        if ( PsmrtsDouble == v_data_type ) {
          this->init_mesh( mesh_indexes, obj_t.get_double_vectors() );
        }
        else {
          // Initialize with float vectors
          this->init_mesh( mesh_indexes, obj_t.get_float_vectors() );
        }

        // Retrieve any shape partitions of the facets
        m_shapes = obj_t.get_index_shape_map( mesh_indexes );

        m_bullet_mesh.reset( this->create_map_mesh( ) );
      }

      /** Destructor */
      virtual ~PsmrtsBulletMeshMap() { }

      /** Returns mesh name, ie. bullet */
      inline const std::string &name() const {
        return ( m_name );
      }

      /** Returns mesh's shape identifier */
      inline int id() const {
        return ( m_shape_id );
      }

      /** Returns mesh part number */
      inline int partno() const {
        return ( m_part_no );
      }          

      /** Returns the mesh's type - bullet */
      inline std::string mesh_type() const {
        return ( std::string( "bullet" ) );
      }

      /** Returns mesh's data name, type, shape id, and part number */
      inline std::string mesh_data_id() const {
        std::string dataname = name();
        if ( dataname.length() == 0 ) dataname = "StridingMesh";
        std::string mesh_id_str( dataname + 
                                  "::" + this->mesh_type() + 
                                  "::" + std::to_string( this->id() ) +
                                  "::" + std::to_string( this->partno() ) );
        return ( mesh_id_str );
      }

      /** Returns a pointer to the mesh data */
      inline const btTriangleIndexVertexArray *mesh() const {
        return ( m_bullet_mesh.get() );
      }

      /** Returns vector of partitioned shapes in the mesh */
      inline const std::vector<PsmrtsVector3i> &shapes() const {
        return ( m_shapes );
      }

      /** Creates a triangle mesh mapping with a designated max number of parts */
      inline btTriangleIndexVertexArray *create_map_mesh( const size_t maxparts = 0 ) const {
        
        // Set mesh state
        std::unique_ptr<btTriangleIndexVertexArray> bt_mesh;
        
        // Process if valid
        if ( 0 < this->nfacets() ) {

          // Allocate a new mesh map
          bt_mesh.reset( new btTriangleIndexVertexArray() );

          size_t n_parts = bt_MaxBodyParts();
          if ( maxparts > 0 ) n_parts = std::min( maxparts, bt_MaxBodyParts() );

          auto bt_max_triangles_t = bt_MaxTrianglesPerPart();
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
      inline static btBvhTriangleMeshShape *create_collision_shape( btTriangleIndexVertexArray *mesh,
                                                                    const bool useCompression = true,
                                                                    const bool buildBvh = true ) {

        // Sanity check on pointer
        if ( nullptr == mesh ) {
          std::string mess = "PsmrtsBulletMeshMap::create_collision_body - provided mesh is invalid/null";
          throw std::runtime_error( mess );
        }

        // Creates compressed and optimized Bullet meshes
        return ( new btBvhTriangleMeshShape( mesh, useCompression, buildBvh ) );
      }

      /** Creates a triangle mesh based on internal mesh data */
      inline  btBvhTriangleMeshShape *create_collision_shape( const bool useCompression = true,
                                                              const bool buildBvh = true ) const {

        // Creates compressed and optimized Bullet meshes
        return ( this->create_collision_shape( m_bullet_mesh.get(), useCompression, buildBvh ) );
      }

      using PsmrtsMeshData::elapsed_life_time_s;
      using PsmrtsMeshData::track_count;
      using PsmrtsMeshData::performance_snapshot;      

      private:
        /// Variables for the Bullet system
        std::string                                  m_name;     /**! The name of the Bullet shape model */
        int                                          m_shape_id; /**! Identifier of the shape */
        int                                          m_part_no;  /**! Bullet shape part number */
        std::vector<PsmrtsVector3i>                  m_shapes;   /**! Partitions that map shapes */
        mutable std::shared_ptr<btTriangleIndexVertexArray>  m_bullet_mesh; /**! Ensure persistent references */


        inline void init_mesh_map(const std::string &name = "bullet", 
                                  const int shapeid = 0, 
                                  const int partno = 0 ) {
          m_name      = name;
          m_shape_id  = shapeid;
          m_part_no   = partno;
          m_shapes.clear();
          m_bullet_mesh.reset();
        }
        
  };


} // namespace psmrts:bullet

#endif // PsmrtsBulletMeshMap_hpp
