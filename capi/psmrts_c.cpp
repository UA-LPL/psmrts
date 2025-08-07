/** @file psmrts_c.cpp */

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
#include <PsmrtsTracer.hpp> 


/*============ PSMRTS C API type definitions ============*/
/* Must be defined before including psmrts_c.h */
#define PSMRTS_POINTERS 1
using PSMRTS_RayTrace              = psmrts::PRQRayTrace;
using PSMRTS_Shape                 = psmrts::PsmrtsMeshData; // will be psmrts::PsmrtsShape
using PSMRTS_Tracer                = psmrts::PsmrtsTracer; 
using PSMRTS_PriorityTracer        = psmrts::PsmrtsPriorityTracer;
using PSMRTS_PhotometricRayTrace   = psmrts::PRQPhotometricTrace;
using PSMRTS_TraceArray            = psmrts::PRQRayTraceArray;
using PSMRTS_PhotometricTraceArray = psmrts::PRQPhotometricTraceArray;

/* Include the PSMRTS C api include */
#include "psmrts_c.h"

/**
 * @brief vector_to_eigen - Converts a PSMRTS_Vector3d to an Eigen Vector3d.
 *
 * This function converts an input PSMRTS_Vector3d to an Eigen Vector3d..
 *
 * @param v3d PSMRTS_Vector3d.
 * @return Eigen::Vector3d converted from PSMRTS_Vector3d.
 */
inline Eigen::Vector3d vector_to_eigen( const PSMRTS_Vector3d &v3d ) {
  return ( Eigen::Vector3d( { v3d.a, v3d.b, v3d.c } ) );
}

/**
 * @brief eigen_to_vector - Converts an Eigen Vector3d to a PSMRTS_Vector3d.
 *
 * This function converts an input PSMRTS_Vector3d to an Eigen Vector3d.
 *
 * @param v Eigen::Vector3d
 * @return PSMRTS_Vector3d converted from Eigen::Vector3d.
 */
inline PSMRTS_Vector3d eigen_to_vector( const Eigen::Vector3d &v ) {
  PSMRTS_Vector3d v3d = { v[0], v[1], v[2] };
  return ( v3d );
}

/**
 * @brief evaluate - Evaluate input bool type and return PSMRTS_TRUE or
 *                   PSMRTS_FALSE
 *
 * This function evaluates the given bool type and returns PSMRTS_TRUE or
 * PSMRTS_FALSE.
 *
 * @param b bool orginating from C++ to agnosting PSMRTS type
 * @return Values of input PSMRTS_BOOL namely, PSMRTS_TRUE or PSMRTS_FALSE.
 */
inline PSMRTS_BOOL evaluate( const bool b ) {
  return ( b ? PSMRTS_TRUE : PSMRTS_FALSE );
}

extern "C" {

/*============ PSMRTS information functions ============*/

/**
 * @brief psmrts_version - Returns a string with the PSMRTS version.
 *
 * This function returns a string with the PSMRTS version.
 *
 * @return char* String with PSMRTS version.
 */
const char *psmrts_version() {
  return ( PROJECT_VERSION );
}

/**
 * @brief psmrts_info - Returns a string with the PSMRTS version.
 *
 * This function returns a string with the PSMRTS version.
 *
 * @return char* String with PSMRTS version.
 */
const char *psmrts_info() {
  return ( PSMRTS_VERSION );
}

/**
 * @brief psmrts_vector3d - Creates a PSMRTS 3d vector object from input
 *                          coordinates.
 *
 * Given input coordinates, this function creates and returns a PSMRTS_Vector3d
 * object. The coordinates could be ...
 *  1. xyz
 *  2. longitude (degrees), latitude (degrees), radius (km)
 *  3. longitude (radians), latitude (radians), radius (km)
 *
 * @param v1 coordinate 1.
 * @param v2 coordinate 2.
 * @param v3 coordinate 3.
 * @return PSMRTS_Vector3d Vector with input coordinates.
 */
PSMRTS_Vector3d psmrts_vector3d( const double v1,
                                 const double v2,
                                 const double v3 ) {

  PSMRTS_Vector3d v3d = { v1, v2, v3 };
  return ( v3d );
}

/**
 * @brief psmrts_negate - Creates a negated (or flipped) version of a 3d vector.
 *
 * Given an input PSMRTS_Vector3d, this function creates and returns a negated
 * (i.e. flipped) version of that vector.
 *
 * @param v Pointer to PSMRTS_Vector3d.
 * @return PSMRTS_Vector3d with negated coordinates.
 */
PSMRTS_Vector3d psmrts_negate( const PSMRTS_Vector3d *v ) {

  PSMRTS_Vector3d v3d;
  v3d.x = -v->x;
  v3d.y = -v->y;
  v3d.z = -v->z;

  return ( v3d );
}

/**
 * @brief psmrts_subtract - Subtracts two vectors. Result is in a new vector.
 *
 * Given two PSMRTS_Vector3d objects, this function subtracts one from the other
 * and returns the result in a new vector.
 *
 * @param v1 1st PSMRTS_Vector3d.
 * @param v2 2nd PSMRTS_Vector3d.
 * @return PSMRTS_Vector3d with difference of 1st and 2nd input vectors.
 */
PSMRTS_Vector3d psmrts_subtract( const PSMRTS_Vector3d *v1,
                                 const PSMRTS_Vector3d *v2 ) {
  PSMRTS_Vector3d v3d;

  v3d.x = v1->x - v2->x;
  v3d.y = v1->y - v2->y;
  v3d.z = v1->z - v2->z;

  return ( v3d );
}

/**
 * @brief psmrts_add - Adds two vectors. Result is in a new vector.
 *
 * Given two PSMRTS_Vector3d objects, this function adds them and returns the
 * the result in a new vector.
 *
 * @param v1 1st PSMRTS_Vector3d.
 * @param v2 2nd PSMRTS_Vector3d.
 * @return PSMRTS_Vector3d with sum of 1st and 2nd input vectors.
 */
PSMRTS_Vector3d psmrts_add( const PSMRTS_Vector3d *v1,
                            const PSMRTS_Vector3d *v2 ) {
    PSMRTS_Vector3d v3d;

    v3d.x = v1->x + v2->x;
    v3d.y = v1->y + v2->y;
    v3d.z = v1->z + v2->z;

    return ( v3d );
}

/**
 * @brief psmrts_scale - Uses given scale factor to scale vector.
 *
 * Given a PSMRTS_Vector3d and a scale factor, this function scales the vector.
 *
 * @param v PSMRTS_Vector3d.
 * @param scale scale factor.
 * @return PSMRTS_Vector3d Scaled vector.
 */
PSMRTS_Vector3d psmrts_scale( const PSMRTS_Vector3d *v,
                              const double scale ) {
  PSMRTS_Vector3d v3d;

  v3d.x = scale * v->x;
  v3d.y = scale * v->y;
  v3d.z = scale * v->z;

  return ( v3d );
}

/**
 * @brief psmrts_length - Returns length of given vector.
 *
 * Given a PSMRTS_Vector3d, this function returns its length.
 *
 * @param v PSMRTS_Vector3d.
 * @return double Vector length.
 */
double psmrts_length( const PSMRTS_Vector3d *v ) {

  double x2 = v->x * v->x;
  double y2 = v->y * v->y;
  double z2 = v->z * v->z;

  double length  = std::sqrt( x2 + y2 + z2 );

  return ( length );
}

/**
 * @brief psmrts_create_ray - Creates a PSMRTS ray trace object
 * 
 * Given vectors describing observer position and look direction, this
 * function creates a PSMRTS ray trace object that can be used to trace
 * on a shape. 
 * 
 * The observer position is a vector from the shape origin to its body-fixed
 * position relative to the target body origin.
 *
 * The lookdir is converted to a unit vector and originates from the observer
 * location toward the target body.
 * 
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param observer Vector defining observer position (km); i.e. ray origin.
 * @param lookdir  Vector defining direction (km) from observer to trace.
 * @return Resulting PSMRTS_RayTrace object.
 */
PSMRTS_RayTrace *psmrts_create_ray( const PSMRTS_Vector3d *observer,
                                    const PSMRTS_Vector3d *lookdir ) {

  return ( new PSMRTS_RayTrace( vector_to_eigen( *observer ),
                                vector_to_eigen( *lookdir ) ) );
}

/**
 * @brief psmrts_ray_set_observation - Resets an existing trace with input
 *                                     observer and look direction vectors.
 *
 * Given vectors describing observer position and look direction, this
 * function resets the input PSMRTS_RayTrace object.
 *
 * The observer position is a vector from the shape origin to its body-fixed
 * position relative to the target body origin.
 *
 * The lookdir is converted to a unit vector and originates from the observer
 * location toward the target body.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param observer Observer position vector (km); i.e. ray origin.
 * @param lookdir  Look direction vector (km) from observer to trace.
 * @return PSMRTS_RayTrace with updated observer and look direction vectors.
 */
PSMRTS_RayTrace *psmrts_ray_set_observation( const PSMRTS_Vector3d *observer,
                                             const PSMRTS_Vector3d *lookdir,
                                             PSMRTS_RayTrace *trace ) {

  assert( trace != nullptr && "psmrts_ray_trace::PSMRTS_RayTrace is null" );

  *trace = PSMRTS_RayTrace( vector_to_eigen( *observer ),
                            vector_to_eigen( *lookdir ) );

  return ( trace );
}

/**
 * @brief psmrts_ray_trace - Runs a trace on ray and updates that ray with the
 * trace results.
 *
 * Given PSMRTS_Tracer and PSMRTS_RayTrace objects, the tracer is used to run a
 * trace on the ray. The same ray is returned, containing the trace results.
 *
 * The input ray is required to have a valid observer and look direction. It is
 * the responsibility of the caller to check for valid pointer return.
 *
 * @param ray PSMRTS_RayTrace object.
 * @param tracer PSMRTS_Tracer object.
 * @return PSMRTS_RayTrace* Same ray as input, with content updated by the trace.
 */
PSMRTS_RayTrace *psmrts_ray_trace( PSMRTS_RayTrace *ray,
                                   const PSMRTS_Tracer *tracer ) {

  assert( ray != nullptr && "psmrts_ray_trace::PSMRTS_RayTrace is null" );
  tracer->process( *ray );

  return ( ray );
}

/**
 * @brief psmrts_ray_trace_v - Creates a PSMRTS ray trace object.
 *
 * Given a tracer plus observer position and look direction vectors, this
 * function creates a PSMRTS ray trace object that can be used to trace on a
 * shape.
 *
 * The observer position is a vector from the shape origin to its body-fixed
 * position relative to the target body origin.
 *
 * The lookdir is converted to unit vector and eminates from the observer
 * location toward the target body.
 *
 * It is the responsibility of the caller to check for valid pointer return. It
 * is also the responsibility of the caller to free the pointer returned by this
 * function.
 *
 * @param observer Pointer to 3d vector defining observer position (km), i.e.
 *                 the ray origin.
 * @param lookdir Pointer to 3d vector defining direction (km) vector from
 *                observer to trace.
 * @param tracer Pointer to PSMRTS_Tracer object.
 * @return Pointer to the resulting PSMRTS_RayTrace object.
 */
PSMRTS_RayTrace *psmrts_ray_trace_v( const PSMRTS_Vector3d *observer,
                                     const PSMRTS_Vector3d *lookdir,
                                     const PSMRTS_Tracer *tracer ) {

  return ( psmrts_ray_trace( psmrts_create_ray( observer, lookdir ),
                             tracer ) );
}

/**
 * @brief psmrts_ray_observer - Returns observer position vector for input
 *                              ray trace.
 *
 * Given n PSMRTS_RayTrace object, this function returns its associated
 * observer position vector.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @return PSMRTS_Vector3d defining observer position associated with input ray.
 */
PSMRTS_Vector3d psmrts_ray_observer( const PSMRTS_RayTrace *ray ) {
  return ( eigen_to_vector( ray->trace().observer() ) );
}

/**
 * @brief psmrts_ray_lookdir - Returns look direction vector for input
 *        ray trace.
 *
 * Given an PSMRTS_RayTrace object, this function returns its associated
 * look direction vector.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @return PSMRTS_Vector3d defining look direction associated with input ray.
 */
PSMRTS_Vector3d psmrts_ray_lookdir( const PSMRTS_RayTrace *ray ) {
  return ( eigen_to_vector( ray->trace().lookdir() ) );
}

/**
 * @brief psmrts_ray_has_hit - Returns true/false indicating if ray
 *        intercepts surface.
 *
 * Given an PSMRTS_RayTrace object, this function returns true/false
 * indicating if the ray intercepts a surface.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @return PSMRTS_BOOL indicating if input ray intercepts the surface.
 */
PSMRTS_BOOL psmrts_ray_has_hit( const PSMRTS_RayTrace *ray ) {
  return ( evaluate( ray->trace().hasHit() ) );
}

/**
 * @brief psmrts_ray_xyz - Returns surface intercept point relative to
 *        the body origin.
 *
 * Given an PSMRTS_RayTrace object, this function returns a vector
 * of the rays surface intercept point relative to the body origin.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @return PSMRTS_Vector3d Vector defining ray surface intercept relative to
 *                         body origin.
 */
PSMRTS_Vector3d psmrts_ray_xyz( const PSMRTS_RayTrace *ray ) {
  return ( eigen_to_vector( ray->trace().xyz() ) );
}

/**
 * @brief psmrts_ray_raypt - Returns vector along look direction to
 *        surface.
 *
 * Given an PSMRTS_RayTrace object, this function returns a vector
 * along the look direction to the surface.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @return PSMRTS_Vector3d Vector along the ray look direction to the surface.
 */
PSMRTS_Vector3d psmrts_ray_raypt( const PSMRTS_RayTrace *ray ) {
  return ( eigen_to_vector( ray->trace().raypt() ) );
}

/**
 * @brief psmrts_ray_normal - Returns normal vector at the surface
 *        intercept, if the intercept exists.
 *
 * Given an PSMRTS_RayTrace object, this function returns the normal
 * vector at the surface, if the intercept exists.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @return PSMRTS_Vector3d Normal vector at surface intercept, if it exists.
 */
PSMRTS_Vector3d psmrts_ray_normal( const PSMRTS_RayTrace *ray ) {
  return ( eigen_to_vector( ray->trace().normal() ) );
}

/**
 * @brief psmrts_ray_intercept_radius - Returns the radius at the surface
 *        intercept point, if it exists.
 *
 * Given an PSMRTS_RayTrace object, this function returns the radius
 * at the surface intercept point, if it exists.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @return double Radius at surface intercept, if it exists.
 */
double psmrts_ray_intercept_radius( const PSMRTS_RayTrace *ray ) {
  return ( ray->trace().radius() );
}

/**
 * @brief psmrts_ray_intercept_slant_distance - Returns slant distance
 *        at the ray surface intercept point, if it exists.
 *
 * Given an PSMRTS_RayTrace object, this function returns the slant
 * distance at the surface intercept point, if it exists. The slant
 * distance is defined from the observer to the surface intercept
 * point, if it exists.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @return double Slant distance at surface intercept, if it exists.
 */
double psmrts_ray_intercept_slant_distance( const PSMRTS_RayTrace *ray ) {
  return ( ray->trace().slant_distance() );
}

/**
 * @brief psmrts_ray2ray_distance - Returns the distance between two
 *        ray trace surface intercepts, if they exist.
 *
 * Given two PSMRTS_RayTrace objects, this function returns the distance
 * between their surface intercept points, if they exists.
 *
 * @param ray1 Pointer to 1st PSMRTS_RayTrace object.
 * @param ray2 Pointer to 2nd PSMRTS_RayTrace object.
 * @return double Distance between input ray surface intercepts, if they exist.
 */
double psmrts_ray2ray_distance( const PSMRTS_RayTrace *ray1,
                                const PSMRTS_RayTrace *ray2 ) {

  return ( ray1->trace().distance( ray2->trace() ) );
}

/**
 * @brief psmrts_separation_angle_radians - Returns the angle between two
 *        vectors, in radians.
 *
 * Given two PSMRTS_Vector3d objects, this function returns the angle
 * between them, in radians.
 *
 * @param v1 Pointer to 1st PSMRTS_Vector3d object.
 * @param v2 Pointer to 2nd PSMRTS_Vector3d object.
 * @return double Angle between two vectors, in radians.
 */
double psmrts_separation_angle_radians( const PSMRTS_Vector3d *v1,
                                        const PSMRTS_Vector3d *v2 ) {

  return ( psmrts::PsmrtsRayTrace::separation_angle( vector_to_eigen( *v1 ),
                                                     vector_to_eigen( *v2 ) ) );
}

/**
 * @brief psmrts_isNear - Returns true/false if the distance between
 *        two ray surface intercepts is/isn't within a given
 *        tolerance.
 *
 * Given two PSMRTS_RayTrace objects and a tolerance, this function
 * returns "true" if the distance between their surface intercepts is
 * within the tolerance and "false" otherwise.
 *
 * @param ray1 Pointer to 1st PSMRTS_RayTrace object.
 * @param ray2 Pointer to 2nd PSMRTS_RayTrace object.
 * @param tolerance_km Distance tolerance in km.
 * @return PSMRTS_BOOL True/false if distance is/isn't within given tolerance.
 */
PSMRTS_BOOL psmrts_isNear( const PSMRTS_RayTrace *ray1,
                           const PSMRTS_RayTrace *ray2,
                           const double tolerance_km ) {

  return ( evaluate( ray1->trace().isNear( ray2->trace(), tolerance_km ) ) );
}

/**
 * @brief psmrts_incidence - Returns the incidence angle (in radians) between
 *        two traces.
 *
 * Given two PSMRTS_RayTrace objects, this function returns the incidence
 * angle (in radians) between them.
 *
 * @param ray1 Pointer to 1st PSMRTS_RayTrace object.
 * @param ray2 Pointer to 2nd PSMRTS_RayTrace object.
 * @return double incidence angle between rays, in radians.
 */
double psmrts_incidence( const PSMRTS_RayTrace *ray1,
                         const PSMRTS_RayTrace *ray2 ) {

  return ( ray1->trace().incidence( ray2->trace() ) );
}

/**
 * @brief psmrts_emission - Returns the emission angle (in radians) between the
 *        normal vector at a surface point and a vector from that surface point
 *        to an observer.
 *
 * Given a PSMRTS_RayTrace object, this function returns the emission angle
 * (in radians) between the normal vector at a surface point and a vector from
 * that surface point to an observer.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @return double Emission angle of input ray, in radians.
 */
double psmrts_emission( const PSMRTS_RayTrace *ray ) {
  return ( ray->trace().emission() );
}

/**
 * @brief psmrts_phase - Returns the phase angle (in radians) subtended between
 *        two vectors from a common surface point to two different positions.
 *
 * Given two PSMRTS_RayTrace objects, this function returns the phase angle (in
 * radians) subtended between two vectors from a common surface point to two
 * different positions.
 *
 * @param ray1 Pointer to 1st PSMRTS_RayTrace object.
 * @param ray2 Pointer to 2nd PSMRTS_RayTrace object.
 * @return double Phase angle between input rays, in radians.
 */
double psmrts_phase( const PSMRTS_RayTrace *ray1,
                     const PSMRTS_RayTrace *ray2 ) {

  return ( ray1->trace().phase( ray2->trace() ) );
}

/**
 * @brief psmrts_create_trace_array - Constructs PSMRTS_TraceArray.
 *
 * This function constructs a PSMRTS_TraceArray.
 *
 * @return Pointer to PSMRTS_TraceArray.
 */
PSMRTS_TraceArray *psmrts_create_trace_array() {
  return ( new PSMRTS_TraceArray() );
}

/**
 * @brief psmrts_trace_array_size - Returns PSMRTS_TraceArray size.
 *
 * Given a PSMRTS_TraceArray object, this function returns its' size.
 *
 * @param tracearray Pointer to PSMRTS_TraceArray object.
 * @return size_t Number of traces in array.
 */
size_t psmrts_trace_array_size( const PSMRTS_TraceArray *tracearray ) {
  return ( tracearray->size() );
}

/**
 * @brief psmrts_trace_array_add_trace - Adds trace to trace array.
 *
 * This function adds a PSMRTS_RayTrace to a given PSMRTS_TraceArray object.
 *
 * @param tracearray Pointer to PSMRTS_TraceArray.
 * @param trace Pointer to PSMRTS_RayTrace to be added.
 * @return size_t Index of newly added trace.
 */
size_t psmrts_trace_array_add_trace( PSMRTS_TraceArray *tracearray,
                                     const PSMRTS_RayTrace *trace ) {    
  return ( tracearray->add_trace( *trace ) );
}

/**
 * @brief psmrts_trace_array_clear - Clears trace array.
 *
 * This function removes all traces from the given PSMRTS_TraceArray.
 *
 * @param tracearray Pointer to PSMRTS_TraceArray.
 * @return void
 */
void psmrts_trace_array_clear(PSMRTS_TraceArray *tracearray) {
  tracearray->clear();

  // now
}

/**
 * @brief psmrts_trace_array_get_trace - Get trace at given index from trace
 *                                       array.
 *
 * This function retrieves a PSMRTS_RayTrace at a given index.
 *
 * NOTE: index is zero-based, i.e. ranging from 0 to 'n-1' where 'n' is the
 *       number of elements in the trace array.
 *
 * @param tracearray Pointer to PSMRTS_TraceArray.
 * @param index Integer array index for requested PSMRTS_RayTrace.
 * @return const PSMRTS_RayTrace Pointer to PSMRTS_RayTrace object at index.
 */
const PSMRTS_RayTrace *psmrts_trace_array_get_trace( const PSMRTS_TraceArray *tracearray,
                                                     size_t index ) {
  try {
    return ( &tracearray->get_trace( index ) );
  }
  catch( const std::exception &e ) {
    tracearray->add_error(e);
    //  return nullptr;
  }
  return nullptr;
}

/* Photometric Tracer methods */

/**
 * @brief psmrts_create_photometric_ray - Creates a PSMRTS photometric ray
 *        trace object.
 *
 * Given vectors describing observer position and look direction, this function
 * creates a PSMRTS photometric ray trace object that can be used to trace on a
 * shape.
 *
 * Observer position is a vector from the shape origin to its body-fixed position relative to the target body origin.
 *
 * the lookdir is converted to a unit vector and originates from the observer
 * location toward the target body.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param observer Vector defining observer position (km), i.e. the ray origin.
 * @param lookdir  Vector defining direction (km) vector from observer to trace.
 * @param sunpos   Vector defining sun direction (km).
 * @return PSMRTS_PhotometricRayTrace Pointer to resulting photometric ray
 *                                    trace.
 */
PSMRTS_PhotometricRayTrace *psmrts_create_photometric_ray( const PSMRTS_Vector3d *observer,
                                                           const PSMRTS_Vector3d *lookdir,
                                                           const PSMRTS_Vector3d *sunpos) {

  return ( new psmrts::PRQPhotometricTrace( vector_to_eigen( *observer ),
                                            vector_to_eigen( *lookdir ),
                                            vector_to_eigen( *sunpos ) ) );
}

/**
 * @brief psmrts_photometric_ray_set_observation - Resets an existing photometric
 *        trace with input observer, look direction and sun position vectors.
 *
 * Given vectors describing observer position, look direction, and sun position,
 * this function resets the input PSMRTS_PhotometricRayTrace object.
 *
 * The observer position is a vector from the shape origin to its body-fixed
 * position relative to the target body origin.
 *
 * The lookdir is converted to a unit vector and originates from the observer
 * location toward the target body.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param observer Observer position vector (km); i.e. ray origin.
 * @param lookdir  Look direction vector (km) from observer to trace.
 * @param sunpos  Sun position vector (km).
 * @return PSMRTS_PhotometricRayTrace with updated observer, look direction, and
 *         sun position vectors.
 */
PSMRTS_PhotometricRayTrace *psmrts_photometric_ray_set_observation( const PSMRTS_Vector3d *observer,
                                                                    const PSMRTS_Vector3d *lookdir,
                                                                    const PSMRTS_Vector3d *sunpos,
                                                                    PSMRTS_PhotometricRayTrace *phototrace ) {

  assert( phototrace != nullptr && "psmrts_ray_trace::PSMRTS_PhotometricRayTrace is null" );

  *phototrace = PSMRTS_PhotometricRayTrace( vector_to_eigen( *observer ),
                                            vector_to_eigen( *lookdir ),
                                            vector_to_eigen( *sunpos ) );

  return ( phototrace );
}

/**
 * @brief psmrts_photo_ray_trace - Runs a photometric trace on PSMRTS_PhotometricRayTrace
 * and updates that raytrace with the results.
 *
 * Given PSMRTS_Tracer and PSMRTS_PhotometricRayTrace objects, the tracer is used to run a
 * trace on the ray. The same ray is returned, containing the trace results.
 *
 * The input ray is required to have a valid observer and look direction. It is
 * the responsibility of the caller to check for valid pointer return.
 *
 * @param photoray PSMRTS_PhotometricRayTrace object.
 * @param tracer PSMRTS_Tracer object.
 * @return PSMRTS_PhotometricRayTrace Same ray as input, with content updated by the trace.
 */
PSMRTS_PhotometricRayTrace *psmrts_photo_ray_trace( PSMRTS_PhotometricRayTrace *photoray,
                                                    const PSMRTS_Tracer *tracer ) {

    assert( photoray != nullptr && "psmrts_photo_ray_trace::PSMRTS_PhotometricRayTrace is null" );
    tracer->process( *photoray );

    return ( photoray );
}

/**
 * @brief psmrts_photometric_incidence - Returns the photometric incidence angle
 *        (in radians) between the normal at a surface point and the vector from
 *        that surface point to the Sun.
 *
 * Given a PSMRTS_PhotometricRayTrace object, this function returns the
 * photometric incidence angle (in radians) between the normal at a surface
 * point and the vector from that surface point to the Sun.
 *
 * @param photometricTrace Pointer to PSMRTS_PhotometricRayTrace object.
 * @return double Photometric incidence angle of input photometric ray trace (in
 *                radians).
 */
double psmrts_photometric_incidence( const PSMRTS_PhotometricRayTrace *photometricTrace ) {
  return ( photometricTrace->incidence() );
}

/**
 * @brief psmrts_photometric_emission - Returns the photometric emission angle
 *        (in radians) between the normal vector at a surface point and a
 *        vector from that surface point to an observer.
 *
 * Given a PSMRTS_PhotometricRayTrace object, this function returns the
 * photometric emission angle between the normal vector at a surface point and
 * a vector from that surface point to an observer.
 *
 * @param photometricTrace Pointer to PSMRTS_PhotometricRayTrace* object.
 * @return double Photometric emission angle of input photometric ray trace (in
 *                radians).
 */
double psmrts_photometric_emission( const PSMRTS_PhotometricRayTrace *photometricTrace ) {
  return ( photometricTrace->emission() );
}

/**
 * @brief psmrts_photometric_phase - Returns the photometric phase angle (in
 *        radians) subtended between two vectors that define observer and sun
 *        position.
 *
 * Given a PSMRTS_PhotometricRayTrace object, this function returns the
 * photometric phase angle (in radians) subtended between two vectors that
 * define observer and sun position.
 *
 * @param photoTrace Pointer to PSMRTS_PhotometricRayTrace object.
 * @return double Photometric phase angle between observer and sun position (in
 *                radians).
 */
double psmrts_photometric_phase( const PSMRTS_PhotometricRayTrace *photoTrace ) {
  return ( photoTrace->phase() );
}

/**
 * @brief psmrts_photometric_observer_trace - Returns the photometric observer
 *        PSMRTS_RayTrace.
 *
 * Given a PSMRTS_PhotometricRayTrace, this function returns the contained
 * PSMRTS_RayTrace observer trace.
 *
 * @param photoTrace Pointer to PSMRTS_PhotometricRayTrace object.
 * @return PSMRTS_RayTrace const pointer to PSMRTS_RayTrace observer trace.
 */
const PSMRTS_RayTrace *psmrts_photometric_observer_trace( const PSMRTS_PhotometricRayTrace *photoTrace ) {
  return ( &photoTrace->observer() );
}

/**
 * @brief psmrts_photometric_sun_trace - Returns the photometric sun trace.
 *
 * Given a PSMRTS_PhotometricRayTrace object, this function returns the
 * photometric sun trace.
 *
 * @param photoTrace Pointer to PSMRTS_PhotometricRayTrace object.
 * @return PSMRTS_RayTrace const pointer to PSMRTS_RayTrace sun trace.
 */
const PSMRTS_RayTrace *psmrts_photometric_sun_trace( const PSMRTS_PhotometricRayTrace *photoTrace ) {
  return ( &photoTrace->sunpos() );
}

/**
 * @brief psmrts_create_photometric_trace_array - Constructs
 *                                                PSMRTS_PhotometricTraceArray.
 *
 * This function constructs a PSMRTS_PhotometricTraceArray.
 *
 * @return Pointer to PSMRTS_PhotometricTraceArray.
 */
PSMRTS_PhotometricTraceArray *psmrts_create_photometric_trace_array() {
  return ( new PSMRTS_PhotometricTraceArray() );
}

/**
 * @brief psmrts_photometric_trace_array_size - Returns size of
 *                                              PSMRTS_PhotometricTraceArray.
 *
 * Given a PSMRTS_PhotometricTraceArray object, this function returns its' size.
 *
 * @param tracearray Pointer to PSMRTS_PhotometricTraceArray object.
 * @return size_t Number of traces in array.
 */
size_t psmrts_photometric_trace_array_size( const PSMRTS_PhotometricTraceArray *tracearray ) {
  return ( tracearray->size() );
}

/**
 * @brief psmrts_photometric_trace_array_add_trace - Adds trace to trace array.
 *
 * This function adds a PSMRTS_PhotometricRayTrace to a given PSMRTS_TraceArray
 * object.
 *
 * @param tracearray Pointer to PSMRTS_PhotometricTraceArray.
 * @param trace Pointer to PSMRTS_PhotometricRayTrace to be added.
 * @return size_t Index of newly added trace.
 */
size_t psmrts_photometric_trace_array_add_trace( PSMRTS_PhotometricTraceArray *tracearray,
                                                 const PSMRTS_PhotometricRayTrace *trace ) {
  return ( tracearray->add_trace( *trace ) );
}

/**
 * @brief psmrts_photometric_trace_array_clear - Clears photometric trace array.
 *
 * This function removes all traces from the given PSMRTS_PhotometricTraceArray.
 *
 * @param tracearray Pointer to PSMRTS_PhotometricTraceArray.
 * @return void
 */
void psmrts_photometric_trace_array_clear(PSMRTS_PhotometricTraceArray *tracearray) {
  tracearray->clear();
}

/**
 * @brief psmrts_photometric_trace_array_get_trace - Get trace at given index
 *                                                   from trace array.
 *
 * This function retrieves a PSMRTS_PhotometricRayTrace at the given index.
 *
 * @param tracearray Pointer to PSMRTS_PhotometricTraceArray.
 * @param index Integer array index for requested PSMRTS_PhotometricRayTrace.
 * @return const PSMRTS_PhotometricRayTrace Pointer ƒ√at index.
 */
const PSMRTS_PhotometricRayTrace *psmrts_photometric_trace_array_get_trace( const PSMRTS_PhotometricTraceArray *tracearray,
                                                                            size_t index ) {
  try {
    return ( &tracearray->get_trace( index ) );
  }
  catch( const std::exception &e ) {
    tracearray->add_error(e);
  //  return nullptr;
  }
  return nullptr;
}

/**
 * @brief psmrts_lonlatrad_to_xyz - Converts vector in longitude, latitude,
 *        radius coordinates to xyz.
 *
 * Given an PSMRTS_Vector3d in longitude, latitude, radius coordinates, this
 * function converts it to xyz coordinates. Input angular coordinates are
 * assumed to be in degrees.
 *
 * @param v Pointer to PSMRTS_Vector3d in lon, lat, radius coordinates.
 * @return PSMRTS_Vector3d Vector converted to xyz coordinates.
 */
PSMRTS_Vector3d psmrts_lonlatrad_to_xyz_d( const PSMRTS_Vector3d *v ) {

  return ( eigen_to_vector( psmrts::lonlatrad_to_xyz_d( vector_to_eigen(*v) ) ) );
}

/**
 * @brief psmrts_xyz_to_lonlatrad - Converts vector in xyz coordinates to
 *        longitude, latitude, radius coordinates.
 *
 * Given an PSMRTS_Vector3d in xyz coordinates, this function converts it to
 * longitude, latitude, radius coordinates. Output angular coordinates are in
 * degrees.
 *
 * @param v Pointer to PSMRTS_Vector3d in xyz coordinates.
 * @return PSMRTS_Vector3d Vector converted to lon, lat, radius coordinates.
 */
PSMRTS_Vector3d psmrts_xyz_to_lonlatrad_d( const PSMRTS_Vector3d *v ) {

  return ( eigen_to_vector( psmrts::xyz_to_lonlatrad_d( vector_to_eigen( *v ) ) ) );
}

/**
 * @brief psmrts_degrees_to_radians - Convert number from degrees to radians.
 *
 * Given a double in degrees, this function converts it to radians.
 *
 * @param d double
 * @return double Input converted to radians.
 */
double psmrts_degrees_to_radians( const double d ) {
  return ( psmrts::degrees_to_radians( d ) );
}

/**
 * @brief psmrts_radians_to_degrees - Convert number from radians to degrees.
 *
 * Given a double in radians, this function converts it to degrees.
 *
 * @param d double
 * @return double Input converted to degrees.
 */
double psmrts_radians_to_degrees( const double d ) {
  return ( psmrts::radians_to_degrees( d ) );
}

/**
 * @brief psmrts_vector3d_to_radians - Convert vector longitude/latitude
 *                                     coordinates from degrees to radians.
 *
 * Given a PSMRTS_Vector3d with longitude/latitude in degrees, this function
 * converts them to radians.
 *
 * @param v PSMRTS_Vector3d
 * @return PSMRTS_Vector3d Vector with longitude/latitude converted to radians.
 */
PSMRTS_Vector3d psmrts_vector3d_to_radians( const PSMRTS_Vector3d *v ) {
    PSMRTS_Vector3d newvec;

    newvec.longitude = psmrts_degrees_to_radians( v->longitude );
    newvec.latitude  = psmrts_degrees_to_radians( v->latitude );
    newvec.radius    = v->radius;

    return ( newvec );
}

/**
 * @brief psmrts_vector3d_to_degrees - Convert vector longitude/latitude
 *                                     coordinates from radians to degrees.
 *
 * Given a PSMRTS_Vector3d with longitude/latitude in radians, this function
 * converts them to degrees.
 *
 * @param v PSMRTS_Vector3d
 * @return PSMRTS_Vector3d Vector with longitude/latitude converted to degrees.
 */
PSMRTS_Vector3d psmrts_vector3d_to_degrees( const PSMRTS_Vector3d *v ) {    
    PSMRTS_Vector3d newvec;

    newvec.longitude = psmrts_radians_to_degrees( v->longitude );
    newvec.latitude  = psmrts_radians_to_degrees( v->latitude );
    newvec.radius    = v->radius;

    return ( newvec );
}

/**
 * @brief psmrts_create_sphere - Creates a PSMRTS sphere tracer
 *
 * Given a radius value (km) and an optional name, this function creates a
 * PSMRTS tracer object that can be used to trace on a sphere.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param radius_km Double, sphere radius in km.
 * @param name  Sphere name (defaults to "sphere" if none provided).
 * @return Pointer to the resulting PSMRTS_Tracer object.
 */
PSMRTS_Tracer *psmrts_create_sphere( const double radius_km,
                                     const char *name ) {

  return ( new PSMRTS_Tracer ( psmrts::PsmrtsTracer::sphere( radius_km, name ) ) );
}

/**
 * @brief psmrts_create_spheroid - Creates a PSMRTS spheroid tracer
 *
 * Given 'a' and 'c' radii (km) and an optional name, this function
 * creates a PSMRTS tracer object that can be used to trace on a spheroid.
 *
 * The a, b radii are set to the input 'a' value. Radii a,b > c produces an 'oblate'
 * or flattened spheroid. Radii a,b < c produces a 'prolate' or elongated spheroid.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param a_radius_km Double, sphere axis 'a' radius in km.
 * @param c_radius_km Double, sphere axis 'c' radius in km.
 * @param name  Spheroid name (defaults to "spheroid" if none provided).
 * @return Pointer to the resulting PSMRTS_Tracer object.
 */
PSMRTS_Tracer *psmrts_create_spheroid( const double a_radius_km,
                                       const double c_radius_km,
                                       const char *name ) {

  return ( new PSMRTS_Tracer ( psmrts::PsmrtsTracer::spheroid( a_radius_km,
                                                               c_radius_km,
                                                               name ) ) );
}

/**
 * @brief psmrts_create_ellipsoid - Creates a PSMRTS ellipsoid tracer
 *
 * Given axes 'a,' 'b,' and 'c' radii (km) and an optional name, this function
 * creates a PSMRTS tracer object that can be used to trace on an ellipsoid.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param a_radius_km Double, sphere axis 'a' radius in km.
 * @param b_radius_km Double, sphere axis 'b' radius in km.
 * @param c_radius_km Double, sphere axis 'c' radius in km.
 * @param name  Ellipsoid name (defaults to "ellipsoid" if none provided).
 * @return Pointer to the resulting PSMRTS_Tracer object.
 */
PSMRTS_Tracer *psmrts_create_ellipsoid( const double a_radius_km,
                                        const double b_radius_km,
                                        const double c_radius_km,
                                        const char *name ) {

  return ( new PSMRTS_Tracer ( psmrts::PsmrtsTracer::ellipsoid( a_radius_km,
                                                                b_radius_km,
                                                                c_radius_km,
                                                                name ) ) );
}

/**
 * @brief psmrts_create_ellipsoid_v - Creates a PSMRTS ellipsoid tracer
 *
 * Given ellipsoid radii in the form of an PSMRTS_Vector3d (km)and an optional
 * name, this function creates a PSMRTS tracer object that can be used to trace
 * on an ellipsoid.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param radii PSMRTS_Vector3d, stores 3 ellipsoid radii (km)
 * @param name  Ellipsoid name (defaults to "ellipsoid" if none provided).
 * @return Pointer to the resulting PSMRTS_Tracer object.
 */
PSMRTS_Tracer *psmrts_create_ellipsoid_v( const PSMRTS_Vector3d *radii,
                                          const char *name ) {

  return ( new PSMRTS_Tracer( psmrts::PsmrtsTracer::ellipsoid( vector_to_eigen(*radii),
                                                               name ) ) );
}

/**
 * @brief psmrts_create_bullet - Creates a PSMRTS Bullet tracer
 *
 * Given a shapefile, this function creates a PSMRTS tracer object that can be
 * used to trace on an meshfile via the bullet ray trace engine.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param meshfile const char*, mesh filename.
 * @return Pointer to the resulting PSMRTS_Tracer object.
 */
PSMRTS_Tracer *psmrts_create_bullet( const char *objfile ) {
  return ( new PSMRTS_Tracer( psmrts::PsmrtsTracer::bullet( objfile ) ) );
}

/**
 * @brief psmrts_create_naifdsk - Creates a PSMRTS Naif DSK tracer
 *
 * Given a shapefile, this function creates a PSMRTS tracer object that can be
 * used to trace on an meshfile via the Naif DSK ray trace engine.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param meshfile const char*, mesh filename.
 * @return Pointer to the resulting PSMRTS_Tracer object.
 */
PSMRTS_Tracer *psmrts_create_naifdsk( const char *dskfile ) {

  return ( new PSMRTS_Tracer( psmrts::PsmrtsTracer::naifdsk( dskfile ) ) );
}

/**
 * @brief psmrts_tracer_valid - Validates PSMRTS_Tracer.
 *
 * Validates given PSMRTS_Tracer pointer by confirming it is not null.
 *
 * @param tracer Pointer to PSMRTS_Tracer object.
 * @return PSMRTS_BOOL Validity of input PSMRTS_Tracer.
 */
PSMRTS_BOOL psmrts_tracer_valid( const PSMRTS_Tracer *tracer ) {
  return ( evaluate( 0 != tracer ) );
}

/**
 * @brief psmrts_free_ray - Frees memory allocated to input PSMRTS_RayTrace
 *                          pointer.
 *
 * This function frees memory allocated to the input PSMRTS_RayTrace pointer.
 *
 * @param trace Pointer to PSMRTS_RayTrace.
 * @return void
 */
void psmrts_free_ray( PSMRTS_RayTrace *trace ) {
  delete trace;
}

/**
 * @brief psmrts_free_shape - Frees memory allocated to input PSMRTS_Shape
 *                            pointer.
 *
 * This function frees memory allocated to the input PSMRTS_Shape pointer.
 *
 * @param shape Pointer to PSMRTS_Shape.
 * @return void
 */
void psmrts_free_shape( PSMRTS_Shape *shape ) {
  delete shape;
}

/**
 * @brief psmrts_free_tracer - Frees memory allocated to input PSMRTS_Tracer
 *                            pointer.
 *
 * This function frees memory allocated to the input PSMRTS_Tracer pointer.
 *
 * @param tracer Pointer to PSMRTS_Tracer.
 * @return void
 */
void psmrts_free_tracer( PSMRTS_Tracer *tracer ){
  delete tracer;
}

/**
 * @brief psmrts_free_shapetracer - Frees memory allocated to input
 *                                  PSMRTS_ShapeTracer pointer.
 *
 * This function frees memory allocated to the input PSMRTS_ShapeTracer pointer.
 *
 * @param stracer Pointer to PSMRTS_ShapeTracer.
 * @return void
 */
void psmrts_free_shapetracer( PSMRTS_Tracer *stracer ) {
  delete stracer;
}

/**
 * @brief psmrts_free_priority_tracer - Frees memory allocated to input
 *                      PSMRTS_PriorityTracer pointer.
 *
 * This function frees memory allocated to the input PSMRTS_PriorityTracer
 * pointer.
 *
 * @param ptracer Pointer to PSMRTS_PriorityTracer.
 * @return void
 */
void psmrts_free_priority_tracer( PSMRTS_PriorityTracer *ptracer ) {
  delete ptracer;
}

/**
 * @brief psmrts_free_photometric_ray - Frees memory allocated to input
 *                                      PSMRTS_PhotometricRayTrace pointer.
 *
 * This function frees memory allocated to the input PSMRTS_PhotometricRayTrace
 * pointer.
 *
 * @param phototrace Pointer to PSMRTS_PhotometricRayTrace.
 * @return void
 */
void psmrts_free_photometric_ray( PSMRTS_PhotometricRayTrace *phototrace ) {
  delete phototrace;
}

/**
 * @brief psmrts_free_trace_array - Frees memory allocated to input
 *                                  PSMRTS_TraceArray pointer.
 *
 * This function frees memory allocated to the input PSMRTS_TraceArray pointer.
 * Note that we don't free the traces in the array.
 *
 * @param tracearray Pointer to PSMRTS_TraceArray.
 * @return void
 */
void psmrts_free_trace_array( PSMRTS_TraceArray *tracearray ) {
  delete tracearray;
}

/**
 * @brief psmrts_free_photometric_trace_array - Frees memory allocated to input
 *                                         PSMRTS_PhotometricTraceArray pointer.
 *
 * This function frees memory allocated to the input
 * PSMRTS_PhotometricTraceArray pointer.
 * Note that we don't free the traces in the array.
 *
 * @param ptracearray Pointer to PSMRTS_PhotometricTraceArray.
 * @return void
 */
void psmrts_free_photometric_trace_array( PSMRTS_PhotometricTraceArray *ptracearray ) {
  delete ptracearray;
}

}
