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


/*---> Have CMAKE generate PSMRTS CAPI versioning here! */


/*============ Type definitions ============*/
enum PSMRTSTypes {
  PSMRTS_RAYTRACE,
  PSMRTS_SHAPE,
  PSMRTS_TRACER,
  PSMRTS_SHAPE_TRACER,
  PSMRTS_PRIORITY_TRACER
};

/* Conditional types */
typedef int    PSMRTS_BOOL;
#define PSMRTS_TRUE       1
#define PSMRTS_FALSE      0


/*============ PSMRTS C API type definitions ============*/
typedef struct psmrts_raytrace        PSMRTSRayTrace;
typedef struct psmrts_shape           PSMRTSShape;
typedef struct psmrts_tracer          PSMRTSTracer;
typedef struct psmrts_shape_tracer    PSMRTSShapeTracer;
typedef struct psmrts_priority_tracer PSMRTSPriorityTracer;

/*============ PSMRTS information functions ============*/
extern const char PSMRTS_DLL *psmrts_version();
extern const char PSMRTS_DLL *psmrts_info();

/*============ PSMRTS tracing functions ============*/

extern PSMRTSShapeTracer *psmrts_load_shape( const char *shape, const char *tracer );
extern void psmrts_lonlat_to_xyz( const double longitude_d, const double latitude_d, const double radius_km, double xyz[3] );
extern void psmrts_vector_scale( const double v[3], const double scale, double vout[3] );
extern PSMRTSRayTrace *psmrts_ray_trace( PSMRTSShapeTracer *tracer, const double scpos[3], const double lookdir[3] );

extern PSMRTS_BOOL psmrts_isvalid( const PSMRTSRayTrace *trace );

/*============ PSMRTS memory functions ============*/

extern void psmrts_free( PSMRTSRayTrace *trace );
extern void psmrts_free( PSMRTSShape *shape );
extern void psmrts_free( PSMRTSTracer *tracer );
extern void psmrts_free( PSMRTSShapeTracer *stracer );
extern void psmrts_free( PSMRTSPriorityTracer *ptracer );


#ifdef __cplusplus
}
#endif

#endif // psmrts_c_h
