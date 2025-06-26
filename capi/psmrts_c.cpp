

#include <string>
#include <Eigen/Geometry>

#include "psmrts_version.h"

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsBuffer.hpp>
#include <PsmrtsMeshData.hpp>
#include <PsmrtsRayTrace.hpp>
#include <PsmrtsOBJFormat.hpp>
#include <PsmrtsDSKFormat.hpp>
#include <PsmrtsPLYFormat.hpp>
#include <EllipsoidTracerModel.hpp>
#include <PsmrtsBulletWorldModel.hpp>
#include <PsmrtsRequest.hpp>
#include <PsmrtsPriorityTracer.hpp>


/*============ PSMRTS C API type definitions ============*/
/* Must be defined before including psmrts_c.h */
#define PSMRTS_POINTERS 1
using PSMRTS_RayTrace            = psmrts::PRQRayTrace;
using PSMRTS_Shape          = psmrts::PsmrtsMeshData;
using PSMRTS_Tracer         = psmrts::PsmrtsTracerModel;
using PSMRTS_ShapeTracer    = psmrts::PsmrtsTracerModel;
using PSMRTS_PriorityTracer = psmrts::PsmrtsPriorityTracer;



/* Include the PSMRTS C api include */
#include "psmrts_c.h"

inline Eigen::Vector3d vector_to_eigen( const PSMRTS_Vector3d &v3d ) {
  return ( Eigen::Vector3d( { v3d.a, v3d.b, v3d.c } ) );
}

inline PSMRTS_Vector3d eigen_to_vector( const Eigen::Vector3d &v ) {
  PSMRTS_Vector3d v3d = { v[0], v[1], v[2] };
   return ( v3d );
}

extern "C" {

/*============ PSMRTS information functions ============*/

const char *psmrts_version() {
  return ( PROJECT_VERSION );
}

const char *psmrts_info() {
  return ( PSMRTS_VERSION );
}
#if 0
PSMRTS_ShapeTracer *psmrts_load_shape( const char *shape, const char *tracer ) {
  /* char *tracer = "tracer:bullet:optimizebvh=true"  */
  /* char *shape = "obj/data/bennu_20facets.obj"  */
  /* char *shape = "ellipsoid:0.298"  */
  std::string s_shape( shape );
  std::string s_tracer( tracer );
  std::vector<std::string> v_tracer = psmrts::string_tokenizer( s_tracer );
  std::vector<std::string> v_shape = psmrts::string_tokenizer( s_shape );

  if (v_tracer.size() == 0 || v_shape.size() == 0 ) {
    fprintf(stderr, "Error: Invalid Tracer Data.");
    return 0;
  }

  try {
    psmrts::PsmrtsMeshData mesh;
    if ( psmrts::psmrts_file_extension( v_shape ) == "obj" ) {
      mesh = psmrts::PsmrtsOBJFormat( v_shape );
    }
    else {
      if ( psmrts::psmrts_file_extension( v_shape ) == "ply" ) {
        mesh = psmrts::PsmrtsPLYFormat ply( v_shape );
      }
    }

    /* Allocate tracer */
    if ( v_tracer.find("dsk") != std::string::npos ) {
      return ( new psmrts::NaifDskShapeTracer ( v_shape ) );
    }
    else if ( v_tracer.find("ellipsoid") != std::string::npos ) {
      std::vector<std::string> tokens = psmrts::string_tokenizer(v_shape, "=:,");
      // Ex: "ellipsoid:0.1" -> { "ellipsoid", "0.1" }
      if (tokens.size() == 2) {
        Eigen::Vector3d radii( {std::stod(tokens[1]), std::stod(tokens[1]), std::stod(tokens[1]) } );
        return ( new psmrts::EllipsoidShapeTracer( radii ));
      } 
      // Ex: "ellipsoid:0.1,0.2" -> { "ellipsoid", "0.1", "0.2" }
      else if (tokens.size() == 3) {
        Eigen::Vector3d radii( {std::stod(tokens[1]), std::stod(tokens[2]), std::stod(tokens[2]) } );
        return ( new psmrts::EllipsoidShapeTacer( radii ));
      }
      // Ex: "ellipsoid:0.1,0.2,0.3" -> { "ellipsoid", "0.1", "0.2", "0.3" }
      else if (tokens.size() == 4) {
        Eigen::Vector3d radii( {std::stod(tokens[1]), std::stod(tokens[2]), std::stod(tokens[3]) } );
        return (new psmrts::EllipsoidShapeTracer( radii ));
      }
      else {
        // *** Error Condition *** 
        fprintf(stderr, "Error: Ellipsoid Format Conversion Issue.");
        return 0;
      }
    }
    else {
      psmrts::bullet::PsmrtsBulletWorldModel bt_world( mesh, v_shape );
      return ( new psmrts::BulletShapeTracer( bt_world ) );
    }
  }
  catch (...) {
    /* NOOP failure */
    fprintf(stderr, "Error: Invalid Tracer Format.");
    return 0;
  }


  return ( NULL );
}
#endif

PSMRTS_Vector3d psmrts_vector3d( const double x, const double y, const double z ) {
  PSMRTS_Vector3d v3d = { x, y, z };  
  return ( v3d );
}

PSMRTS_Vector3d psmrts_negate( const PSMRTS_Vector3d &v ) {
  PSMRTS_Vector3d v3d = { -v.x, -v.y, -v.z };  
  return ( v3d );
}

PSMRTS_Vector3d psmrts_subtract( const PSMRTS_Vector3d &v1, const PSMRTS_Vector3d &v2 ) {
  PSMRTS_Vector3d v3d = { v1.x-v2.x, v1.y-v2.y, v1.z-v2.z };  
  return ( v3d );
}


/**
 * @brief psmrts_create_ray - Creates a ray trace componet
 * 
 * This function creates a PSMRTS ray trace object that can be used to trace
 * on a shape. 
 * 
 * The observer position is a vector from the shape origin to its body-fixed
 * position relative to the target body origin.
 * 
 * The lookdir is converted to unit vector and eminates from the observer
 * location toward the target body.
 * 
 * It is up to the caller to check for valid pointer return.
 * 
 * @param observer Position (km) of the observer, origin of the ray
 * @param lookdir  Direction (km) vector from observer to trace
 * @return PSMRTS_RayTrace* Pointer to the resulting ray trace
 */
PSMRTS_RayTrace *psmrts_create_ray( const PSMRTS_Vector3d &observer, const PSMRTS_Vector3d &lookdir ) {
  return ( new PSMRTS_RayTrace( vector_to_eigen( observer ), vector_to_eigen( lookdir ) ) );
}

PSMRTS_RayTrace *psmrts_ray_trace( PSMRTS_RayTrace *ray, const PSMRTS_Tracer *tracer ) {
  tracer->ray_trace( ray->trace().observer(), ray->trace().lookdir(), ray->trace() );
  return ( ray );
}

PSMRTS_RayTrace *psmrts_ray_trace_v( const PSMRTS_Vector3d &observer,
                                const PSMRTS_Vector3d &lookdir,
                                const PSMRTS_Tracer *ellipsoid ) {
  return ( psmrts_ray_trace( psmrts_create_ray( observer, lookdir ), ellipsoid ) );
}

PSMRTS_Vector3d psmrts_ray_observer( const PSMRTS_RayTrace *ray ) {
  return ( eigen_to_vector( ray->trace().observer() ) );
}

PSMRTS_Vector3d psmrts_ray_lookdir( const PSMRTS_RayTrace *ray ) {
  return ( eigen_to_vector( ray->trace().lookdir() ) );
}

PSMRTS_BOOL psmrts_ray_has_hit( const PSMRTS_RayTrace *ray ) {
    return ( ray->trace().hasHit() ?  PSMRTS_TRUE : PSMRTS_FALSE );
}

PSMRTS_Vector3d psmrts_ray_xyz( const PSMRTS_RayTrace *ray ) {
  return ( eigen_to_vector( ray->trace().xyz() ) );
}

PSMRTS_Vector3d psmrts_ray_normal( const PSMRTS_RayTrace *ray ) {
  return ( eigen_to_vector( ray->trace().normal() ) );
}

double psmrts_ray_intercept_radius( const PSMRTS_RayTrace *ray) {
    return ( ray->trace().radius() );
}

double psmrts_ray_intercept_slant_distance( const PSMRTS_RayTrace *ray) {
    return ( ray->trace().slant_distance() );
}

// PSMRTS_ShapeTracer *psmrts_load_shape( const char *shape, const char *tracer );

// void psmrts_lonlat_to_xyz( const double longitude_d, const double latitude_d, const double radius_km, double xyz[3] ) {

// }

// void psmrts_vector_scale( const double v[3], const double scale, double vout[3] );
// PSMRTS_RayTrace *PSMRTS_RayTrace_trace( PSMRTS_ShapeTracer *tracer, const double scpos[3], const double lookdir[3] );

PSMRTS_BOOL psmrts_tracer_valid( const PSMRTS_ShapeTracer *tracer ) {
  return ( ( 0 != tracer ) ?  PSMRTS_TRUE : PSMRTS_FALSE );
}


void psmrts_free_ray( PSMRTS_RayTrace *trace ) {
  delete trace;
}

void psmrts_free_shape( PSMRTS_Shape *shape ) {
  delete shape;
}

void psmrts_free_tracer( PSMRTS_Tracer *tracer ){
  delete tracer;
}

void psmrts_free_shapetracer( PSMRTS_ShapeTracer *stracer ) {
  delete stracer;
}

void psmrts_free( PSMRTS_PriorityTracer *ptracer ) {
  delete ptracer;
}

}
