#ifndef BulletTracerModel_hpp
#define BulletTracerModel_hpp

#include <string>

#include <Eigen/Geometry>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include "PsmrtsBulletWorldModel.hpp"

namespace psmrts::bullet {
  /**
   * @brief Abstract base class interface for ray tracing models
   * 
   * This class provides the PSMRST tracer model for Bullet tracers.
   * 
   */
  class BulletTracerModel  {
    public:
      BulletTracerModel( );
      BulletTracerModel( const PsmrtsBulletWorldModel &bt_model );
      BulletTracerModel( const PsmrtsMeshData &mesh, 
                         const std::string &name );

      virtual ~BulletTracerModel() = default;

      /** Returns name of tracer model, ie. bullet */
      std::string tracer_model_name() const;

      /** Name of the shape model source */
      const std::string &shapefile() const;

      bool isValid() const;

      /** Total number of plates/facets in model */
      size_t plate_count()  const;

      /** Total verticies in the model */
      size_t vertex_count() const;

      /** Returns the maximum radius in the model */
      double maximum_radius() const;

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
      bool ray_trace( const Eigen::Vector3d &observer,
                              const Eigen::Vector3d &lookdir,
                              PsmrtsRayTrace &ray ) const;

      bool ray_trace( PsmrtsRayTrace &ray ) const;


      /**
       * @brief Get the facet object at the ray intersection
       * 
       * @param ray 
       * @return true 
       * @return false 
       */
      bool get_facet( const PsmrtsRayTrace &ray,
                             PsmrtsRayTrace::FacetDatum &facet ) const;


      const PsmrtsBulletWorldModel &model() const;

    private:
      PsmrtsBulletWorldModel   m_bullet_model;
    
  };

} // namespace psmrts

#endif
