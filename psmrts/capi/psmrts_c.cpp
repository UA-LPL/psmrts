/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

/** @file psmrts_c.cpp */

#include <string>
#include <deque>
#include <Eigen/Geometry>

#include <psmrts/core/psmrts_version.h>

#include <psmrts/core/products/ProductConfiguration.hpp> 
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/PsmrtsBufferData.hpp>
#include <psmrts/core/PsmrtsBuffer.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/PsmrtsPriorityTracer.hpp>
#include <psmrts/core/PsmrtsInvoice.hpp>

/*============ PSMRTS C API type definitions ============*/
/* Must be defined before including psmrts_c.h */
#define PSMRTS_POINTERS 1
using PSMRTS_Invoice               = psmrts::PsmrtsInvoice;
using PSMRTS_Translations          = psmrts::PsmrtsTranslations;
using PSMRTS_ProductConfiguration  = psmrts::ProductConfiguration;
using PSMRTS_String                = std::string;
using PSMRTS_StringArray           = std::deque<PSMRTS_String>;
using PSMRTS_RayTrace              = psmrts::PRQRayTrace;
using PSMRTS_Shape                 = psmrts::PsmrtsShape;
using PSMRTS_Tracer                = psmrts::PsmrtsTracer;
using PSMRTS_PriorityTracer        = psmrts::PsmrtsPriorityTracer;
using PSMRTS_PhotometricRayTrace   = psmrts::PRQPhotometricTrace;
using PSMRTS_TraceArray            = psmrts::PRQRayTraceArray;
using PSMRTS_PhotometricTraceArray = psmrts::PRQPhotometricTraceArray;

/* Include the PSMRTS C api include */
#include <psmrts/capi/psmrts_c.h>

/**
 * @brief vector_to_eigen_d - Converts a PSMRTS_Vector3d of doubles to an
 *                            Eigen Vector3d.
 *
 * This function converts an input PSMRTS_Vector3d of doubles to an
 * Eigen Vector3d.
 *
 * @param v3d PSMRTS_Vector3d of doubles.
 * @return Eigen::Vector3d converted from PSMRTS_Vector3d.
 */
inline Eigen::Vector3d vector_to_eigen_d( const PSMRTS_Vector3d &v3d ) {
  return ( Eigen::Vector3d( { v3d.a, v3d.b, v3d.c } ) );
}

/**
 * @brief eigen_to_vector_d - Converts an Eigen Vector3d of doubles to a
 *                            PSMRTS_Vector3d.
 *
 * This function converts an input PSMRTS_Vector3d of doubles to an
 * Eigen Vector3d.
 *
 * @param v Eigen::Vector3d of doubles.
 * @return PSMRTS_Vector3d converted from Eigen::Vector3d.
 */
inline PSMRTS_Vector3d eigen_to_vector_d( const Eigen::Vector3d &v ) {
  PSMRTS_Vector3d v3d = { v[0], v[1], v[2] };
  return ( v3d );
}

/**
 * @brief vector_to_eigen_i - Converts a PSMRTS_Vector3i of integers to an
 *                            Eigen Vector3i.
 *
 * This function converts an input PSMRTS_Vector3i of integers to an
 * Eigen Vector3i.
 *
 * @param v3i PSMRTS_Vector3i of integers.
 * @return Eigen::Vector3i converted from PSMRTS_Vector3i.
 */
inline Eigen::Vector3i vector_to_eigen_i( const PSMRTS_Vector3i &v3i ) {
  return ( Eigen::Vector3i( { v3i.i, v3i.j, v3i.k } ) );
}

/**
 * @brief eigen_to_vector_i - Converts an Eigen Vector3i of integers to a
 *                            PSMRTS_Vector3d.
 *
 * This function converts an input PSMRTS_Vector3i of integers to an
 * Eigen Vector3d.
 *
 * @param v Eigen::Vector3i
 * @return PSMRTS_Vector3i converted from Eigen::Vector3i.
 */
inline PSMRTS_Vector3i eigen_to_vector_i( const Eigen::Vector3i &v ) {
  PSMRTS_Vector3i v3i = { v[0], v[1], v[2] };
  return ( v3i );
}

/**
 * @brief psmrts_facet_to_capi - Converts psmrts::PRQFacet to a c api facet.
 *
 * This function converts an input psmrts::PRQFacet to a c api facet structure.
 *
 * @param prq_facet psmrts::PRQFacet.
 * @return PSMRTS_Facet C api facet converted from psmrts::PRQFacet.
 */
inline PSMRTS_Facet psmrts_facet_to_capi( const psmrts::PRQFacet &prq_facet ) {
  PSMRTS_Facet facet;

  facet.m_has_facet = prq_facet.m_facet.m_has_facet;
  facet.m_plateid   = prq_facet.m_facet.m_plateid;
  facet.m_segment   = prq_facet.m_facet.m_segment;
  facet.m_indexes   = eigen_to_vector_i( prq_facet.m_facet.m_indexes );
  facet.m_vector1   = eigen_to_vector_d( prq_facet.m_facet.m_vector1 );
  facet.m_vector2   = eigen_to_vector_d( prq_facet.m_facet.m_vector2 );
  facet.m_vector3   = eigen_to_vector_d( prq_facet.m_facet.m_vector3 );
  facet.m_normal    = eigen_to_vector_d( prq_facet.m_facet.m_normal );

  return ( facet );
}

/**
 * @brief capi_facet_to_psmrts - Converts a c api facet to a psmrts::PRQFacet.
 *
 * This function converts an input c api facet to a psmrts::PRQFacet.
 *
 * @param facet C api facet.
 * @return psmrts::PRQFacet converted from c api facet.
 */
inline psmrts::PRQFacet capi_facet_to_psmrts( const PSMRTS_Facet &facet ) {
  psmrts::PRQFacet prq_facet;

  prq_facet.m_facet.m_has_facet = facet.m_has_facet;
  prq_facet.m_facet.m_plateid = facet.m_plateid;
  prq_facet.m_facet.m_segment = facet.m_segment;
  prq_facet.m_facet.m_indexes   = vector_to_eigen_i( facet.m_indexes );
  prq_facet.m_facet.m_vector1   = vector_to_eigen_d( facet.m_vector1 );
  prq_facet.m_facet.m_vector2   = vector_to_eigen_d( facet.m_vector2 );
  prq_facet.m_facet.m_vector3   = vector_to_eigen_d( facet.m_vector3 );
  prq_facet.m_facet.m_normal    = vector_to_eigen_d( facet.m_normal );

  return ( prq_facet );
}

/**
 * @brief to_psmrts_bool - Evaluate input bool type and return PSMRTS_TRUE or
 *                         PSMRTS_FALSE.
 *
 * This function evaluates the given bool type and returns PSMRTS_TRUE or
 * PSMRTS_FALSE.
 *
 * @param b bool originating from C++ to agnostic PSMRTS type
 * @return Values of input PSMRTS_BOOL namely, PSMRTS_TRUE or PSMRTS_FALSE.
 */
inline PSMRTS_BOOL to_psmrts_bool( const bool b ) {
  return ( b ? PSMRTS_TRUE : PSMRTS_FALSE );
}

extern "C" {

/*============ PSMRTS information functions ============*/

/**
 * @brief psmrts_version - Returns a string with the PSMRTS version.
 *
 * This function returns a string with the PSMRTS Project version.
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

/*============ PSMRTS_String functions ================*/

/**
 * @brief psmrts_create_string() - Create a PSMRTS_String from a given const char* sbuf.
 *
 * Create a PSMRTS_String from a given const char* sbuf.
 *
 * @param sbuf const char* string buffer
 * @return PSMRTS_String
 */
PSMRTS_String *psmrts_create_string( const char* sbuf ) {

  PSMRTS_String *pstr = new PSMRTS_String( sbuf );

  return pstr;
}

/**
 * @brief psmrts_string_set() - Given PSMRTS_String *s and const char* sbuf, set the contents of
 *                              s to sbuf.
 *
 * Given PSMRTS_String *s and const char* sbuf, set the contents of s to sbuf.
 *
 * @param s PSMRTS_String.
 * @param sbuf const char* string buffer
 * @return void
 */
void psmrts_string_set( PSMRTS_String *s, const char* sbuf ) {
 s->assign(sbuf);
}

/**
 * @brief psmrts_string_length() - Return the length of a given PSMRTS_String.
 *
 * Return the length of a given PSMRTS_String.
 *
 * @param s PSMRTS_String.
 * @return int length of string.
 */
int psmrts_string_length( const PSMRTS_String *s ) {
 return s->length();
}

/**
 * @brief psmrts_string_content() - Given a PSMRTS_String pointer, this method returns
 *                                  a pointer to its content in the form of a pointer to
 *                                  a null-terminated C-style character array.
 *
 * Returns input string content in the form of a pointer to null-terminated C-style character array.
 *
 * @param s PSMRTS_String.
 * @return const char* pointer to null-terminated C-style character array with content of input string.
 */
const char* psmrts_string_content( const PSMRTS_String *s ) {
 return s->c_str();
}

/*============ PSMRTS_StringArray functions ================*/

/**
 * @brief psmrts_create_string_array - Constructs PSMRTS_StringArray.
 *
 * This function constructs a PSMRTS_StringArray.
 *
 * @return Pointer to PSMRTS_StringArray.
 */
PSMRTS_StringArray *psmrts_create_string_array() {
  return ( new PSMRTS_StringArray() );
}

/**
 * @brief psmrts_string_array_size - Returns PSMRTS_StringArray size.
 *
 * Given a PSMRTS_StringArray object, this function returns its' size.
 *
 * @param tracearray Pointer to PSMRTS_StringArray object.
 * @return size_t Number of strings in array.
 */
size_t psmrts_string_array_size( const PSMRTS_StringArray *stringarray ) {
  return ( stringarray->size() );
}

/**
 * @brief psmrts_string_array_add_string - Adds string to string array.
 *
 * This function adds a PSMRTS_String to a given PSMRTS_StringArray object.
 *
 * @param stringarray Pointer to PSMRTS_StringArray.
 * @param sbuf const char*.
 * @return size_t Index of newly added string.
 */
size_t psmrts_string_array_add_string( PSMRTS_StringArray *stringarray,
                                       const char* sbuf ) {
  
  stringarray->push_back( *psmrts_create_string( sbuf ) );
  
  // return index of newly added string
  return ( stringarray->size() - 1 );
}

/**
 * @brief psmrts_string_array_clear - Clears string array.
 *
 * This function removes all strings from the given PSMRTS_StringArray.
 *
 * WARNING: References to strings in the string array are invalidated after this call!
 *
 * @param stringarray Pointer to PSMRTS_StringArray.
 * @return void
 */
void psmrts_string_array_clear( PSMRTS_StringArray *stringarray ) {
  stringarray->clear();
}

/**
 * @brief psmrts_string_array_get_string - Get string at given index from string
 *                                         array.
 *
 * This function retrieves a PSMRTS_String at the given index.
 *
 * NOTE: index is zero-based, i.e. ranging from 0 to 'n-1' where 'n' is the
 *       number of elements in the string array.
 *
 * @param stringarray Pointer to PSMRTS_StringArray.
 * @param index Integer array index for requested PSMRTS_String.
 * @return const PSMRTS_String Pointer to PSMRTS_String object at index.
 */
const PSMRTS_String *psmrts_string_array_get_string( const PSMRTS_StringArray *stringarray,
                                                     size_t index ) {
  return ( &stringarray->at( index ) );
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
 * @brief psmrts_vector3i - Creates a PSMRTS 3d integer vector object from input
 *                          coordinates.
 *
 * Given input integer coordinates, this function creates and returns a PSMRTS_Vector3i.
 *
 * @param v1 coordinate 1.
 * @param v2 coordinate 2.
 * @param v3 coordinate 3.
 * @return PSMRTS_Vector3i Integer vector constructed with input coordinates.
 */
PSMRTS_Vector3i psmrts_vector3i( const int v1, const int v2, const int v3 ) {

  PSMRTS_Vector3i v3i = { v1, v2, v3 };
  return ( v3i );
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

  return ( new PSMRTS_RayTrace( vector_to_eigen_d( *observer ),
                                vector_to_eigen_d( *lookdir ) ) );
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

  *trace = PSMRTS_RayTrace( vector_to_eigen_d( *observer ),
                            vector_to_eigen_d( *lookdir ) );

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
 * The lookdir is converted to unit vector and emanates from the observer
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
  return ( eigen_to_vector_d( ray->trace().observer() ) );
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
  return ( eigen_to_vector_d( ray->trace().lookdir() ) );
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
  return ( to_psmrts_bool( ray->trace().hasHit() ) );
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
  return ( eigen_to_vector_d( ray->trace().xyz() ) );
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
  return ( eigen_to_vector_d( ray->trace().raypt() ) );
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
  return ( eigen_to_vector_d( ray->trace().normal() ) );
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

  return ( psmrts::PsmrtsRayTrace::separation_angle( vector_to_eigen_d( *v1 ),
                                                     vector_to_eigen_d( *v2 ) ) );
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

  return ( to_psmrts_bool( ray1->trace().isNear( ray2->trace(), tolerance_km ) ) );
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
 * @brief psmrts_trace_array_trace - Processes all traces in a trace array.
 *
 * This function processes all traces in a trace array.
 *
 * @param tracearray Pointer to PSMRTS_TraceArray.
 * @param tracer Pointer to PSMRTS_Tracer.
 * @return PSMRTS_BOOL Success or failure of process.
 */
extern PSMRTS_BOOL psmrts_trace_array_trace( PSMRTS_TraceArray *tracearray,
                                             const PSMRTS_Tracer *tracer) {
  return ( tracer->process( *tracearray ) );
}

/**
 * @brief psmrts_trace_array_clear - Clears trace array.
 *
 * This function removes all traces from the given PSMRTS_TraceArray.
 *
 * WARNING: References to rays in the trace array are invalidated after this call!
 *
 * @param tracearray Pointer to PSMRTS_TraceArray.
 * @return void
 */
void psmrts_trace_array_clear(PSMRTS_TraceArray *tracearray) {
  tracearray->clear();
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
  return ( &tracearray->get_trace( index ) );
}

/**
 * @brief psmrts_create_photometric_ray - Creates a PSMRTS photometric ray
 *        trace object.
 *
 * Given vectors describing observer position and look direction, this function
 * creates a PSMRTS photometric ray trace object that can be used to trace on a
 * shape.
 *
 * Observer position is a vector from the shape origin to its body-fixed position
 * relative to the target body origin.
 *
 * The lookdir is converted to a unit vector and originates from the observer
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

  return ( new psmrts::PRQPhotometricTrace( vector_to_eigen_d( *observer ),
                                            vector_to_eigen_d( *lookdir ),
                                            vector_to_eigen_d( *sunpos ) ) );
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

  *phototrace = PSMRTS_PhotometricRayTrace( vector_to_eigen_d( *observer ),
                                            vector_to_eigen_d( *lookdir ),
                                            vector_to_eigen_d( *sunpos ) );

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
 * @brief psmrts_photometric_trace_array_trace - Processes all traces in a photometric trace
 *                                               array.
 *
 * This function processes all traces in a photometric trace array.
 *
 * @param tracearray Pointer to PSMRTS_PhotometricTraceArray.
 * @param tracer Pointer to PSMRTS_Tracer.
 * @return PSMRTS_BOOL Success or failure of process.
 */
extern PSMRTS_BOOL psmrts_photometric_trace_array_trace( PSMRTS_PhotometricTraceArray *tracearray,
                                                         const PSMRTS_Tracer *tracer) {
  return ( tracer->process( *tracearray ) );
}

/**
 * @brief psmrts_photometric_trace_array_clear - Clears photometric trace array.
 *
 * This function removes all traces from the given PSMRTS_PhotometricTraceArray.
 *
 * WARNING: References to rays in the trace array are invalidated after this call!
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
  return ( &tracearray->get_trace( index ) );
}

/**
 * @brief psmrts_lonlatrad_to_xyz_d - Converts vector in longitude, latitude,
 *        radius coordinates to xyz.
 *
 * Given an PSMRTS_Vector3d in longitude, latitude, radius coordinates, this
 * function converts it to xyz coordinates. Input angular coordinates are
 * assumed to be in degrees.
 *
 * WARNING: Latitude is assumed to lie within -90 to +90 degree range. If
 *          latitude falls outside of that range, it is clamped to
 *          identically -90 or +90 degrees.
 *
 * @param v Pointer to PSMRTS_Vector3d in lon, lat, radius coordinates.
 * @return PSMRTS_Vector3d Vector converted to xyz coordinates.
 */
PSMRTS_Vector3d psmrts_lonlatrad_to_xyz_d( const PSMRTS_Vector3d *v ) {

  return ( eigen_to_vector_d( psmrts::lonlatrad_to_xyz_d( vector_to_eigen_d(*v) ) ) );
}

/**
 * @brief psmrts_xyz_to_lonlatrad_d - Converts vector in xyz coordinates to
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

  return ( eigen_to_vector_d( psmrts::xyz_to_lonlatrad_d( vector_to_eigen_d( *v ) ) ) );
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
 * @param v PSMRTS_Vector3d Input vector with longitude/latitude in radians.
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

  return ( new PSMRTS_Tracer( psmrts::PsmrtsTracer::ellipsoid( vector_to_eigen_d(*radii),
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
 * @param objfile const char*, mesh filename.
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
 * @param dskfile const char*, mesh filename.
 * @return Pointer to the resulting PSMRTS_Tracer object.
 */
PSMRTS_Tracer *psmrts_create_naifdsk( const char *dskfile ) {
  return ( new PSMRTS_Tracer( psmrts::PsmrtsTracer::naifdsk( dskfile ) ) );
}

/**
 * @brief psmrts_get_facet - Creates and processes a psmrts::PRQFacet.
 *
 * Given PSMRTS_Tracer, PSMRTS_RayTrace, and PSMRTS_Facet objects, the tracer is used to
 * process the psmrts::PRQFacet. The PRQFacet data is copied to a PSMRTS_Facet structure
 * and returned.
 *
 * The input ray is required to have a valid observer and look direction. It is
 * the responsibility of the caller to check for valid pointer return.
 *
 * @param ray PSMRTS_RayTrace pointer.
 * @param tracer PSMRTS_Tracer pointer.
 * @param facet PSMRTS_Facet pointer.
 * @return PSMRTS_BOOL TRUE/FALSE, success of facet processing.
 */
PSMRTS_BOOL psmrts_get_facet( PSMRTS_RayTrace *ray, const PSMRTS_Tracer *tracer,
                              PSMRTS_Facet *facet ) {

  assert( ray != nullptr && "psmrts_ray_trace::PSMRTS_RayTrace is null" );
  assert( tracer != nullptr && "psmrts_tracer::PSMRTS_Tracer is null" );

  // construct PRQFacet with PsmrtsRayTrace from PSMRTS_RayTrace argument
  psmrts::PRQFacet prqFacet( ray->trace() );

  // process facet
  PSMRTS_BOOL b = ( tracer->process( prqFacet ) ? PSMRTS_TRUE : PSMRTS_FALSE);

  // copy PRQFacet data to PSMRTS_FACET
  *facet = psmrts_facet_to_capi( prqFacet );

  return ( b );
}

/**
 * @brief psmrts_create_obj_shape - Creates obj PSMRTS_Shape.
 *
 * Given a const char* obj filename, this method creates a PSMRTS_Shape.
 *
 * @param objfile const char*.
 * @return PSMRTS_Shape*.
 */
PSMRTS_Shape *psmrts_create_obj_shape( const char *objfile ) {
  return ( new PSMRTS_Shape( psmrts::PsmrtsShape( psmrts::ObjShape( objfile ) ) ) );
}

/**
 * @brief psmrts_create_dsk_shape - Creates dsk PSMRTS_Shape.
 *
 * Given a const char* dsk filename, this method creates a PSMRTS_Shape.
 *
 * @param objfile const char*.
 * @return PSMRTS_Shape*.
 */
PSMRTS_Shape *psmrts_create_dsk_shape( const char *dskfile ) {
  return ( new PSMRTS_Shape( psmrts::PsmrtsShape( psmrts::DskShape( dskfile ) ) ) );
}

/**
 * @brief psmrts_create_ply_shape - Creates ply PSMRTS_Shape.
 *
 * Given a const char* ply filename, this method creates a PSMRTS_Shape.
 *
 * @param objfile const char*.
 * @return PSMRTS_Shape*.
 */
PSMRTS_Shape *psmrts_create_ply_shape( const char *plyfile ) {
  return ( new PSMRTS_Shape( psmrts::PsmrtsShape( psmrts::PlyShape( plyfile ) ) ) );
}  

/**
 * @brief psmrts_facet_surface_area - Computes facet surface area.
 *
 * Given a PSMRTS_Facet, this function computes the facet surface area.
 *
 *
 * @param facet PSMRTS_Facet pointer.
 * @return double facet surface area.
 */
double psmrts_facet_surface_area( const PSMRTS_Facet *facet ) {

  // convert PSMRTS_Facet to psmrts::PRQFacet
  psmrts::PRQFacet prqfacet = capi_facet_to_psmrts( *facet );

  return ( prqfacet.facet().surface_area() );
}

/**
 * @brief psmrts_facet_volume - Computes facet volume.
 *
 * Given a PSMRTS_Facet, this function computes the facet volume which is actually the volume of the
 * tetrahedron defined by the facet vertices and the origin (0,0,0). The resulting volume is signed
 * positive if the facet normal faces outward and negative if it faces inward. The normal direction
 * is defined by the winding order of the vertices. See for example ...
 *
 * <a href="http://chenlab.ece.cornell.edu/Publication/Cha/icip01_Cha.pdf">Cha Zhang and Tsuhan Chen,
 * "Efficient Feature Extraction for 2D/3D Objects in Mesh Representation," Proceedings 2001 International
 * Conference on Image Processing (Cat. No.01CH37205), Thessaloniki, Greece, 2001, pp. 935-938 vol.3,
 * doi: 10.1109/ICIP.2001.958278.</a>
 *
 * @param facet PSMRTS_Facet pointer.
 * @return double signed facet volume.
 */
double psmrts_facet_volume( const PSMRTS_Facet *facet ) {

  // convert PSMRTS_Facet to psmrts::PRQFacet
  psmrts::PRQFacet prqfacet = capi_facet_to_psmrts( *facet );

  return ( prqfacet.facet().volume() );
}

/**
 * @brief psmrts_mesh_surface_area - Computes shape surface area.
 *
 * Given a PSMRTS_Shape, this function computes the total mesh surface area.
 *
 * @param shape PSMRTS_Shape pointer.
 * @return double Shape surface area.
 */
extern double psmrts_mesh_surface_area( const PSMRTS_Shape *shape ) {
  return ( shape->get_mesh().mesh_surface_area() );
}

/**
 * @brief psmrts_mesh_volume - Computes shape volume.
 *
 * Given a PSMRTS_Shape, this function computes the total mesh volume.
 *
 * @param shape PSMRTS_Shape pointer.
 * @return double Shape volume.
 */
extern double psmrts_mesh_volume( const PSMRTS_Shape *shape ) {
  return ( shape->get_mesh().mesh_volume() );
}

/**
 * @brief psmrts_create_translation - Creates PSMRTS_Translations object.
 *
 * This function creates a PSMRTS_Translations object.
 *
 * @return PsmrtsTranslations*.
 */
PSMRTS_Translations *psmrts_create_translation() {
  
  // create translation
  PSMRTS_Translations *trans_t = new PSMRTS_Translations( psmrts::PsmrtsTranslations::create() );

  return trans_t;
}

/**
 * @brief psmrts_add_translation_parameter - Adds parameter from the given name and value
 * strings to an input PSMRTS_Translations object.
 *
 * Adds parameter from the given name and value strings to an input
 * PSMRTS_Translations object.
 *
 * @param translations PsmrtsTranslations*
 * @param name const char*
 * @param value const char*
 * @return void.
 */
void psmrts_add_translation_parameter( PSMRTS_Translations *translations, const char* name,
                                       const char* value ) {  
  translations->add_parameter( name, value );
}

/**
 * @brief psmrts_create_product_config - Creates PSMRTS_ProductConfiguration from the given id.
 *
 * This function creates a PSMRTS_ProductConfiguration from the given id.
 *
 * @param id const char*
 * @return PSMRTS_ProductConfiguration*.
 */
PSMRTS_ProductConfiguration *psmrts_create_product_config( const char *id ) {  
  return ( new PSMRTS_ProductConfiguration( psmrts::ProductConfiguration( id ) ) );
}

/**
 * @brief psmrts_create_config - Creates PSMRTS_ProductConfiguration from the given
 *                               producttype and productname.
 *
 * This function creates a PSMRTS_ProductConfiguration from the given producttype
 * and productname.
 *
 * producttype is either "shape" or "tracer."
 * productname is dsk, ply, or obj if producttype is shape.
 * productname is bullet, ellipsoid, sphere, spheroid, or naifdsk if producttype is tracer.
 * config can be an existing PSMRTS_ProductConfiguration or a nullptr.

 * @param producttype const char*
 * @param productname const char*
 * @param config PSMRTS_ProductConfiguration*
 * @return PSMRTS_ProductConfiguration*.
 */
PSMRTS_ProductConfiguration *psmrts_create_config( const char *producttype, const char* productname,
                                                   PSMRTS_ProductConfiguration *config ) {
  
  // if config is nullptr, create one
  if ( config == nullptr ) {
    config = psmrts_create_product_config( producttype );
  }

  config->add( psmrts::ProductOption( producttype, productname ) );

  return config;
}

/**
 * @brief psmrts_product_config_contains - Check for an existing ProductOption in 
 * the given PSMRTS_ProductConfiguration (not including metadata).
 *
 * Check for an existing ProductOption in the given PSMRTS_ProductConfiguration
 * (not including metadata).
 *
 * @param config PSMRTS_ProductConfiguration*
 * @param text const char*
 * @return PSMRTS_BOOL.
 */
PSMRTS_BOOL psmrts_product_config_contains( PSMRTS_ProductConfiguration *config,
                                            const char* text ) {
  return config->contains( text );
}

/**
 * @brief psmrts_product_config_to_string - Output PSMRTS_ProductConfiguration
 *                                          meta data to string.
 *
 * Output PSMRTS_ProductConfiguration meta data to string.
 * 
 * Note: PSMRTS_String* must be allocated prior to call, and deleted when
 *       no longer needed.
 *
 * @param config PSMRTS_ProductConfiguration*
 * @param pstr PSMRTS_String*
 * @return void.
 */
PSMRTS_C_EXPORT void psmrts_product_config_to_string( PSMRTS_ProductConfiguration *config,
                                                      PSMRTS_String *pstr ) {
  pstr->assign( config->to_json().dump() );  
}

/**
 * @brief psmrts_add_config_options_string - Create ProductOptions from "name"
 *   and "text" and adds it to the given PSMRTS_ProductConfiguration.
 *
 * Create ProductOptions from "name" and "text" and adds it to the given
 * PSMRTS_ProductConfiguration.
 *
 * @param config PSMRTS_ProductConfiguration*
 * @param name const char*.
 * @param text const char*.
 * @return void.
 */
void psmrts_add_product_string( PSMRTS_ProductConfiguration *config,
                                const char *name, const char *text ) {
                                          
  config->add( psmrts::ProductOption( name, text ) );
}

/**
 * @brief psmrts_add_config_options_bool - Create ProductOptions from "name"
 *   and "b" and adds it to the given PSMRTS_ProductConfiguration.
 *
 * Create ProductOptions from "name" and "b" and adds it to the given
 * PSMRTS_ProductConfiguration.
 *
 * @param config PSMRTS_ProductConfiguration*
 * @param name const char*.
 * @param b const PSMRTS_BOOL.
 * @return void.
 */
void psmrts_add_product_bool( PSMRTS_ProductConfiguration *config,
                              const char *name, const PSMRTS_BOOL b ) {
                                          
  config->add( psmrts::ProductOption( name, b ) );
}

/**
 * @brief psmrts_add_config_options_int - Create ProductOptions from "name"
 *   and "i" and adds it to the given PSMRTS_ProductConfiguration.
 *
 * Create ProductOptions from "name" and "i" and adds it to the given
 * PSMRTS_ProductConfiguration.
 *
 * @param config PSMRTS_ProductConfiguration*
 * @param name const char*.
 * @param i const int.
 * @return void.
 */
void psmrts_add_product_int( PSMRTS_ProductConfiguration *config,
                             const char *name, const int i ) {
                                          
  config->add( psmrts::ProductOption( name, i ) );
}

/**
 * @brief psmrts_add_config_options_sizet - Create ProductOptions from "name"
 *   and "szt" and adds it to the given PSMRTS_ProductConfiguration.
 *
 * Create ProductOptions from "name" and "szt" and adds it to the given
 * PSMRTS_ProductConfiguration.
 *
 * @param config PSMRTS_ProductConfiguration*
 * @param name const char*.
 * @param szt const size_t.
 * @return void.
 */
void psmrts_add_product_sizet( PSMRTS_ProductConfiguration *config,
                               const char *name, const size_t szt ) {
                                          
  config->add( psmrts::ProductOption( name, szt ) );
}

/**
 * @brief psmrts_add_config_options_bool - Create ProductOptions from "name"
 *   and "d" and adds it to the given PSMRTS_ProductConfiguration.
 *
 * Create ProductOptions from "name" and "d" and adds it to the given
 * PSMRTS_ProductConfiguration.
 *
 * @param config PSMRTS_ProductConfiguration*
 * @param name const char*.
 * @param d const double.
 * @return void.
 */
void psmrts_add_product_double( PSMRTS_ProductConfiguration *config,
                                const char *name, const double d ) {
                                          
  config->add( psmrts::ProductOption( name, d ) );
}

/**
 * @brief psmrts_add_config_options_double_vector - Create ProductOptions from "name"
 *   and a vector of "count" doubles "d" and adds it to the given
 *   PSMRTS_ProductConfiguration.
 *
 * Create ProductOptions from "name" and a vector of "count" doubles "d" and adds it
 * to the given PSMRTS_ProductConfiguration.
 *
 * @param config PSMRTS_ProductConfiguration*
 * @param name const char*.
 * @param d_vector const double.
 * @param count const int. 
 * @return void.
 */
void psmrts_add_product_double_vector( PSMRTS_ProductConfiguration *config,
                                       const char *name,
                                       const double *d_vector,
                                       const int count ) {

  std::vector<double> cpp_vector(count);
  std::copy_n( d_vector, count, cpp_vector.begin() );
                                                                                          
  config->add( psmrts::ProductOption( name, cpp_vector ) );
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
  return ( to_psmrts_bool( 0 != tracer ) );
}

/*============ PSMRTS_Invoice functions ================*/

/**
 * @brief psmrts_create_invoice - Creates PSMRTS_Invoice given const char* name
 * and PSMRTS_Translations* translation.
 *
 * Creates PSMRTS_Invoice given const char* name and PSMRTS_Translations* translation.
 *
 * @param name const char*.
 * @param translation PSMRTS_Translations*.
 * @return PSMRTS_PSMRTS_Invoice*.
 */
PSMRTS_Invoice *psmrts_create_invoice( const char* name,
                                       PSMRTS_Translations* translation ) {
  if ( translation == nullptr ) {
    return ( new PSMRTS_Invoice( name ) );
  }
    
  return ( new PSMRTS_Invoice( name, *translation ) );
}

/**
 * @brief psmrts_add_config_invoice - Adds the given PSMRTS_ProductConfiguration to
 *                                    the given PSMRTS_Invoice.
 *
 * Adds the given PSMRTS_Invoice to the given PSMRTS_ProductConfiguration.
 *
 * @param config PSMRTS_ProductConfiguration*.
 * @param invoice PSMRTS_Invoice*.
 * @return PSMRTS_BOOL.
 */
PSMRTS_BOOL psmrts_add_config_invoice( PSMRTS_ProductConfiguration *config,
                                       PSMRTS_Invoice *invoice ) {

  return ( invoice->add_product( *config ) );
}

/**
 * @brief psmrts_generate_products - Generates products from given PSMRTS_Invoice.
 *
 * Generates products from given PSMRTS_Invoice.
 *
 * @param invoice PSMRTS_Invoice*.
 * @return PSMRTS_BOOL.
 */
PSMRTS_BOOL psmrts_generate_products( PSMRTS_Invoice *invoice ) {
  return ( invoice->generate_products() );
}

/**
 * @brief psmrts_invoice_error_string - Returns error string from given PSMRTS_Invoice.
 *
 * Returns error string from given PSMRTS_Invoice.

 * Note: If input string is nullptr, a new string is allocated and returned with the
 * caller taking ownership.
 *
 * @param invoice PSMRTS_Invoice*.
 * @return PSMRTS_BOOL.
 */
PSMRTS_String *psmrts_invoice_error_string( const PSMRTS_Invoice *invoice,
                                            PSMRTS_String *string ) {

  PSMRTS_String *string_t = string;

  if ( string_t == nullptr ) {
    string_t = new PSMRTS_String();
  }
  
  *string_t = invoice->errors_to_string();

  return ( string_t );
}

/**
 * @brief psmrts_generate_priority_tracer - Creates PSMRTS_PriorityTracer from the given
 *                                          PSMRTS_Invoice.
 *
 * Creates PSMRTS_PriorityTracer from the given PSMRTS_Invoice.
 * 
 * Note: If input PSMRTS_PriorityTracer is nullptr, a new PSMRTS_PriorityTracer is
 * allocated and returned with the caller taking ownership.
 *
 * @param invoice PSMRTS_Invoice*.
 * @param tracer_p PSMRTS_PriorityTracer*.
 * @return PSMRTS_BOOL.
 */
PSMRTS_PriorityTracer *psmrts_generate_priority_tracer( PSMRTS_Invoice *invoice,
                                                        PSMRTS_PriorityTracer* tracer_p) {
  PSMRTS_PriorityTracer *tracer_t = tracer_p;

  if ( tracer_t == nullptr ) {
    tracer_t = new PSMRTS_PriorityTracer();
  }

  *tracer_t = invoice->get_priority_tracer();

  return tracer_t;
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

/**
 * @brief psmrts_free_product_config - Frees memory allocated to input
 *                                     PSMRTS_ProductConfiguration pointer.
 *
 * This function frees memory allocated to the input PSMRTS_ProductConfiguration pointer.
 *
 * @param config PSMRTS_ProductConfiguration*.
 * @return void
 */
void psmrts_free_product_config( PSMRTS_ProductConfiguration* config ) {
  delete config;
}

/**
 * @brief psmrts_free_invoice - Frees memory allocated to input PSMRTS_Invoice pointer.
 *
 * This function frees memory allocated to the input PSMRTS_Invoice pointer.
 *
 * @param invoice PSMRTS_Invoice*.
 * @return void
 */
void psmrts_free_invoice( PSMRTS_Invoice* invoice ) {
  delete invoice;
}

/**
 * @brief psmrts_free_translations - Frees memory allocated to input psmrts_free_translations pointer.
 *
 * This function frees memory allocated to the input psmrts_free_translations pointer.
 *
 * @param translations psmrts_free_translations*.
 * @return void
 */
void psmrts_free_translations( PSMRTS_Translations* translations ) {
  delete translations;
}

/**
 * @brief psmrts_free_string - Frees memory allocated to input PSMRTS_RayTrace
 *                             pointer.
 *
 * This function frees memory allocated to the input PSMRTS_String pointer.
 *
 * @param s Pointer to PSMRTS_String.
 * @return void
 */
void psmrts_free_string( PSMRTS_String *s ) {
  delete s;
}

/**
 * @brief psmrts_free_string_array - Frees memory allocated to input
 *                                   PSMRTS_StringArray pointer.
 *
 * This function frees memory allocated to the input
 * PSMRTS_StringArray pointer.
 * Note that we don't free the strings in the array.
 *
 * @param ptracearray Pointer to PSMRTS_PhotometricTraceArray.
 * @return void
 */
void psmrts_free_string_array( PSMRTS_StringArray *pstringarray ) {
  delete pstringarray;
}

}
