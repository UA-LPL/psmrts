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
#include <mutex>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include "BulletSystemModel.hpp"
#include "PsmrtsBulletMeshMap.hpp"
#include "PsmrtsBulletClosestRayCallback.hpp"
#include "PsmrtsBulletWorldModel.hpp"

namespace psmrts::bullet {

  /**
   * @brief PsmrtsBulletWorldModel - general maintainance of the Bullet Ray Tracing System
   *
   * @author Kris J. Becker, University of Arizona
   * @history 2024-05-10 Kris J. Becker  Original Version
   */

      /** Default constructor */
      PsmrtsBulletWorldModel::PsmrtsBulletWorldModel( ) {
        initWorld( );
      }

      /** Construct an array of values */
      PsmrtsBulletWorldModel::PsmrtsBulletWorldModel( const std::string &name ) {
        initWorld( name );
      }

      PsmrtsBulletWorldModel::PsmrtsBulletWorldModel( const PsmrtsBulletMeshMap &mesh, 
                                                      const std::string &name,
                                                      const bool useCompression,
                                                      const bool buildBvh ) {
        initWorld( name );
        add_body( mesh, useCompression, buildBvh );
      }

      /** Destructor - order of destruction is important here */
      PsmrtsBulletWorldModel::~PsmrtsBulletWorldModel() { 

        // Order is important!
        m_bt_object.datum() = BtShapeObject();
        m_world.reset();
        m_broadphase.reset();
        m_dispatcher.reset();
        m_collision.reset();
      }

      /** Returns validity state of the world model and its' datum */
      bool PsmrtsBulletWorldModel::isValid() const {
        return ( m_bt_object.datum().isValid() );
      }

      /** Return state of bullet shape compression status */
      bool PsmrtsBulletWorldModel::useCompression() const {
        return ( m_usecompression );
      }

      /** Return state of bound volume heirachy build status */
      bool PsmrtsBulletWorldModel::useBuildBvh() const {
        return ( m_buildbvh );
      }
      
      /** Return state of thread safety status */
      bool PsmrtsBulletWorldModel::useThreadSafety() const {
        return ( m_thread_safety );
      }
      
      /** Returns world model name, ie. Body-Fixed-Coordinate-System */
      const std::string &PsmrtsBulletWorldModel::name() const {
        return ( m_name );
      }

      /** Adds an additional body object to the world model */
      btCollisionObject *PsmrtsBulletWorldModel::add_body( btBvhTriangleMeshShape *shape,
                                                           void *userptr ) {

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
      btCollisionObject *PsmrtsBulletWorldModel::add_body( const PsmrtsBulletMeshMap &mesh,
                                                           const bool useCompression, 
                                                           const bool buildBvh,
                                                           void *userptr ) {

        // To ensure this memory remains viable for the life of the tracer...
        m_mesh_map = mesh;

        return ( add_body( mesh.create_collision_shape( useCompression, buildBvh ), userptr ) );
      }

      /** Returns true if ray trace result contains a hit, directing the trace data into the ray parameter */
      bool PsmrtsBulletWorldModel::extract_ray_trace_results( const PsmrtsBulletClosestRayCallback &results,
                                                               PsmrtsRayTrace &ray ) const {

        // Convert to Eigen vectors and compute the unit lookdir
        Eigen::Vector3d observer_v = PsmrtsBulletClosestRayCallback::toStdVector( results.observer() );                                                            
        Eigen::Vector3d lookdir_v  = PsmrtsBulletClosestRayCallback::toStdVector( results.lookdir() );
        Eigen::Vector3d lookdir_u = ( lookdir_v - observer_v ).normalized();

        // ray.reset( observer_v, lookdir_u );

        ray.datum().m_hit = results.hasHit();
        if ( ray.hasHit() ) {
          ray.datum().m_xyz = PsmrtsBulletClosestRayCallback::toStdVector( results.xyz() );
          ray.datum().m_normal = PsmrtsBulletClosestRayCallback::toStdVector( results.normal() );

          ray.datum().m_plateid = results.triangleIndex();
          ray.datum().m_segment = results.partId();
        }

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
      bool PsmrtsBulletWorldModel::ray_trace( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir,
                             PsmrtsRayTrace &ray ) const {
        return ( this->ray_trace( ray.reset( observer, lookdir ) ) );
      }

      bool PsmrtsBulletWorldModel::ray_trace( PsmrtsRayTrace &ray ) const {
                              
        Eigen::Vector3d t_lookdir = ray.observer() + ( ray.lookdir().normalized()  * ( ray.observer().norm() * 2.0 ) );
        btVector3 b_observer = PsmrtsBulletClosestRayCallback::toBtVector( ray.observer() );
        btVector3 b_lookdir  = PsmrtsBulletClosestRayCallback::toBtVector( t_lookdir );

        PsmrtsBulletClosestRayCallback results(b_observer, b_lookdir );
        (void) bullet_ray_trace( ray.observer(), t_lookdir, results );
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
      bool PsmrtsBulletWorldModel::bullet_ray_trace( const Eigen::Vector3d &observer, 
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
      const PsmrtsBulletMeshMap &PsmrtsBulletWorldModel::mesh() const {
        return ( m_mesh_map );
      }


      double PsmrtsBulletWorldModel::elapsed_life_time_s() const {
        return ( m_tracker.runtime_s() );
      }

      size_t PsmrtsBulletWorldModel::track_count() const {
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
      PsmrtsThreadSafeCounter PsmrtsBulletWorldModel::performance_snapshot() const {
        return ( m_tracker.clone() );
      }

      /** Initialize a new Bullet world structure   */
      void PsmrtsBulletWorldModel::initWorld( const std::string &name ) { 

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



} // namespace psmrts::bullet
