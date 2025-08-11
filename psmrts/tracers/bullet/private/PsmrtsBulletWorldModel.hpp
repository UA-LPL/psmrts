#ifndef PsmrtsBulletWorldModel_hpp
#define PsmrtsBulletWorldModel_hpp

#include <string>
#include <memory>
#include <exception>
#include <mutex>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <BulletSystemModel.hpp>
#include <PsmrtsBulletMeshMap.hpp>
#include <PsmrtsBulletClosestRayCallback.hpp>

namespace psmrts::bullet {

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
        initWorld( );
      }

      /** Construct an array of values */
      PsmrtsBulletWorldModel( const std::string &name ) {
        initWorld( name );
      }

      PsmrtsBulletWorldModel( const PsmrtsBulletMeshMap &mesh, 
                              const std::string &name ) {
        initWorld( name );
        add_body( mesh );
      }

      /** Destructor - order of destruction is important here */
      virtual ~PsmrtsBulletWorldModel() { 

        // Order is important!
        m_bt_object.datum() = BtShapeObject();
        m_world.reset();
        m_broadphase.reset();
        m_dispatcher.reset();
        m_collision.reset();
      }

      /** Returns validity state of the world model and its' datum */
      inline bool isValid() const {
        return ( m_bt_object.datum().isValid() );
      }

      /** Returns world model name, ie. Body-Fixed-Coordinate-System */
      inline const std::string &name() const {
        return ( m_name );
      }

      /** Adds an additional body object to the world model */
      inline btCollisionObject *add_body( btBvhTriangleMeshShape *shape,
                                          void *userptr = nullptr ) {

        m_bt_object.datum().m_sbt_shape.reset( shape );
        m_bt_object.datum().m_sbt_object.reset( new btCollisionObject() );

        m_bt_object.datum().object()->setCollisionShape( m_bt_object.datum().shape() );
        if ( nullptr != userptr ) {
          m_bt_object.datum().object()->setUserPointer( userptr );
        }

        m_world->addCollisionObject( m_bt_object.datum().object() );
        m_world->updateAabbs();

        return ( m_bt_object.datum().object() );
      }

      /** Adds an additional body object, compressed and optimized, to the world model */
      inline btCollisionObject *add_body( const PsmrtsBulletMeshMap &mesh,
                                          const bool useCompression = true,
                                          const bool buildBvh = true,
                                          void *userptr = nullptr  ) {

        // To ensure this memory remains viable for the life of the tracer...
        m_mesh_map = mesh;

        return ( add_body( mesh.create_collision_shape( useCompression, buildBvh ), userptr ) );
      }

      /** Returns true if ray trace result contains a hit, directing the trace data into the ray parameter */
      inline bool extract_ray_trace_results( const PsmrtsBulletClosestRayCallback &results,
                                             PsmrtsRayTrace &ray ) const {

        ray.reset( PsmrtsBulletClosestRayCallback::toStdVector( results.observer() ),
                   PsmrtsBulletClosestRayCallback::toStdVector( results.lookdir()  ) );

        ray.datum().m_hit = results.hasHit();
        ray.datum().m_xyz = PsmrtsBulletClosestRayCallback::toStdVector( results.xyz() );
        ray.datum().m_normal = PsmrtsBulletClosestRayCallback::toStdVector( results.normal() );

        ray.datum().m_plateid = results.triangleIndex();
        ray.datum().m_segment = results.partId();

        return ( ray.hasHit() );
      }
      /**
       * @brief Bullet World Model Ray Trace
       * 
       * This method is used to run an individual body-fixed ray trace from
       * an observer point and a look direction vector. The origin of the
       * "observer" vector is the origin of the planet body and extends
       * outward, presumeably, beyond the maximum radius of the surface in
       * this model. From that point, is the origin of the "lookdir" vector
       * from which to trace for an intersection with the shape model
       * surface. 
       * 
       * The PsmrtsRayTrace class parameter contains the results of the trace,
       * and returns true if results hit.
       * 
       * @param observer Location of the observer (s/c) relative to the
       *                   center of the target body
       * @param lookdir Look direction of the ray from the observer to
       *                   trace for intersections
       * @param ray     PsmrtsRayTrace returns the results of the trace
       * @return true   If the trace intercepts the shape
       * @return false  If no ray trace intercept was found
       */
      inline bool ray_trace( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir,
                             PsmrtsRayTrace &ray ) const {


        Eigen::Vector3d t_lookdir = observer + ( lookdir.normalized()  * ( observer.norm() * 2.0 ) );

        btVector3 b_observer = PsmrtsBulletClosestRayCallback::toBtVector( observer );
        btVector3 b_lookdir  = PsmrtsBulletClosestRayCallback::toBtVector( t_lookdir );

        PsmrtsBulletClosestRayCallback results(b_observer, b_lookdir );
        (void) bullet_ray_trace( observer, t_lookdir, results );
        return ( extract_ray_trace_results( results, ray ) );
      }

      /**
       * @brief Bullet World Model Callback Ray Trace
       * 
       * This method is used to run a bullet-specific individual body-fixed ray 
       * trace from an observer point and a look direction vector. The origin 
       * of the "observer" vector is the origin of the planet body and extends
       * outward, presumeably, beyond the maximum radius of the surface in
       * this model. From that point, is the origin of the "lookdir" vector
       * from which to trace for an intersection with the shape model
       * surface. 
       * 
       * The PsmrtsBulletClosestRayCallback class parameter contains the results 
       * of the trace, and returns true if results hit.
       * 
       * @param observer Location of the observer (s/c) relative to the
       *                   center of the target body
       * @param lookdir Look direction of the ray from the observer to
       *                   trace for intersections
       * @param results PsmrtsBulletClosestRayCallback class holds trace
       *                   results
       * @return true   If the trace intercepts the shape
       * @return false  If no ray trace intercept was found
       */
      inline bool bullet_ray_trace( const Eigen::Vector3d &observer, 
                                    const Eigen::Vector3d &lookdir,
                                    PsmrtsBulletClosestRayCallback &results ) const {


        // Lock up Bullet for thread safety ( >=c++17 )
        btVector3 rayStart( observer[0], observer[1], observer[2] );
        btVector3 rayEnd( lookdir[0], lookdir[1], lookdir[2] );

        // Check for runtime single thread safety option
        if ( true == m_thread_safety ) {
          std::scoped_lock mylocker( m_bt_object.mutex() );
          m_tracker++;
          m_world->rayTest( rayStart, rayEnd, results);
        }
        else {
          m_tracker++;
          m_world->rayTest( rayStart, rayEnd, results);
        }

        return ( results.hasHit() );
      }

      /** Returns the model's associated mesh */
      inline const PsmrtsBulletMeshMap &mesh() const {
        return ( m_mesh_map );
      }


      inline double elapsed_life_time_s() const {
        return ( m_tracker.runtime_s() );
      }

      inline size_t track_count() const {
        return ( m_tracker.count() );
      }

      /**
       * @brief Return a standalone clone of the current tracker stats
       *  
       * Get a snapshot of the performance at this moment. I'd immediately get
       * an end_time = system_clock_time
       * 
       * @return PsmrtsThreadSafeCounter 
       */
      inline PsmrtsThreadSafeCounter performance_snapshot() const {
        return ( m_tracker.clone() );
      }

    private:
      typedef  std::shared_ptr<btCollisionShape>       SharedBulletShape;
      typedef  std::shared_ptr<btCollisionObject>      SharedBulletObject;
      typedef struct bt_shape_object {

        SharedBulletShape  m_sbt_shape;
        SharedBulletObject m_sbt_object;

        bt_shape_object() : m_sbt_shape(), m_sbt_object() { }
        bt_shape_object( btCollisionShape *shape, btCollisionObject *object ) : 
                        m_sbt_shape( shape ), m_sbt_object( object) { }
        virtual ~bt_shape_object() { }

        inline bool isValid() const { 
          return ( m_sbt_shape && m_sbt_object );
        }

        inline btCollisionObject *object() const { 
          return ( m_sbt_object.get() );
        }

        inline  btCollisionShape *shape() const { 
          return ( m_sbt_shape.get() );
        }

      } BtShapeObject;


      /// Variables for the Bullet system
      std::string            m_name; /**! The name of the Bullet world. */
      int                    m_id;   /**! Identifier of the shape */
      PsmrtsBulletMeshMap    m_mesh_map; /**! The mesh map to trace */

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
      DatumMutexWrapper<BtShapeObject>                 m_bt_object;     //!< Mutex wrapped shape model
      PsmrtsThreadSafeCounter                          m_tracker;       // Tracks times and copy counts
      bool                                             m_thread_safety; //!< Enable single thread safety


      /** Initialize a new Bullet world structure   */
      void initWorld( const std::string &name = "Body-Fixed-Coordinate-System" ) { 

        m_name = name;   

        m_collision.reset( new btDefaultCollisionConfiguration() );
        m_dispatcher.reset(new btCollisionDispatcher( m_collision.get() ) );
        m_broadphase.reset( new btDbvtBroadphase() );  // Could also be an AxisSweep

        m_world.reset( new btCollisionWorld( m_dispatcher.get(), 
                                              m_broadphase.get(), 
                                              m_collision.get() ) );
        m_bt_object.datum() = BtShapeObject();
        m_tracker           = PsmrtsThreadSafeCounter();
        m_thread_safety     = true;
      }


  };

} // namespace psmrts::bullet

#endif // PsmrtsBulletWorldModel_hpp