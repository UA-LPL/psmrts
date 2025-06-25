
#include <Eigen/Geometry>

#include "psmrts_version.h"

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsBuffer.hpp>
#include <PsmrtsRayTrace.hpp>
#include <EllipsoidTracerModel.hpp>
#include <PsmrtsRequest.hpp>


/*============ PSMRTS C API type definitions ============*/
/* Must be defined before including psmrts_c.h */
using PSMRTS_Ray            = psmrts::RRQRayTrace;
using PSMRTS_Shape          = psmrts::PsmrtsMeshData;
using PSMRTS_Tracer         = psmrts::PsmrtsTracerModel;
using PSMRTS_ShapeTracer    = psmrts::PsmsrtsShapeTracer;
using PSMRTS_PriorityTracer = psmrts::PsmsrtsPriorityTracer;


/* Include the PSMRTS C api include */
#include <psmrts_c.h>

inline Eigen::Vector3d vector_to_eigen( const PSMRTS_Vector3d &v3d ) {
  return ( Eigen::Vector3d( { v3d.a, v3d.b, v3d.c } );
}

inline PSMRTS_Vector3d eigen_to_vector( const Eigen::Vector3d &v ) {
   return ( PSMRTS_Vector3d( { v[0], v[1], v[2] } ) );
}

extern "C" {

/*============ PSMRTS information functions ============*/

const char *psmrts_version() {
  return ( PROJECT_VERSION );
}

const char *psmrts_info() {
  return ( PSMRTS_VERSION );
}

PSMRTSShapeTracer *psmrts_load_shape( const char *shape, const char *tracer ) {
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
    PsmrtsMeshData mesh;
    if ( psmrts_file_extension( v_shape ) == "obj" ) {
      mesh = psmrts::PsmrtsOBJFormat( v_shape );
    }
    else {
      if (psmrts_file_extension( v_shape ) == "ply" ) {
        mesh = psmrts::PsmrtsPLYFormat ply( v_shape );
      }
    }

    /* Allocate tracer */
    if ( v_tracer.find("dsk") != std::string::npos ) {
      return ( new psmrts::NaifDskShapeTracer ( v_shape ) );
    }
    else if ( v_tracer.find("ellipsoid") != std::string::npos ) {
      std::vector<std::string> tokens = psmrts::string_tokenizer(v_shape, ":,");
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
 * @return PSMRTS_Ray* Pointer to the resulting ray trace
 */
PSMRTS_Ray *psmrts_create_ray( const PSMRTS_Vector3d &observer, const PSMRTS_Vector3d &lookdir ) {
  return ( new PSMRTS_Ray( vector_to_eigen( observer ), vector_to_eigen( lookdir ) ) );
}



void psmrts_free_ray( PSMRTS_Ray *trace ) {
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