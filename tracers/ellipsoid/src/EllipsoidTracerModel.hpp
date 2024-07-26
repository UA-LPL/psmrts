#ifndef EllipsoidTracerModel_hpp
#define EllipsoidTracerModel_hpp

#include <exception>
#include <string>
#include <vector>
#include <mutex>

#include <Eigen/Geometry>

#include <DskKernelModel.hpp>
#include <PsmrtsTracerModel.hpp>
#include <NaifEllipsoidShape.hpp>

namespace psmrts  {
  /**
   * @brief Abstract base class interface for ray tracing models
   * 
   * This abstract class provides a generic interface to ray tracing systems
   * that can be used in the PSMRTS system. These method function declarations
   * defines the minimum set of functions that any PSMRTS-compatible ray
   * tracer must implement.
   * 
   * Note this class does not attempt to specify constructors as there's going
   * to be many. Higher level s/w architectures can manage construction and 
   * usage of models that use this ABC.
   * 
   * @see PsmrtsShapeTracerAdaptor.hpp
   * 
   */
  class EllipsoidTracerModel : public PsmrtsTracerModel {
    public:
     EllipsoidTracerModel( ) {  }
     EllipsoidTracerModel( const naif::NaifEllipsoidShape &ellipsoid ) : 
                           PsmrtsTracerModel( ),
                           m_model( ellipsoid ) {  }
     EllipsoidTracerModel( const Eigen::Vector3d &radii,
                           const std::string &source = "ellipsoid" ) : 
                           PsmrtsTracerModel( ),
                           m_model( radii, source ) {  }

      virtual ~EllipsoidTracerModel() { }

      /* Name of tracer system (PSMRTS) */
      virtual std::string tracer_model_type() const {
        return ( m_model.tracer_model_type() );
      };

      /** Name of tracer model such as  "naifdsk" and "bullet" */
      virtual std::string tracer_model_name() const {
        return ( m_model.tracer_model_name() );
      }

      /** Unique tracer id of this instance */
      virtual std::string shape_tracer_id() const {
        return ( m_model.shape_tracer_id() );
      }

      /** Name of the shape model source */
      virtual std::string shapefile() const {
        return ( m_model.shapefile() );
      };

      /** Total number of plates/facets in model */
      virtual size_t plate_count()  const {
        return ( m_model.plate_count() );
      };

      /** Total verticies in the model */
      virtual size_t vertex_count() const {
        return ( m_model.plate_count() );
      };

      /** Returns the maximum radius in the modek */
      virtual double maximum_radius() const {
        return ( m_model.maximum_radius() );
      };

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
      virtual bool ray_trace( const Eigen::Vector3d &observer,
                              const Eigen::Vector3d &lookdir,
                              PsmrtsRayTrace &ray ) const {
        return ( m_model.ray_trace( observer, lookdir, ray ) );
      }

      /**
       * @brief Get the facet object at the ray intersection
       * 
       * @param ray 
       * @return true 
       * @return false 
       */
      virtual bool get_facet( const PsmrtsRayTrace &ray,
                              PsmrtsRayTrace::FacetDatum &facet ) const {
        return ( m_model.get_facet( ray, facet ) );
      }


      /** Clone a copy of this shape tracer model */
      virtual PsmrtsTracerModel *clone() const {
        return ( new EllipsoidTracerModel( m_model ) );
      }

      /** Return an ellipsoid tracer for the shape */
      virtual PsmrtsTracerModel *ellipsoid() const {
        return ( new EllipsoidTracerModel( m_model.radii() ) );
      }
    
    protected:
      naif::NaifEllipsoidShape m_model;
  };

} // namespace psmrts

#endif
