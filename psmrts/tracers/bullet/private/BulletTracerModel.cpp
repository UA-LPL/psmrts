#include <string>

#include <Eigen/Geometry>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include "PsmrtsBulletWorldModel.hpp"
#include "BulletTracerModel.hpp"

namespace psmrts::bullet {

      BulletTracerModel::BulletTracerModel( ) {  }
      BulletTracerModel::BulletTracerModel( const PsmrtsBulletWorldModel &bt_model ) :
                                            m_bullet_model( bt_model ) {  }

      /** Returns name of tracer model, ie. bullet */
      std::string BulletTracerModel::tracer_model_name() const {
        return ( m_bullet_model.mesh().mesh_type() );
      }

      /** Name of the shape model source */
      const std::string &BulletTracerModel::shapefile() const {
        return ( m_bullet_model.name() );
      }

      /** Total number of plates/facets in model */
      size_t BulletTracerModel::plate_count()  const {
        return ( m_bullet_model.mesh().nfacets() );
      }

      /** Total verticies in the model */
      size_t BulletTracerModel::vertex_count() const{
        return ( m_bullet_model.mesh().nvectors() );
      }

      /** Returns the maximum radius in the model */
      double BulletTracerModel::maximum_radius() const {
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
      bool BulletTracerModel::ray_trace( const Eigen::Vector3d &observer,
                              const Eigen::Vector3d &lookdir,
                              PsmrtsRayTrace &ray ) const {
        // this->local_tracker()++;
        return ( this->ray_trace( ray.reset( observer, lookdir ) ) );
      }

      bool BulletTracerModel::ray_trace( PsmrtsRayTrace &ray ) const {
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
      bool BulletTracerModel::get_facet( const PsmrtsRayTrace &ray,
                             PsmrtsRayTrace::FacetDatum &facet ) const {

       // Sanity check validity of raytrace
        facet.m_has_facet = false;

        if ( ray.hasHit() && m_bullet_model.mesh().isValid() ) {
          facet = m_bullet_model.mesh().get_facet( ray.plateid(), ray.segment_number() );
        }

        return ( facet.isValid() );
      }


} // namespace psmrts
