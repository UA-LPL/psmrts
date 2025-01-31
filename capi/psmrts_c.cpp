
#include <Eigen/Geometry>

#include "psmrts_version.h"

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsBuffer.hpp>
#include <PsmrtsRayTrace.hpp>
#include <EllipsoidTracerModel.hpp>


/*============ PSMRTS C API type definitions ============*/
/* Must be defined before including psmrts_c.h */
typedef PSMRTSMesh           PsmrtsVector3d;
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


}