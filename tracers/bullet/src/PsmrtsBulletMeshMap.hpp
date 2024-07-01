#ifndef PsmrtsBulletMeshMap_hpp
#define PsmrtsBulletMeshMap_hpp

#include <string>
#include <memory>
#include <exception>

#include <Eigen/Geometry>

#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>

#include <BulletSystemModel.hpp>
#include <PsmrtsOBJFormat.hpp>


namespace psmrts::bullet {

  /**! Special definitions of the Bullet configuration */
  typedef int            BulletNativeIndexType; // Typo Fix - remove prior to push
  typedef btScalar       BulletNativeVertexType;
  typedef PsmrtsMeshData<BulletNativeIndexType, BulletNativeVertexType>    BulletNativeMeshData;

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
  template <typename MODEL> 
    class PsmrtsBulletMeshMap {
      public:
        typedef typename MODEL::index_data_type       index_data_type;
        typedef typename MODEL::vector_data_type      vector_data_type;

        // The desired data buffer type to map to a Bullet indexed mesh
        typedef typename MODEL::index_type       MeshFacetIndex;
        typedef typename MODEL::vector_type      MeshFacetVector;

        typedef typename MODEL::const_index_reference  const_index_reference;
        typedef typename MODEL::const_vector_reference const_vector_reference;

        /** Default constructor */
        PsmrtsBulletMeshMap( ) {
          init( );
        }

        /** Construct an array of values */
        PsmrtsBulletMeshMap( const MODEL &mesh, const std::string &name, 
                              const int mesh_id, const int partno = 0 ) {
          init( name, mesh_id, partno );
          m_mesh_data = mesh;
          m_bullet_mesh.reset( create_map_mesh( m_mesh_data ) );
          // m_bullet_shape.reset( create_collision_shape( m_bullet_mesh.get() ) );
        }

        PsmrtsBulletMeshMap( const PsmrtsOBJFormat &obj_t ) {
          init( obj_t.obj_source() );
          m_mesh_data = MODEL( obj_t.get_indexes<index_data_type>( ), obj_t.get_vectors<vector_data_type>() );
          m_bullet_mesh.reset( create_map_mesh( m_mesh_data ) );
          // m_bullet_shape.reset( create_collision_shape( m_bullet_mesh.get() ) );
        }

        /** Destructor */
        virtual ~PsmrtsBulletMeshMap() { }


        inline bool isValid() const {
          return ( nullptr != this->mesh() );
        }

        inline const std::string &name() const {
          return ( m_name );
        }

        inline int id() const {
          return ( m_shape_id );
        }

        inline int partno() const {
          return ( m_part_no );
        }          

        inline std::string mesh_type() const {
          return ( std::string( "Bullet" ) );
        }

        inline std::string mesh_data_id() const {
          std::string dataname = name();
          if ( dataname.length() == 0 ) dataname = "StridingMesh";
          std::string mesh_id_str( dataname + 
                                    "::" + this->mesh_type() + 
                                    "::" + std::to_string( this->id() ) +
                                    "::" + std::to_string( this->partno() ) );
          return ( mesh_id_str );
        }

        inline const MODEL &data() const {
          return ( m_mesh_data );
        }

        inline const btTriangleIndexVertexArray *mesh() const {
          return ( m_bullet_mesh.get() );
        }

#if 0
        inline const btCollisionShape *shape() const {
          return ( m_bullet_shape.get() );
        }
#endif
        /** Create a Bullet btVector3 type array from data set scalar type */
        template <typename SCALAR_T> 
          static inline btVector3 toBullet( const SCALAR_T &v ) {
            return ( b3Vector( v[0], v[1], v[2]) );
          }


        inline btTriangleIndexVertexArray *create_map_mesh( const MODEL &mesh, const size_t maxparts = 0 ) const {
          
          // Set mesh state
          std::unique_ptr<btTriangleIndexVertexArray> bt_mesh;
          
          // Process if valid
          if ( 0 < mesh.nfacets() ) {

            // Allocate a new mesh map
            bt_mesh.reset( new btTriangleIndexVertexArray() );

            size_t n_parts = bt_MaxBodyParts();
            if ( maxparts > 0 ) n_parts = std::min( maxparts, bt_MaxBodyParts() );

            auto bt_max_triangles_t = bt_MaxTrianglesPerPart();
            size_t nfacets_t = mesh.nfacets();

            size_t n_mapped_t = 0;
            for ( size_t index = 0 ; index < nfacets_t ; index += n_mapped_t ) {

              size_t n_indexes = nfacets_t - index;
              if ( n_indexes > bt_MaxTrianglesPerPart() ) n_indexes = bt_MaxTrianglesPerPart();

              btIndexedMesh mesh_t;

              // Set up acess to range of triangle mesh indexes
              mesh_t.m_numTriangles        = n_indexes;
              mesh_t.m_triangleIndexStride = mesh.indexes().data_size() * mesh.indexes().scalar_size();
              mesh_t.m_triangleIndexBase   = reinterpret_cast<const unsigned char *> ( mesh.get_index( index ).data() );
              mesh_t.m_indexType           = bt_type_code( mesh.get_index( index ).data() );

              // The full number of vertexs are used for every part
              mesh_t.m_numVertices  = mesh.nvectors();
              mesh_t.m_vertexBase   = reinterpret_cast<const unsigned char *> ( mesh.get_vector( 0 ).data() );
              mesh_t.m_vertexStride = mesh.vectors().data_size() * mesh.vectors().scalar_size();
              mesh_t.m_vertexType   = bt_type_code( mesh.get_vector( 0 ).data() );

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


        inline  btBvhTriangleMeshShape *create_collision_shape( const bool useCompression = true,
                                                                const bool buildBvh = true ) const {

          // Creates compressed and optimized Bullet meshes
          return ( this->create_collision_shape( m_bullet_mesh.get(), useCompression, buildBvh ) );
        }

        private:
          /// Variables for the Bullet system
          std::string                                  m_name;     /**! The name of the Bullet shape model */
          int                                          m_shape_id; /**! Identifier of the shape */
          int                                          m_part_no;  /**! Bullet shape part number */
          MODEL                                        m_mesh_data;
          std::shared_ptr<btTriangleIndexVertexArray>  m_bullet_mesh; /**! Ensure persistent references */
          // std::shared_ptr<btBvhTriangleMeshShape>      m_bullet_shape; /**! Ensure persistent references */

          inline void init(const std::string &name = "BulletMesh", 
                            const int shapeid = 0, const int partno = 0) {
            m_name      = name;
            m_shape_id  = shapeid;
            m_part_no   = partno;
            m_mesh_data = MODEL();
            m_bullet_mesh.reset();
            // m_bullet_shape.reset();
          }
          
    };

  // Convenience declarations
  typedef PsmrtsBulletMeshMap<BulletNativeMeshData>  NativeBulletMeshMap;

} // namespace psmrts:bullet

#endif // PsmrtsBulletMeshMap_hpp