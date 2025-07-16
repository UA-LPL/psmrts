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
  PSMRTS_PRIORITY_TRACER,
  PSMRTS_TRACE_ARRAY,
  PSMRTS_PHOTOMETRIC_TRACE_ARRAY
};

/* Conditional types */
typedef int    PSMRTS_BOOL;
#define PSMRTS_TRUE       1
#define PSMRTS_FALSE      0

/* Define 3-vector C structure of doubles */
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

/* Define 3-vector C structure of ints */
typedef union {
  struct {
    int i;
    int j;
    int k;
  };
  int data[3];
} PSMRTS_Vector3i;

/* Facet data structure contains the elements defining a single facet */
typedef struct psmrts_facet {
  int m_has_facet;
  PSMRTS_Vector3i m_indexes;
  PSMRTS_Vector3d m_vector1;
  PSMRTS_Vector3d m_vector2;
  PSMRTS_Vector3d m_vector3;
  PSMRTS_Vector3d m_normal;
} PSMRTS_Facet;

/*============ PSMRTS C API type definitions ============*/
#if !defined( PSMRTS_POINTERS )
typedef struct psmrts_raytrace                 PSMRTS_RayTrace;
typedef struct psmrts_shape                    PSMRTS_Shape;
typedef struct psmrts_tracer                   PSMRTS_Tracer;
typedef struct psmrts_priority_tracer          PSMRTS_PriorityTracer;
typedef struct psmrts_photometric_tracer       PSMRTS_PhotometricRayTrace;

typedef struct psmrts_trace_array              PSMRTS_TraceArray;
typedef struct psmrts_photometric_trace_array  PSMRTS_PhotometricTraceArray;
#endif

/*============ PSMRTS information functions =============*/
extern const char PSMRTS_DLL *psmrts_version();
extern const char PSMRTS_DLL *psmrts_info();

/*============ PSMRTS_Vector3d functions ================*/
extern PSMRTS_Vector3d psmrts_vector3d( const double v1, const double v2,
                                        const double v3 );

extern PSMRTS_Vector3d psmrts_negate( const PSMRTS_Vector3d *v );
extern PSMRTS_Vector3d psmrts_subtract( const PSMRTS_Vector3d *v1,
                                        const PSMRTS_Vector3d *v2 );
extern PSMRTS_Vector3d psmrts_add( const PSMRTS_Vector3d *v1,
                                   const PSMRTS_Vector3d *v2 );
extern PSMRTS_Vector3d psmrts_scale( const PSMRTS_Vector3d *v,
                                     const double scale );
extern double          psmrts_length( const PSMRTS_Vector3d *v );

/*============ PSMRTS Trace functions ===================*/
extern PSMRTS_RayTrace *psmrts_create_ray( const PSMRTS_Vector3d *observer,
                                           const PSMRTS_Vector3d *lookdir );
extern PSMRTS_RayTrace *psmrts_ray_trace( PSMRTS_RayTrace *ray,
                                          const PSMRTS_Tracer *tracer );
extern PSMRTS_RayTrace *psmrts_ray_trace_v( const PSMRTS_Vector3d *observer,
                                            const PSMRTS_Vector3d *lookdir,
                                            const PSMRTS_Tracer *tracer );
extern PSMRTS_Vector3d psmrts_ray_observer( const PSMRTS_RayTrace *ray );
extern PSMRTS_Vector3d psmrts_ray_lookdir( const PSMRTS_RayTrace *ray );
extern PSMRTS_BOOL     psmrts_ray_has_hit( const PSMRTS_RayTrace *ray );
extern PSMRTS_Vector3d psmrts_ray_xyz( const PSMRTS_RayTrace *ray );
extern PSMRTS_Vector3d psmrts_ray_raypt( const PSMRTS_RayTrace *ray );
extern PSMRTS_Vector3d psmrts_ray_normal( const PSMRTS_RayTrace *ray );
extern double psmrts_ray_intercept_radius( const PSMRTS_RayTrace *ray );
extern double psmrts_ray_intercept_slant_distance( const PSMRTS_RayTrace *ray );
extern double psrmrts_ray2ray_distance( const PSMRTS_RayTrace *ray1,
                                        const PSMRTS_RayTrace *ray2 );
extern double psmrts_separation_angle_radians( const PSMRTS_Vector3d *v1,
                                               const PSMRTS_Vector3d *v2 );

extern bool psrmrts_isNear( const PSMRTS_RayTrace *ray1,
                            const PSMRTS_RayTrace *ray2,
                            const double tolerance_km ); // default argument?

extern double psmrts_incidence( const PSMRTS_RayTrace *ray1,
                                const PSMRTS_RayTrace *ray2 );
extern double psmrts_emission( const PSMRTS_RayTrace *ray );
extern double psmrts_phase( const PSMRTS_RayTrace *ray1,
                            const PSMRTS_RayTrace *ray2 );

/*============ PSMRTS TraceArray functions ==============*/
extern PSMRTS_TraceArray *psmrts_create_trace_array();
extern size_t psmrts_trace_array_size( const PSMRTS_TraceArray *tracearray );
extern size_t psmrts_trace_array_add_trace( PSMRTS_TraceArray *tracearray,
                                            const PSMRTS_RayTrace *trace );
extern const PSMRTS_RayTrace *psmrts_trace_array_get_trace( const PSMRTS_TraceArray *tracearray,
                                                            size_t index );

/*======== PSMRTS Photometric Trace functions ===========*/
extern PSMRTS_PhotometricRayTrace *psmrts_create_photometric_ray( const PSMRTS_Vector3d *observer,
                                                                  const PSMRTS_Vector3d *lookdir,
                                                                  const PSMRTS_Vector3d *sunpos);

extern double psmrts_photometric_incidence( const PSMRTS_PhotometricRayTrace *photoTrace );
extern double psmrts_photometric_emission( const PSMRTS_PhotometricRayTrace *photoTrace );
extern double psmrts_photometric_phase( const PSMRTS_PhotometricRayTrace *photoTrace1,
                                        const PSMRTS_PhotometricRayTrace *photoTrace2 );
extern const PSMRTS_RayTrace *psmrts_photometric_observer_trace( PSMRTS_PhotometricRayTrace *photoTrace );
extern const PSMRTS_RayTrace *psmrts_photometric_sun_trace( PSMRTS_PhotometricRayTrace *photoTrace);

/*====== PSMRTS Photometric TraceArray functions ========*/
extern PSMRTS_PhotometricTraceArray *psmrts_create_photometric_trace_array();
extern size_t psmrts_photometric_trace_array_size( const PSMRTS_PhotometricTraceArray *tracearray );
extern size_t psmrts_photometric_trace_array_add_trace( PSMRTS_PhotometricTraceArray *tracearray,
                                                      const PSMRTS_PhotometricRayTrace *trace );
extern const PSMRTS_PhotometricRayTrace *psmrts_photometric_trace_array_get_trace( const PSMRTS_PhotometricTraceArray *tracearray,
                                                                                   size_t index );

/* Coordinate conversion functions ======================*/
extern PSMRTS_Vector3d psmrts_lonlatrad_to_xyz( const PSMRTS_Vector3d *lonlatrad );
extern PSMRTS_Vector3d psmrts_xyz_to_lonlatrad( const PSMRTS_Vector3d *xyz );

/* Degree/radian conversion functions ===================*/
extern PSMRTS_Vector3d psmrts_radians_to_degrees( const PSMRTS_Vector3d *lonlatrad_r );
extern PSMRTS_Vector3d psmrts_degrees_to_radians( const PSMRTS_Vector3d *lonlatrad_d );

/*============ PSMRTS Tracer functions ==================*/
extern PSMRTS_Tracer *psmrts_create_sphere( const double radius_km,
                                            const char *name );
extern PSMRTS_Tracer *psmrts_create_spheroid( const double a_radius_km,
                                              const double c_radius_km,
                                              const char *name );
extern PSMRTS_Tracer *psmrts_create_ellipsoid( const double a_radius_km,
                                               const double b_radius_km,
                                               const double c_radius_km,
                                               const char *name );
extern PSMRTS_Tracer *psmrts_create_ellipsoid_v( const PSMRTS_Vector3d &radii,
                                                 const char *name );
extern PSMRTS_Tracer *psmrts_create_bullet( const char *meshfile );
extern PSMRTS_Tracer *psmrts_create_naifdsk( const char *meshfile );

extern PSMRTS_BOOL psmrts_tracer_valid( const PSMRTS_Tracer *trace );

/*============ PSMRTS memory free functions =============*/

extern void psmrts_free_ray( PSMRTS_RayTrace *trace );
extern void psmrts_free_shape( PSMRTS_Shape *shape );
extern void psmrts_free_tracer( PSMRTS_Tracer *tracer );
extern void psmrts_free_priority_tracer( PSMRTS_PriorityTracer *ptracer );
extern void psmrts_free_photometric_ray( PSMRTS_PhotometricRayTrace *ptracer );
extern void psrmts_free_trace_array( PSMRTS_TraceArray *tracearray );
extern void psrmts_free_photometric_trace_array( PSMRTS_PhotometricTraceArray *ptracearray );

#ifdef __cplusplus
}
#endif

#endif // psmrts_c_h
