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

// Define 3-vector C structure 
typedef union {
  struct {
    double x;
    double y;
    double z;
  };
  struct {
    double a;
    double b;
    double c;
  };
  struct {
    double longitude;
    double latitude;
    double radius;
  };  
  double data[3];
} PSMRTS_Vector3d;

/*============ PSMRTS C API type definitions ============*/
#if !defined( PSMRTS_POINTERS )
typedef struct psmrts_raytrace        PSMRTS_RayTrace;
typedef struct psmrts_shape           PSMRTS_Shape;
typedef struct psmrts_tracer          PSMRTS_Tracer;
typedef struct psmrts_shape_tracer    PSMRTS_ShapeTracer;
typedef struct psmrts_priority_tracer PSMRTS_PriorityTracer;
#endif

/*============ PSMRTS information functions ============*/
extern const char PSMRTS_DLL *psmrts_version();
extern const char PSMRTS_DLL *psmrts_info();

extern PSMRTS_Vector3d psmrts_vector3d( const double x, const double y, const double z );
extern PSMRTS_Vector3d psmrts_lonlatrad_d( const double longitude_d, const double latitude_d, const double radius_km );
extern PSMRTS_Vector3d psmrts_lonlatrad_r( const double longitude_r, const double latitude_r, const double radius_km );
extern PSMRTS_Vector3d psmrts_negate( const PSMRTS_Vector3d &v );
extern PSMRTS_Vector3d psmrts_subtract( const PSMRTS_Vector3d &v1, const PSMRTS_Vector3d &v2 );
extern PSMRTS_Vector3d psmrts_add( const PSMRTS_Vector3d &v1, const PSMRTS_Vector3d &v2 );
extern PSMRTS_Vector3d psmrts_scale( const PSMRTS_Vector3d &v1, const double scale );
extern double          psmrts_length( const PSMRTS_Vector3d &v1 );

/*============ PSMRTS ray functions ============*/
extern PSMRTS_RayTrace *psmrts_create_ray( const PSMRTS_Vector3d &observer, const PSMRTS_Vector3d &lookdir );
extern PSMRTS_RayTrace *psmrts_ray_trace( PSMRTS_RayTrace *ray, const PSMRTS_ShapeTracer *tracer );
extern PSMRTS_RayTrace *psmrts_ray_trace_v( const PSMRTS_Vector3d &observer,
                                            const PSMRTS_Vector3d &lookdir,
                                            const PSMRTS_ShapeTracer *tracer );


extern PSMRTS_Vector3d psmrts_ray_observer( const PSMRTS_RayTrace *ray );
extern PSMRTS_Vector3d psmrts_ray_lookdir( const PSMRTS_RayTrace *ray );
extern PSMRTS_BOOL     psmrts_ray_has_hit( const PSMRTS_RayTrace *ray );
extern PSMRTS_Vector3d psmrts_ray_xyz( const PSMRTS_RayTrace *ray );
extern PSMRTS_Vector3d psmrts_ray_normal( const PSMRTS_RayTrace *ray );
extern double psmrts_ray_intercept_radius( const PSMRTS_RayTrace *ray );
extern double psmrts_ray_intercept_slant_distance( const PSMRTS_RayTrace *ray );

/*============ PSMRTS tracing functions ============*/

// extern PSMRTS_ShapeTracer *psmrts_load_shape( const char *shape, const char *tracer );
extern PSMRTS_Vector3d psmrts_lonlatrad_to_xyz( const PSMRTS_Vector3d &lonlatrad );
extern PSMRTS_Vector3d psmrts_xyz_to_lonlatrad_r( const PSMRTS_Vector3d &xyz );
extern PSMRTS_Vector3d psmrts_radians_to_degrees( const PSMRTS_Vector3d &lonlatrad_r );
extern PSMRTS_Vector3d psmrts_degrees_to_radians( const PSMRTS_Vector3d &lonlatrad_d );

// extern PSMRTS_Ray *psmrts_ray_trace( PSMRTS_ShapeTracer *tracer, const double scpos[3], const double lookdir[3] );

extern PSMRTS_BOOL psmrts_tracer_valid( const PSMRTS_ShapeTracer *trace );

/*============ PSMRTS memory functions ============*/

extern void psmrts_free_ray( PSMRTS_RayTrace *trace );
extern void psmrts_free_shape( PSMRTS_Shape *shape );
extern void psmrts_free_tracer( PSMRTS_Tracer *tracer );
extern void psmrts_free_shapetracer( PSMRTS_ShapeTracer *stracer );
extern void psmrts_free( PSMRTS_PriorityTracer *ptracer );


#ifdef __cplusplus
}
#endif

#endif // psmrts_c_h
