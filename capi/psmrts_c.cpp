
#include <Eigen/Geometry>

#include "psmrts_version.h"

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsBuffer.hpp>
#include <PsmrtsRayTrace.hpp>
#include <EllipsoidTracerModel.hpp>


/*============ PSMRTS C API type definitions ============*/
/* Must be defined before including psmrts_c.h */
typedef PSMRTSRayTrace       PsmrtsRayTrace;
typedef PSMRTSShape          PsmrtsMeshData;
typedef PSMRTSTracer         PsmrtsTracerModel;
typedef PSMRTSShapeTracer    PsmsrtsShapeTracer;
typedef PSMRTSPriorityTracer PsmsrtsPriorityTracer;


/* Include the PSMRTS C api include */
#include <psmrts_c.h>

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

void psmrts_free( PSMRTSRayTrace *trace ) {
  delete trace;
}

void psmrts_free( PSMRTSShape *shape ) {
  delete shape;
}

void psmrts_free( PSMRTSTracer *tracer ) {
  delete tracer;
}

void psmrts_free( PSMRTSShapeTracer *stracer ) {
  delete stracer;
}

void psmrts_free( PSMRTSPriorityTracer *ptracer ) {
  delete ptracer;
}

}