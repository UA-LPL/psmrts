#ifndef PsmrtsTracerModel_hpp
#define PsmrtsTracerModel_hpp

#include <exception>
#include <string>
#include <vector>
#include <mutex>

#include <Eigen/Geometry>
#include <PsmrtsUtilities.hpp>
#include <RayTrace.hpp>

namespace psmrts {

  class PsmrtsTracerModel {
    public:
      PsmrtsTracerModel( ) {  }
      virtual ~PsmrtsTracerModel() { }

      virtual std::string tracer_model_type() const = 0;
      virtual std::string tracer_model_name() const = 0;
      virtual std::string shape_tracer_id()   const = 0;
      virtual std::string shapefile()         const = 0;

      virtual size_t plate_count()  const = 0;
      virtual size_t vertex_count() const = 0;

      virtual bool ray_trace( const Eigen::Vector3d &observer,
                             const Eigen::Vector3d &lookdir,
                             RayTrace &ray  ) const = 0;

      virtual PsmrtsTracerModel *clone() const = 0;
      virtual PsmrtsTracerModel *ellipsoid() const = 0;
    
  };

} // namespace psmrts

#endif
