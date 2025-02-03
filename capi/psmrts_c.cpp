
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

  std::string v_shape( shape );
  std::string v_tracer( tracer );

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
      /* Create ellipsoid with 1,2 or 3 radii..*/
    }
    else {
      psmrts::bullet::PsmrtsBulletWorldModel bt_world( mesh, v_shape );
      return ( new psmrts::BulletShapeTracer( bt_world ) );
    }
  }
  catch (...) {
    /* NOOP failure */
  }


  return ( NULL );
}

void psmrts_free( PSMRTSRayTrace *trace ) {
  delete trace;
}

}