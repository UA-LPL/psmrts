#pragma once 
#ifndef psmrts_c_h
#define psmrts_c_h


#ifndef __cplusplus
# include <stddef.h> /* for size_t definition */
#else
#include <cstddef>
using std::size_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIN32) || defined(_MSC_VER) || defined(__CYGWIN__)
#define PSMRTS_EXPORT
#define PSMRTS_DLL   __declspec(dllexport)
#else
#define PSMRTS_DLL
#endif


/*============ PSMRTS C API type definitions ============*/
typedef struct psmrts_mesh            PSMRTSMesh;
typedef struct psmrts_raytrace        PSMRTSRayTrace;
typedef struct psmrts_shape           PSMRTSShape;
typedef struct psmrts_tracer          PSMRTSTracer;
typedef struct psmrts_shape_tracer    PSMRTSShapeTracer;
typedef struct psmrts_priority_tracer PSMRTSPriorityTracer;

/*============ PSMRTS information functions ============*/
extern const char PSMRTS_DLL *psmrts_version();
extern const char PSMRTS_DLL *psmrts_info();


#ifdef __cplusplus
}
#endif

#endif // psmrts_c_h
