#ifndef PsmrtsBulletWorldModel_hpp
#define PsmrtsBulletWorldModel_hpp

#include <string>
#include <memory>
#include <exception>

#include <Eigen/Geometry>

#include <RayTrace.hpp>
#include <PsmrtsDataModel.hpp>
#include <PsmrtsBulletMeshMap.hpp>

namespace psmrts {
  namespace bullet {

  /**
   * @brief PsmrtsBulletWorldModel - general maintainance of the Bullet Ray Tracing System
   *
   * @author Kris J. Becker, University of Arizona
   * @history 2024-05-10 Kris J. Becker  Original Version
   */

    class PsmrtsBulletWorldModel {
      public:
        /** Default constructor */
        PsmrtsBulletWorldModel( ) {
          init( );
        }

        /** Construct an array of values */
        PsmrtsBulletWorldModel( const std::string &name ) {
          init( );
        }

        PsmrtsBulletWorldModel( const PsmrtsBulletMeshMap &mesh, 
                                const std::string &name ) {
          init( );
          add_body( mesh );
        }

        /** Destructor - order of destruction is important here */
        virtual ~PsmrtsBulletWorldModel() { 
          m_collision.reset();
          m_dispatcher.reset();
          m_broadphase.reset();
          m_world.reset();
          m_mutex.reset();
        }

        inline add_body( const PsmrtsBulletShape &shape ) {

        }


        inline bool ray_trace( const Eigen::Vector3d &observer, 
                                const Eigen::Vector3d &lookdir,
                                btCollisionWorld::RayResultCallback &results ) const {

        }

        inline bool extract_ray_trace( const btCollisionWorld::RayResultCallback &results,
                                       RayTrace &raytrace ) const {

        }


      private:
        std::deque<PsmrtBulletMeshMap> m_mesh_bodies;

        /// Variables for the Bullet system
        std::string            m_name; /**! The name of the Bullet world. */
        int                    m_id;   /**! Identifier of the shape */

            // Order of these pointers matter due to destructor behavior!
        std::shared_ptr<btDefaultCollisionConfiguration> m_collision; /**! The collision
                                                                              configuration for
                                                                              the world. */
        std::shared_ptr<btCollisionDispatcher>           m_dispatcher; /**! The dispatcher for the
                                                                              world. */
        std::shared_ptr<btBroadphaseInterface>           m_broadphase; /**! The interface for overlaps
                                                                              in the world's aabb
                                                                              acceleration tree. */
        std::shared_ptr<btCollisionWorld>                m_world; /**! The Bullet collision world that
                                                                          contains the representation of
                                                                          the body/target. */
        std::shared_ptr<std::mutex>                      m_mutex;     //!< Mutex for thread safety


          /** Initialize a new Bullet world structure   */
          void initWorld(const std::string &name = "Body-Fixed-Coordinate-System") { 
            m_name = name;   
            m_collision.reset( new btDefaultCollisionConfiguration() );
            m_dispatcher.reset(new btCollisionDispatcher( m_collision ) );
            m_broadphase.reset( new btDbvtBroadphase() );  // Could also be an AxisSweep
            m_world.reset( new btCollisionWorld( m_dispatcher, 
                                                 m_broadphase, 
                                                 m_collision ) );
            m_mutex.reset( new QMutex() );
            m_mesh_bodies.clear(); 
          }

    };
  }  // namespace bullet  
}

#endif // PsmrtsBulletWorldModel_hpp