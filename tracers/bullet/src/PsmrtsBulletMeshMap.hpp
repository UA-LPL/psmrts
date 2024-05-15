#ifndef PsmrtsBulletMeshMap_hpp
#define PsmrtsBulletMeshMap_hpp

#include <string>
#include <memory>
#include <exception>

#include <Eigen/Geometry>
#include <btBulletDynamicsCommon.h>

#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>

namespace psmrts {
  namespace bullet {

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
   * @author Kris J. Becker, University of Arizona
   * @history 2024-05-15 Kris J. Becker  Original Version
   */
    template <typename MESHDATA = PsmrtsDoubleMeshData> 
      class PsmrtsBulletMeshMap {
        public:
          typedef btScalar                         BulletVertexType;
          typedef int                              BulletIndexType;
          typedef Eigen::Vector3<BulletVertexType> BulletVector;
          typedef Eigen::Vector3<BulletIndexType>  BulletIndex;
          typedef PsmrtsDataModel<BulletIndex>     BulletIndexArray;
          typedef PsmrtsDataModel<BulletVector>    BulletVertexArray;
          typedef PsmrtsMeshData<BulletVertexArray, BulletVertexArray> BulletMeshMap;

          typedef btTriangleIndexVertexArray BulletMesh;  /**! Bullet mesh type*/

          /** Default constructor */
          PsmrtsBulletMeshMap( ) {
            init( );
          }

          /** Construct an array of values */
          PsmrtsBulletMeshMap( const PsmrtsDataModel & std::string &name, const int mesh_id ) {
            init( );
            m_name = name;
            m_id = mesh_id;
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

          inline const MESHDATA &data() const {
            return ( m_mesh );
          }

          inline const BulletMesh *mesh() const {
            return ( m_bullet_mesh.get() );
          }

          template <typename T> 
            static inline btVector3 toBullet( const T &v ) {
              return ( b3Vector )
            }

          /// Variables for the Bullet system
          private:
            std::string                m_name;     /**! The name of the Bullet world. */
            int                        m_shape_id; /**! Identifier of the shape */
            int                        m_part_no;  /**! Bullet shape part number */
            MESHDATA                   m_mesh;
            std::shared_ptr<BulleMesh> m_bullet_mesh; /**! Only way to ensure persistence references */

            inline void init() {
              m_name      = "BulletMesh";
              m_shape_id  = 0;
              m_part_no   = 0;
              m_mesh_data = MESHDATA();
              m_bullet_mesh.reset();
            }
            
            inline void init( const MESHDATA &mesh, const std::string &name = "BulletMesh", 
                              const int shapeid = 0, const int partno = 0 ) {
              m_name      = name;
              m_shape_id  = shapeid;
              m_part_no   = partno;
              m_mesh_data = mesh;

              if ( m_mesh_data.isValid() ) {
                m_bullet_mesh.reset( new BulletMesh() );


              }
              else {
              m_bullet_mesh.reset(  );
              }

            }            

      };
  }  // namespace bullet  
}

#endif // PsmrtsBulletMeshMap_hpp