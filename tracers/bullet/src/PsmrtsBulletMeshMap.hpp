#ifndef PsmrtsBulletMeshMap_hpp
#define PsmrtsBulletMeshMap_hpp

#include <string>
#include <memory>
#include <exception>

#include <Eigen/Geometry>

#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>

#include <BulletSystemModel.hpp>
#include <PsmrtsOBJAsset.hpp>


namespace psmrts {
  namespace bullet {

    /**! Special definitions of the Bullet con*/
    typedef btScalar                                 BulletInternalVertexType;
    typedef int                                      BulletInternalIndexType;
    typedef Eigen::Vector3<BulletInternalVertexType> BulletInternalVector;
    typedef Eigen::Vector3<BulletInternalIndexType>  BulletInternalIndex;
    typedef PsmrtsDataModel<BulletInternalIndex>     BulletInternalIndexArray;
    typedef PsmrtsDataModel<BulletInternalVector>    BulletInternalVertexArray;
    typedef PsmrtsMeshData<BulletInternalIndexArray, BulletInternalVertexArray> BulletInternalMeshModel;

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
    template <typename T = BulletInternalMeshModel> 
      class PsmrtsBulletMeshMap {
        public:
          // The native Bullet mesh types

          typedef btTriangleIndexVertexArray BulletMeshMap;  /**! Bullet mesh type*/

          // The desired data buffer type to map to a Bullet indexed mesh
          typedef typename T::MeshFacetIndex    MeshFacetIndex;
          typedef typename T::MeshFacetVector   MeshFacetVector;

          typedef typename T::value_type        index_type;
          typedef typename T::value_type        vector_type;

          /** Default constructor */
          PsmrtsBulletMeshMap( ) {
            init( );
          }

          /** Construct an array of values */
          PsmrtsBulletMeshMap( const T &mesh, std::string &name, 
                               const int mesh_id, const int partno = 0 ) {
            init( name, mesh_id, partno );
            (void) create_map_mesh( mesh );
          }

          PsmrtsBulletMeshMap( const PsmrtsOBJAsset &obj_t ) {
            init( obj_t.obj_source() );
            (void) create_map_mesh( T( obj_t.get_indexes( ), obj_t.get_vectors() ) );
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
            return ( m_mesh_id );
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
                                     "::" + std::to_string( this->partno() );
            return ( mesh_id_str );
          }

          inline const T &data() const {
            return ( m_mesh );
          }

          inline const BulletMesh *mesh() const {
            return ( m_bullet_mesh.get() );
          }

          template <typename T> 
            static inline btVector3 toBullet( const T &v ) {
              return ( b3Vector( v[0], v[1], v[2]) );
            }

          /// Variables for the Bullet system
          private:
            std::string                    m_name;     /**! The name of the Bullet world. */
            int                            m_shape_id; /**! Identifier of the shape */
            int                            m_part_no;  /**! Bullet shape part number */
            T                              m_mesh_data;
            std::shared_ptr<BulletMeshMap> m_bullet_mesh; /**! Only way to ensure persistence references */



            inline void init(const std::string &name = "BulletMesh", 
                             const int shapeid = 0, const int partno = 0) {
              m_name      = name;
              m_shape_id  = shapeid;
              m_part_no   = partno;
              m_mesh_data = T();
              m_bullet_mesh.reset();
            }
            
            inline BulletMeshMap *create_map_mesh( const T &mesh, const int maxparts = 0 ) {
              
              m_mesh_data = mesh;
              size_t n_parts = bt_MaxBodyParts();
              if ( maxparts > 0 ) n_parts = std::min( maxparts, bt_MaxBodyParts() );

              // Allocate a new mesh map
              std::unique_ptr<BulletMeshMap> bt_mesh;

              if ( m_mesh_data.isValid() ) {
                bt_mesh.reset( new BulletMesh() );

                auto bt_max_triangles_t = bt_MaxTrianglesPerPart();
                size_t nfacets_t = m_mesh_data.nfacets();

                size_t n_mapped_t = 0;
                for ( size_t index = 0 ; index < nfacets_t ; index += n_mapped_t ) {

                  size_t n_indexes = nfacets_t - index;
                  if ( n_indexes > bt_MaxTrianglesPerPart() ) n_indexes = bt_MaxTrianglesPerPart();

                  btIndexedMesh mest_t;

                  mesh_t.m_numTriangles        = n_left_t;
                  mesh_t.m_triangleIndexStride = m_mesh_data.indexes().data_size() * m_mesh_data.indexes().scalar_size();
                  mesh_t.m_triangleIndexBase   = m_mesh_data.get_index( index ).data();
                  mesh_t.m_indexType           = bt_type_code( m_mesh_data.get_index( index ).data() );

                  mesh_t.m_numVertices  = m_mesh_data.nvectors();
                  mesh_t.m_vertexBase   = m_mesh_data.get_vector( 0 ).data();
                  mesh_t.m_vertexStride = m_mesh_data.vectors().data_size() * m_mesh_data.vectors().scalar_size();
                  mesh_t.m_vertexType   = bt_type_code( m_mesh_data.get_vector( 0 ).data() );

                  // Add this offset
                  bt_mesh->addIndexedMesh( mesh_t );
                }
              
                // Store this mesh
                m_bullet_mesh.reset( bt_mesh.release() ); 
                return ( m_bullet_mesh.get() );
              }

            }            

      };

    // Convenience declarations
    typedef PsmrtsBulletMeshMap<BulletInternalMeshModel>  NativeBulletMesh;

    typedef PsmrtsDoubleMeshData                          DefaultPsmrtsBulletMesh;
    typedef PsmrtsBulletMeshMap<DefaultPsmrtsBulletMesh>  DefaultBulletMesh;

  }  // namespace bullet  
} // namespace psmrts

#endif // PsmrtsBulletMeshMap_hpp