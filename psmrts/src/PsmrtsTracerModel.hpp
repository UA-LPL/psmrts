#ifndef PsmrtsTracerModel_hpp
#define PsmrtsTracerModel_hpp

#include <exception>
#include <string>
#include <vector>
#include <mutex>

#include <Eigen/Geometry>
#include <PsmrtsUtilities.hpp>
#include <PsmrtsRayTrace.hpp>

namespace psmrts {
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
  class PsmrtsTracerModel {
    public:
      PsmrtsTracerModel( ) {  }
      virtual ~PsmrtsTracerModel() { }

      /* Name of tracer system (PSMRTS) */
      virtual std::string tracer_model_type() const = 0;

      /** Name of tracer model such as  "naifdsk" and "bullet" */
      virtual std::string tracer_model_name() const = 0;

      /** Unique tracer id of this instance */
      virtual std::string shape_tracer_id()   const = 0;

      /** Name of the shape model source */
      virtual std::string shapefile()         const = 0;

      /** Total number of plates/facets in model */
      virtual size_t plate_count()  const = 0;

      /** Total verticies in the model */
      virtual size_t vertex_count() const = 0;

      /** Returns the maximum radius in the modek */
      virtual double maximum_radius() const = 0;

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
                              PsmrtsRayTrace &ray ) const = 0;

      /** Clone a copy of this shape tracer model */
      virtual PsmrtsTracerModel *clone() const = 0;

      /** Return an ellipsoid tracer for the shape */
      virtual PsmrtsTracerModel *ellipsoid() const = 0;
    
  };

} // namespace psmrts

#endif
