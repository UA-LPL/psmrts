#ifndef BulletTracerModel_hpp
#define BulletTracerModel_hpp

#include <string>

#include <Eigen/Geometry>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include "PsmrtsBulletWorldModel.hpp"
#include <psmrts/tracers/ellipsoid/private/EllipsoidTracerModel.hpp>

namespace psmrts::bullet {
  /**
   * @brief Abstract base class interface for ray tracing models
   * 
   * This class provides the PSMRST tracer model for Bullet tracers.
   * 
   */
  class BulletTracerModel  {
    public:
      BulletTracerModel( ) {  }
      BulletTracerModel( const PsmrtsBulletWorldModel &bt_model ) :
                               m_bullet_model( bt_model ) {  }

      virtual ~BulletTracerModel() { }

      /** Returns name of tracer model, ie. bullet */
      inline std::string tracer_model_name() const {
        return ( m_bullet_model.mesh().mesh_type() );
      }

      /** Name of the shape model source */
      inline const std::string &shapefile() const {
        return ( m_bullet_model.name() );
      }

      /** Total number of plates/facets in model */
      inline size_t plate_count()  const {
        return ( m_bullet_model.mesh().nfacets() );
      }

      /** Total verticies in the model */
      inline size_t vertex_count() const{
        return ( m_bullet_model.mesh().nvectors() );
      }

      /** Returns the maximum radius in the model */
      inline double maximum_radius() const {
        return ( m_bullet_model.mesh().maximum_radius() );
      }

      /**
       * @brief Ray trace method on the shape model in this tracer
       * 
       * The main method used to run individual body-fixed ray traces from 
       * an observer point and a look direction vector. The origin of the
       * "observer" vector is the origin of the planet body and extends
       * outward, presumeably, beyond the maximum radius of the surface in
       * this model. From that point, is the origin of the "lookdir" vector
       * from which to trace for an intersection with the shape model
       * surface. 
       * 
       * The PsmrtsRayTrace class contains the results of the ray trace and can
       * be used in subsequent operations.
       * 
       * @param observer Location of the observer (s/c) relative to the
       *                   center of the target body
       * @param lookdir  Look direction of the ray from the observer to
       *                   trace for intersections
       * @param ray      PsmrtsRayTrace returns the results of the trace
       * @return true    If the trace intercepts the shape
       * @return false   If no ray trace intercept was found
       */
      inline bool ray_trace( const Eigen::Vector3d &observer,
                              const Eigen::Vector3d &lookdir,
                              PsmrtsRayTrace &ray ) const {
        // this->local_tracker()++;
        return ( this->ray_trace( ray.reset( observer, lookdir ) ) );
      }

      inline bool ray_trace( PsmrtsRayTrace &ray ) const {
        // this->local_tracker()++;
        return ( m_bullet_model.ray_trace( ray ) );
      }


      /**
       * @brief Get the facet object at the ray intersection
       * 
       * @param ray 
       * @return true 
       * @return false 
       */
      inline bool get_facet( const PsmrtsRayTrace &ray,
                             PsmrtsRayTrace::FacetDatum &facet ) const {

       // Sanity check validity of raytrace
        facet.m_has_facet = false;

        if ( ray.hasHit() && m_bullet_model.mesh().isValid() ) {
          facet = m_bullet_model.mesh().get_facet( ray.plateid(), ray.segment_number() );
        }

        return ( facet.isValid() );
      }

    private:
      PsmrtsBulletWorldModel   m_bullet_model;
    
  };

} // namespace psmrts

#endif
