/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsBulletWorldModel_hpp
#define PsmrtsBulletWorldModel_hpp

#include <string>
#include <memory>
#include <mutex>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include "BulletSystemModel.hpp"
#include "PsmrtsBulletMeshMap.hpp"
#include "PsmrtsBulletClosestRayCallback.hpp"
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>


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
      PsmrtsBulletWorldModel( );

      /** Construct an array of values */
      PsmrtsBulletWorldModel( const std::string &name );

      PsmrtsBulletWorldModel( const PsmrtsBulletMeshMap &mesh, 
                              const std::string &name, 
                              const bool useCompression = true,
                              const bool buildBvh = true );

      /** Destructor - order of destruction is important here */
      virtual ~PsmrtsBulletWorldModel();

      /** Returns validity state of the world model and its' datum */
      bool isValid() const;
      bool useCompression() const;
      bool useBuildBvh() const;
      bool useThreadSafety() const;

      /** Returns world model name, ie. Body-Fixed-Coordinate-System */
      const std::string &name() const;

      /** Adds an additional body object to the world model */
      btCollisionObject *add_body( btBvhTriangleMeshShape *shape,
                                   void *userptr = nullptr );

      /** Adds an additional body object, compressed and optimized, to the world model */
      btCollisionObject *add_body( const PsmrtsBulletMeshMap &mesh,
                                   const bool useCompression = true,
                                   const bool buildBvh = true,
                                   void *userptr = nullptr  );

      /** Returns true if ray trace result contains a hit, directing the trace data into the ray parameter */
      bool extract_ray_trace_results( const PsmrtsBulletClosestRayCallback &results,
                                      PsmrtsRayTrace &ray ) const;

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
      bool ray_trace( const Eigen::Vector3d &observer, 
                      const Eigen::Vector3d &lookdir,
                      PsmrtsRayTrace &ray ) const;

      bool ray_trace( PsmrtsRayTrace &ray ) const;

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
      bool bullet_ray_trace( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir,
                             PsmrtsBulletClosestRayCallback &results ) const;
      /** Returns the model's associated mesh */
      const PsmrtsBulletMeshMap &mesh() const;


      double elapsed_life_time_s() const;

      size_t track_count() const;

      /**
       * @brief Return a standalone clone of the current tracker stats
       *  
       * Get a snapshot of the performance at this moment. I'd immediately get
       * an end_time = system_clock_time
       * 
       * @return PsmrtsThreadSafeCounter 
       */
      PsmrtsThreadSafeCounter performance_snapshot() const;

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
      PsmrtsBulletMeshMap    m_mesh_map; /**! The mesh map to trace */
      bool                   m_usecompression;
      bool                   m_buildbvh;

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
      void initWorld( const std::string &name = "Body-Fixed-Coordinate-System" );


  };

  // Declare a shared pointer type for the world model
  using SharedBulletWorldModel = std::shared_ptr<PsmrtsBulletWorldModel>;

} // namespace psmrts::bullet

#endif // PsmrtsBulletWorldModel_hpp
