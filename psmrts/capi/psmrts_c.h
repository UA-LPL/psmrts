/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#pragma once
#ifndef psmrts_c_h
#define psmrts_c_h

#ifndef __cplusplus
# include <stddef.h> /* for size_t definition */
#else
#include <cassert>
#include <cstddef>
using std::size_t;
#endif

#include <psmrts/capi/psmrts_c_export.h>

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
  PSMRTS_BOOL     m_has_facet;
  int             m_plateid;  //! 0-based plate id/index of intercepted facet
  int             m_segment;  //! Segment (DSK)/identifier of shape source
  PSMRTS_Vector3i m_indexes;
  PSMRTS_Vector3d m_vector1;
  PSMRTS_Vector3d m_vector2;
  PSMRTS_Vector3d m_vector3;
  PSMRTS_Vector3d m_normal;
} PSMRTS_Facet;

/*============ PSMRTS C API type definitions ============*/
#if !defined( PSMRTS_POINTERS )
#define PSMRTS_POINTERS 1
typedef struct psmrts_raytrace                 PSMRTS_RayTrace;
typedef struct psmrts_shape                    PSMRTS_Shape;
typedef struct psmrts_tracer                   PSMRTS_Tracer;
typedef struct psmrts_priority_tracer          PSMRTS_PriorityTracer;
typedef struct psmrts_photometric_raytrace     PSMRTS_PhotometricRayTrace;

typedef struct psmrts_trace_array              PSMRTS_TraceArray;
typedef struct psmrts_photometric_trace_array  PSMRTS_PhotometricTraceArray;
#endif

/*============ PSMRTS information functions =============*/
PSMRTS_C_EXPORT const char PSMRTS_DLL *psmrts_version();
PSMRTS_C_EXPORT const char PSMRTS_DLL *psmrts_info();

/*============ PSMRTS_Vector3d functions ================*/
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_vector3d( const double v1, const double v2,
                                        const double v3 );

PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_negate( const PSMRTS_Vector3d *v );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_subtract( const PSMRTS_Vector3d *v1,
                                        const PSMRTS_Vector3d *v2 );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_add( const PSMRTS_Vector3d *v1,
                                   const PSMRTS_Vector3d *v2 );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_scale( const PSMRTS_Vector3d *v,
                                     const double scale );
PSMRTS_C_EXPORT double          psmrts_length( const PSMRTS_Vector3d *v );

/*============ PSMRTS_Vector3i functions ================*/
PSMRTS_C_EXPORT PSMRTS_Vector3i psmrts_vector3i( const int v1, 
                                                 const int v2,
                                                 const int v3 );

/*============ PSMRTS Trace functions ===================*/
PSMRTS_C_EXPORT PSMRTS_RayTrace *psmrts_create_ray( const PSMRTS_Vector3d *observer,
                                           const PSMRTS_Vector3d *lookdir );
PSMRTS_C_EXPORT PSMRTS_RayTrace *psmrts_ray_set_observation( const PSMRTS_Vector3d *observer,
                                                    const PSMRTS_Vector3d *lookdir,
                                                    PSMRTS_RayTrace *trace );
PSMRTS_C_EXPORT PSMRTS_RayTrace *psmrts_ray_trace( PSMRTS_RayTrace *ray,
                                          const PSMRTS_Tracer *tracer );
PSMRTS_C_EXPORT PSMRTS_RayTrace *psmrts_ray_trace_v( const PSMRTS_Vector3d *observer,
                                            const PSMRTS_Vector3d *lookdir,
                                            const PSMRTS_Tracer *tracer );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_ray_observer( const PSMRTS_RayTrace *ray );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_ray_lookdir( const PSMRTS_RayTrace *ray );
PSMRTS_C_EXPORT PSMRTS_BOOL     psmrts_ray_has_hit( const PSMRTS_RayTrace *ray );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_ray_xyz( const PSMRTS_RayTrace *ray );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_ray_raypt( const PSMRTS_RayTrace *ray );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_ray_normal( const PSMRTS_RayTrace *ray );
PSMRTS_C_EXPORT double psmrts_ray_intercept_radius( const PSMRTS_RayTrace *ray );
PSMRTS_C_EXPORT double psmrts_ray_intercept_slant_distance( const PSMRTS_RayTrace *ray );
PSMRTS_C_EXPORT double psmrts_ray2ray_distance( const PSMRTS_RayTrace *ray1,
                                       const PSMRTS_RayTrace *ray2 );
PSMRTS_C_EXPORT double psmrts_separation_angle_radians( const PSMRTS_Vector3d *v1,
                                               const PSMRTS_Vector3d *v2 );
PSMRTS_C_EXPORT PSMRTS_BOOL psmrts_isNear( const PSMRTS_RayTrace *ray1,
                                  const PSMRTS_RayTrace *ray2,
                                  const double tolerance_km );
PSMRTS_C_EXPORT double psmrts_incidence( const PSMRTS_RayTrace *ray1,
                                const PSMRTS_RayTrace *ray2 );
PSMRTS_C_EXPORT double psmrts_emission(  const PSMRTS_RayTrace *ray );
PSMRTS_C_EXPORT double psmrts_phase( const PSMRTS_RayTrace *ray1,
                            const PSMRTS_RayTrace *ray2 );

/*============ PSMRTS TraceArray functions ==============*/
PSMRTS_C_EXPORT PSMRTS_TraceArray *psmrts_create_trace_array();
PSMRTS_C_EXPORT size_t psmrts_trace_array_size( const PSMRTS_TraceArray *tracearray );
PSMRTS_C_EXPORT size_t psmrts_trace_array_add_trace( PSMRTS_TraceArray *tracearray,
                                            const PSMRTS_RayTrace *trace );
PSMRTS_C_EXPORT PSMRTS_BOOL psmrts_trace_array_trace( PSMRTS_TraceArray *tracearray,
                                             const PSMRTS_Tracer *tracer);
PSMRTS_C_EXPORT void psmrts_trace_array_clear(PSMRTS_TraceArray *tracearray);
PSMRTS_C_EXPORT const PSMRTS_RayTrace *psmrts_trace_array_get_trace( const PSMRTS_TraceArray *tracearray,
                                                            size_t index );

/*======== PSMRTS Photometric Trace functions ===========*/
PSMRTS_C_EXPORT PSMRTS_PhotometricRayTrace *psmrts_create_photometric_ray( const PSMRTS_Vector3d *observer,
                                                                  const PSMRTS_Vector3d *lookdir,
                                                                  const PSMRTS_Vector3d *sunpos);
PSMRTS_C_EXPORT PSMRTS_PhotometricRayTrace *psmrts_photometric_ray_set_observation( const PSMRTS_Vector3d *observer, // to be tested
                                                                           const PSMRTS_Vector3d *lookdir,
                                                                           const PSMRTS_Vector3d *sunpos,
                                                                           PSMRTS_PhotometricRayTrace *trace );
PSMRTS_C_EXPORT PSMRTS_PhotometricRayTrace *psmrts_photo_ray_trace( PSMRTS_PhotometricRayTrace *photoray,
                                                           const PSMRTS_Tracer *tracer );
PSMRTS_C_EXPORT double psmrts_photometric_incidence( const PSMRTS_PhotometricRayTrace *photoTrace );
PSMRTS_C_EXPORT double psmrts_photometric_emission( const PSMRTS_PhotometricRayTrace *photoTrace );
PSMRTS_C_EXPORT double psmrts_photometric_phase( const PSMRTS_PhotometricRayTrace *photoTrace );
PSMRTS_C_EXPORT const PSMRTS_RayTrace *psmrts_photometric_observer_trace( const PSMRTS_PhotometricRayTrace *photoTrace );
PSMRTS_C_EXPORT const PSMRTS_RayTrace *psmrts_photometric_sun_trace( const PSMRTS_PhotometricRayTrace *photoTrace);

/*====== PSMRTS Photometric TraceArray functions ========*/ // to be tested
PSMRTS_C_EXPORT PSMRTS_PhotometricTraceArray *psmrts_create_photometric_trace_array();
PSMRTS_C_EXPORT size_t psmrts_photometric_trace_array_size( const PSMRTS_PhotometricTraceArray *tracearray );
PSMRTS_C_EXPORT size_t psmrts_photometric_trace_array_add_trace( PSMRTS_PhotometricTraceArray *tracearray,
                                                        const PSMRTS_PhotometricRayTrace *trace );
PSMRTS_C_EXPORT PSMRTS_BOOL psmrts_photometric_trace_array_trace( PSMRTS_PhotometricTraceArray *tracearray,
                                                         const PSMRTS_Tracer *tracer);
PSMRTS_C_EXPORT void psmrts_photometric_trace_array_clear(PSMRTS_PhotometricTraceArray *tracearray);
PSMRTS_C_EXPORT const PSMRTS_PhotometricRayTrace *psmrts_photometric_trace_array_get_trace( const PSMRTS_PhotometricTraceArray *tracearray,
                                                                                   size_t index );

/* Coordinate conversion functions ======================*/
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_lonlatrad_to_xyz_d( const PSMRTS_Vector3d *lonlatrad );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_xyz_to_lonlatrad_d( const PSMRTS_Vector3d *xyz );

/* Degree/radian conversion functions ===================*/
PSMRTS_C_EXPORT double psmrts_degrees_to_radians( const double d );
PSMRTS_C_EXPORT double psmrts_radians_to_degrees( const double d );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_vector3d_to_radians( const PSMRTS_Vector3d *v );
PSMRTS_C_EXPORT PSMRTS_Vector3d psmrts_vector3d_to_degrees( const PSMRTS_Vector3d *v );

/*============ PSMRTS Tracer functions ==================*/
PSMRTS_C_EXPORT PSMRTS_Tracer *psmrts_create_sphere( const double radius_km,
                                            const char *name );
PSMRTS_C_EXPORT PSMRTS_Tracer *psmrts_create_spheroid( const double a_radius_km,
                                              const double c_radius_km,
                                              const char *name );
PSMRTS_C_EXPORT PSMRTS_Tracer *psmrts_create_ellipsoid( const double a_radius_km,
                                               const double b_radius_km,
                                               const double c_radius_km,
                                               const char *name );
PSMRTS_C_EXPORT PSMRTS_Tracer *psmrts_create_ellipsoid_v( const PSMRTS_Vector3d *radii,
                                                          const char *name );
PSMRTS_C_EXPORT PSMRTS_Tracer *psmrts_create_bullet( const char *objfile );
PSMRTS_C_EXPORT PSMRTS_Tracer *psmrts_create_naifdsk( const char *dskfile );
PSMRTS_C_EXPORT PSMRTS_BOOL psmrts_tracer_valid( const PSMRTS_Tracer *trace );

/*============ PSMRTS Facet functions ===================*/
PSMRTS_C_EXPORT PSMRTS_BOOL psmrts_get_facet( PSMRTS_RayTrace *ray, const PSMRTS_Tracer *tracer,
                                              PSMRTS_Facet *facet );
PSMRTS_C_EXPORT double psmrts_facet_surface_area( const PSMRTS_Facet *facet );
PSMRTS_C_EXPORT double psmrts_facet_volume( const PSMRTS_Facet *facet );

/*============ PSMRTS Shape (Mesh) functions ============*/
PSMRTS_C_EXPORT PSMRTS_Shape *psmrts_create_obj_shape( const char *objfile );
PSMRTS_C_EXPORT PSMRTS_Shape *psmrts_create_dsk_shape( const char *dskfile );
PSMRTS_C_EXPORT PSMRTS_Shape *psmrts_create_ply_shape( const char *plyfile );
PSMRTS_C_EXPORT double psmrts_mesh_surface_area( const PSMRTS_Shape *shape );
PSMRTS_C_EXPORT double psmrts_mesh_volume( const PSMRTS_Shape *shape );

/*============ PSMRTS memory free functions =============*/
PSMRTS_C_EXPORT void psmrts_free_ray( PSMRTS_RayTrace *trace );
PSMRTS_C_EXPORT void psmrts_free_shape( PSMRTS_Shape *shape );
PSMRTS_C_EXPORT void psmrts_free_tracer( PSMRTS_Tracer *tracer );
PSMRTS_C_EXPORT void psmrts_free_priority_tracer( PSMRTS_PriorityTracer *ptracer );
PSMRTS_C_EXPORT void psmrts_free_photometric_ray( PSMRTS_PhotometricRayTrace *ptracer );
PSMRTS_C_EXPORT void psmrts_free_trace_array( PSMRTS_TraceArray *tracearray );
PSMRTS_C_EXPORT void psmrts_free_photometric_trace_array( PSMRTS_PhotometricTraceArray *ptracearray );

#ifdef __cplusplus
}
#endif

#endif // psmrts_c_h
