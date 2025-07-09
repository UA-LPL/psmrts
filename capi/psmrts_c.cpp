

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


/*============ PSMRTS C API type definitions ============*/
/* Must be defined before including psmrts_c.h */
#define PSMRTS_POINTERS 1
using PSMRTS_RayTrace       = psmrts::PRQRayTrace;
using PSMRTS_Shape          = psmrts::PsmrtsMeshData;
using PSMRTS_Tracer         = psmrts::PsmrtsTracerModel;
using PSMRTS_ShapeTracer    = psmrts::PsmrtsTracerModel;
using PSMRTS_PriorityTracer = psmrts::PsmrtsPriorityTracer;

/* photometric tracers */
using PSMRTS_PhotometricRayTrace = psmrts::PRQPhotometricTrace;

/* Include the PSMRTS C api include */
#include "psmrts_c.h"

/**
 * @brief vector_to_eigen - Converts a PSMRTS_Vector3d to an Eigen Vector3d.
 *
 * This function converts an input PSMRTS_Vector3d to an Eigen Vector3d..
 *
 * @param v3d PSMRTS_Vector3d.
 * @return Eigen::Vector3d Eigen Vector3d converted from PSMRTS_Vector3d.
 */
inline Eigen::Vector3d vector_to_eigen( const PSMRTS_Vector3d &v3d ) {
  return ( Eigen::Vector3d( { v3d.a, v3d.b, v3d.c } ) );
}

/**
 * @brief eigen_to_vector - Converts an Eigen Vector3d to a PSMRTS_Vector3d.
 *
 * This function converts an input PSMRTS_Vector3d to an Eigen Vector3d.
 *
 * @param v Eigen::Vector3d.
 * @return PSMRTS_Vector3d PSMRTS_Vector3d converted from an Eigen Vector3d.
 */
inline PSMRTS_Vector3d eigen_to_vector( const Eigen::Vector3d &v ) {
  PSMRTS_Vector3d v3d = { v[0], v[1], v[2] };
   return ( v3d );
}

extern "C" {

/*============ PSMRTS information functions ============*/

/**
 * @brief psmrts_version - Returns a string with the PSMRTS version.
 *
 * This function returns a string with the PSRMTS version.
 *
 * @return char* String with PSMRTS version.
 */
const char *psmrts_version() {
  return ( PROJECT_VERSION );
}

/**
 * @brief psmrts_info - Returns a string with the PSMRTS version.
 *
 * This function returns a string with the PSRMTS version.
 *
 * @return char* String with PSMRTS version.
 */
const char *psmrts_info() {
  return ( PSMRTS_VERSION );
}
#if 0
PSMRTS_ShapeTracer *psmrts_load_shape( const char *shape, const char *tracer ) {
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
    psmrts::PsmrtsMeshData mesh;
    if ( psmrts::psmrts_file_extension( v_shape ) == "obj" ) {
      mesh = psmrts::PsmrtsOBJFormat( v_shape );
    }
    else {
      if ( psmrts::psmrts_file_extension( v_shape ) == "ply" ) {
        mesh = psmrts::PsmrtsPLYFormat ply( v_shape );
      }
    }

    /* Allocate tracer */
    if ( v_tracer.find("dsk") != std::string::npos ) {
      return ( new psmrts::NaifDskShapeTracer ( v_shape ) );
    }
    else if ( v_tracer.find("ellipsoid") != std::string::npos ) {
      std::vector<std::string> tokens = psmrts::string_tokenizer(v_shape, "=:,");
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
#endif

/**
 * @brief psmrts_vector3d - Creates a PSMRTS 3d vector object
 *
 * Given input x,y, and z coordinates, this function creates and returns a
 * PSMRTS_Vector3d object.
 *
 * @param x double x-coordinate.
 * @param y double y-coordinate.
 * @param z double z-coordinate.
 * @return PSMRTS_Vector3d Vector with input coordinates.
 */
PSMRTS_Vector3d psmrts_vector3d( const double x, const double y,
                                 const double z ) {
  PSMRTS_Vector3d v3d = { x, y, z };  
  return ( v3d );
}

/**
 * @brief psmrts_negate - Creates a negated (or flipped) version of a 3d vector.
 *
 * Given an input PSMRTS_Vector3d, this function creates and returns a negated
 * (i.e. flipped) version of that vector.
 *
 * @param v Pointer to PSMRTS_Vector3d.
 * @return PSMRTS_Vector3d Input vector with negated coordinates.
 */
PSMRTS_Vector3d psmrts_negate( const PSMRTS_Vector3d *v ) {
  PSMRTS_Vector3d v3d = { -v->x, -v->y, -v->z };
  return ( v3d );
}

/**
 * @brief psmrts_subtract - Subtracts two vectors. Result is in a new vector.
 *
 * Given two PSMRTS_Vector3d objects, this function subtracts one from the other
 * and returns the result in a new vector.
 *
 * @param v1 Pointer to 1st PSMRTS_Vector3d.
 * @param v2 Pointer to 2nd PSMRTS_Vector3d.
 * @return PSMRTS_Vector3d Difference of 1st and 2nd input vectors.
 */
PSMRTS_Vector3d psmrts_subtract( const PSMRTS_Vector3d *v1,
                                 const PSMRTS_Vector3d *v2 ) {
  PSMRTS_Vector3d v3d = { v1->x-v2->x, v1->y-v2->y, v1->z-v2->z };
  return ( v3d );
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
 * the lookdir is converted to a unit vector and originates from the observer
 * location toward the target body.
 * 
 * It is the responsibility of the caller to check for valid pointer return.
 * .
 * @param observer Pointer to vector defining observer position (km); i.e. ray
 *                 origin.
 * @param lookdir  Pointer to vector defining direction (km) from observer to
 *                 trace.
 * @return Pointer to the resulting PSMRTS_RayTrace object.
 */
PSMRTS_RayTrace *psmrts_create_ray( const PSMRTS_Vector3d *observer,
                                    const PSMRTS_Vector3d *lookdir ) {
  return ( new PSMRTS_RayTrace( vector_to_eigen( *observer ),
                                vector_to_eigen( *lookdir ) ) );
}

/**  TBD: HOW DO INPUT AND OUTPUT RAYS DIFFER? Kris says they are the same ray,
 *   but the output ray will have a lot more content (WHAT CONTENT?).
 *   SHOULD THIS HAVE A MORE DESCRIPTIVE NAME?
 *
 * @brief psmrts_ray_trace - Creates a PSMRTS ray trace object
 *
 * Given PSMRTS_RayTrace and PSMRTS_Tracer objects, this function creates a
 * new PSMRTS ray trace object that can be used to trace on a shape. The output
 * ray is identical to the input ray except it will contain more content as
 * provided by the PSMRTS_Tracer.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 *
 * @param ray Pointer to PSMRTS_RayTrace object.
 * @param tracer  Pointer to PSMRTS_Tracer object.
 * @return Pointer to resulting PSMRTS_RayTrace object with updated content.
 */
PSMRTS_RayTrace *psmrts_ray_trace( PSMRTS_RayTrace *ray,
                                   const PSMRTS_Tracer *tracer ) {
  tracer->ray_trace( ray->trace().observer(), ray->trace().lookdir(),
                     ray->trace() );
  return ( ray );
}

/**
 * @brief psmrts_ray_trace_v - Creates a PSMRTS ray trace object.
 *
 * Given an ellipsoid tracer plus observer position and look direction
 * vectors, this function creates a PSMRTS ray trace object that can be
 * used to trace on a shape.
 *
 * The observer position is a vector from the shape origin to its body-fixed
 * position relative to the target body origin.
 *
 * The lookdir is converted to unit vector and eminates from the observer
 * location toward the target body.
 *
 * The ellipsoid tracer object ... TBD.
 *
 * It is up to the caller to check for valid pointer return.
 *
 * @param observer Pointer to 3d vector defining observer position (km), i.e.
 *                 the ray origin.
 * @param lookdir Pointer to 3d vector defining direction (km) vector from
 *                observer to trace.
 * @param ellipsoid Pointer to ellipsoid PSMRTS_Tracer object.
 * @return Pointer to the resulting PSMRTS_RayTrace object.
 */
PSMRTS_RayTrace *psmrts_ray_trace_v( const PSMRTS_Vector3d *observer,
                                     const PSMRTS_Vector3d *lookdir,
                                     const PSMRTS_Tracer *ellipsoid ) {
  return ( psmrts_ray_trace( psmrts_create_ray( observer, lookdir ),
                             ellipsoid ) );
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
    return ( ray->trace().hasHit() ?  PSMRTS_TRUE : PSMRTS_FALSE );
}

/* WHAT IF RAY DOESN'T HAVE INTERCEPT? */
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

/* WHAT IF RAY DOESN'T HAVE INTERCEPT? */
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

/* WHAT IF RAY DOESN'T HAVE INTERCEPT? */
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

/* WHAT IF RAY DOESN'T HAVE INTERCEPT? */
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
double psmrts_ray_intercept_radius( const PSMRTS_RayTrace *ray) {
    return ( ray->trace().radius() );
}

/* WHAT IF RAY DOESN'T HAVE INTERCEPT? */
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
 */double psmrts_ray_intercept_slant_distance( const PSMRTS_RayTrace *ray) {
    return ( ray->trace().slant_distance() );
}

/* WHAT IF ONE OR BOTH RAYS DON'T HAVE INTERCEPT? */
/**
 * @brief psrmrts_ray2ray_distance - Returns the distance between two
 *        ray trace surface intercepts, if they exist.
 *
 * Given two PSMRTS_RayTrace objects, this function returns the distance
 * between their surface intercept points, if they exists.
 *
 * @param ray1 Pointer to 1st PSMRTS_RayTrace object.
 * @param ray2 Pointer to 2nd PSMRTS_RayTrace object.
 * @return double Distance between input ray surface intercepts, if they exist.
 */
double psrmrts_ray2ray_distance( const PSMRTS_RayTrace *ray1,
                                 const PSMRTS_RayTrace *ray2 ) {
    return ( ray1->trace().distance(ray2->trace()) );
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
    return ( psmrts::PsmrtsRayTrace::separation_angle(vector_to_eigen(*v1),
                                                      vector_to_eigen(*v2)));
}

/* what if one or both rays don't have intercept? */
/**
 * @brief psrmrts_isNear - Returns true/false if the distance between
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
 * @return bool True/false if distance is/isn't within the given tolerance.
 */
bool psrmrts_isNear( const PSMRTS_RayTrace *ray1,
                     const PSMRTS_RayTrace *ray2,
                     const double tolerance_km ) {
    return ( ray1->trace().isNear(ray2->trace()) );
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
    return ( ray1->trace().incidence(ray2->trace()) );
}

/* WHAT IF THE RAY DOESN'T HAVE AN INTERCEPT? */
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
    return ( ray1->trace().phase(ray2->trace()) );
}

/* Photometric Tracer methods */

/**
 * @brief psmrts_create_photometric_ray - Creates a PSMRTS photometric ray
 *        trace object.
 *
 * Given vectors describing observer position and look direction, this
 * function creates a PSMRTS ray trace object that can be used to trace
 * on a shape.
 *
 * The observer position is a vector from the shape origin to its body-fixed
 * position relative to the target body origin.
 *
 * the lookdir is converted to a unit vector and originates from the observer
 * location toward the target body.
 *
 * It is the responsibility of the caller to check for valid pointer return.
 * .
 * @param observer Pointer to 3d vector defining observer position (km), i.e.
 *                 the ray origin.
 * @param lookdir  Pointer to 3d vector defining direction (km) vector from
 *                 observer to trace.
 * @return PSMRTS_PhotometricRayTrace Pointer to resulting photometric ray
 *                                    trace.
 */
PSMRTS_PhotometricRayTrace *psmrts_create_photometric_ray( const PSMRTS_Vector3d *observer,
                                                           const PSMRTS_Vector3d *lookdir,
                                                           const PSMRTS_Vector3d *sunpos) {
    return ( new psmrts::PRQPhotometricTrace( vector_to_eigen( *observer ),
                                              vector_to_eigen( *lookdir ),
                                              vector_to_eigen( *sunpos )) );
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
double psmrts_photo_incidence(const PSMRTS_PhotometricRayTrace *photometricTrace ) {
    return (photometricTrace->incidence() );
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
double psmrts_photo_emission(const PSMRTS_PhotometricRayTrace *photometricTrace ) {
    return (photometricTrace->emission() );
}

/**
 * @brief psmrts_photometric_phase - Returns the photometric phase angle (in
 *        radians) subtended between two vectors from a common surface point to
 *        1) an observer and 2) the sun.
 *
 * Given a PSMRTS_PhotometricRayTrace object, this function returns the
 * photometric phase angle (in radians) subtended between two vectors from a
 * common surface point to 1) an observer and 2) the sun.
 *
 * @param photoTrace1 Pointer to 1st PSMRTS_PhotometricRayTrace object.
 * @param photoTrace2 Pointer to 2nd PSMRTS_PhotometricRayTrace object.
 * @return double Photometric phase angle between input trace objects (in
 *                radians).
 */
double psmrts_photo_phase( const PSMRTS_PhotometricRayTrace *photoTrace1,
                           const PSMRTS_PhotometricRayTrace *photoTrace2 ) {
    return ( photoTrace1->observer_trace().phase(photoTrace2->observer_trace()) );
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
PSMRTS_Vector3d psmrts_lonlatrad_to_xyz( const PSMRTS_Vector3d *v ) {
  return ( eigen_to_vector(psmrts::latlonrad_to_xyz_d(vector_to_eigen(*v))) );
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
PSMRTS_Vector3d psmrts_xyz_to_lonlatrad( const PSMRTS_Vector3d *v ) {
    return ( eigen_to_vector(psmrts::xyz_to_latlonrad_d(vector_to_eigen(*v))) );
}

// TBD
// PSMRTS_ShapeTracer *psmrts_load_shape( const char *shape, const char *tracer );

// void psmrts_lonlat_to_xyz( const double longitude_d, const double latitude_d, const double radius_km, double xyz[3] ) {

// }

// TBD
// void psmrts_vector_scale( const double v[3], const double scale, double vout[3] );
// PSMRTS_RayTrace *PSMRTS_RayTrace_trace( PSMRTS_ShapeTracer *tracer, const double scpos[3], const double lookdir[3] );

/**
 * @brief psmrts_tracer_valid - Determines validity of a PSMRTS_ShapeTracer
 *        object.
 *
 * Given a PSMRTS_PhotometricRayTrace object, this function returns the
 * photometric phase angle (in radians) subtended between two vectors from a
 * common surface point to 1) an observer and 2) the sun.
 *
 * @param tracer Pointer to PSMRTS_ShapeTracer object.
 * @return bool Validity of input PSMRTS_ShapeTracer.
 */
PSMRTS_BOOL psmrts_tracer_valid( const PSMRTS_ShapeTracer *tracer ) {
  return ( ( 0 != tracer ) ?  PSMRTS_TRUE : PSMRTS_FALSE );
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
void psmrts_free_shapetracer( PSMRTS_ShapeTracer *stracer ) {
  delete stracer;
}

/**
 * @brief psmrts_free - Frees memory allocated to input
 *                      PSMRTS_PriorityTracer pointer.
 *
 * This function frees memory allocated to the input PSMRTS_PriorityTracer
 * pointer.
 *
 * @param ptracer Pointer to PSMRTS_PriorityTracer.
 * @return void
 */
void psmrts_free( PSMRTS_PriorityTracer *ptracer ) {
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

}
