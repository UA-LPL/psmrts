#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <Eigen/Geometry>

#include "PsmrtsBulletClosestRayCallback.hpp"
#include "BulletSystemModel.hpp"

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

namespace psmrts::bullet {

  PsmrtsBulletClosestRayCallback::PsmrtsBulletClosestRayCallback() : 
    btCollisionWorld::ClosestRayResultCallback(btVector3(0,0,0), btVector3(0,0,0)), 
                            m_point(0.0, 0.0, 0.0), m_normal(0.0, 0.0, 0.0),
                            m_triangleIndex(-1), m_partId(-1) {

    // Set default ray tracing flags
    m_flags = defaultFlags();
  }


  /**
   * Construct a callback from another callback, an intersection point, and a
   * surface normal.
   * 
   * @param result The callback to to construct a copy of. The intersection
   *               point and normal will not be copied.
   * @param point The intersection point to store.
   * @param normal The surface normal to store.
   */
  PsmrtsBulletClosestRayCallback::PsmrtsBulletClosestRayCallback(const PsmrtsBulletClosestRayCallback &result, 
                                  const btVector3 &point,
                                  const btVector3 &normal ) :
                                  btCollisionWorld::ClosestRayResultCallback( result ) {
    m_point = point;
    m_normal = normal;
    return;
  }


  /**
   * Construct a callback from a ray start and end.
   * 
   * @param observer The beginning of the ray.
   * @param lookdir The end of the ray.
   */
  PsmrtsBulletClosestRayCallback::PsmrtsBulletClosestRayCallback( const Eigen::Vector3d &observer, 
                                  const Eigen::Vector3d &lookdir) : 
                                  btCollisionWorld::ClosestRayResultCallback(PsmrtsBulletClosestRayCallback::toBtVector( observer ),
                                                                              PsmrtsBulletClosestRayCallback::toBtVector( lookdir ) ), 
                                  m_point(0.0, 0.0, 0.0), m_normal(0.0, 0.0, 0.0),
                                  m_triangleIndex(-1), m_partId(-1) { 

    // Set default ray tracing flags
    m_flags = defaultFlags();
  }


  /**
   * Construct a callback from a ray start and end.
   * 
   * @param observer The beginning of the ray.
   * @param lookdir The end of the ray.
   */
  PsmrtsBulletClosestRayCallback::PsmrtsBulletClosestRayCallback( const btVector3 &observer, 
                                  const btVector3 &lookdir) : 
                                  btCollisionWorld::ClosestRayResultCallback(observer, lookdir), 
                                  m_point(0.0, 0.0, 0.0), m_normal(0.0, 0.0, 0.0),
                                  m_triangleIndex(-1), m_partId(-1) { 

    // Set default ray tracing flags
    m_flags = defaultFlags();
  }


  /**
   * @brief This constructor is used to create a single ray hit 
   *  
   * This constructor is intended to be used in a multi-ray hit environment such 
   * as the BulletAllRayHitsRayCallback object to provide the same features 
   * provided in this single hit object. 
   * 
   * @author 2017-03-17 Kris Becker 
   * 
   * @param observer The beginning of the ray.
   * @param lookdir The end of the ray.
   * @param source The original callback this is being created from.
   * @param rayResult The local intersection result.
   * @param normalInWorldSpace Is the normal in the local result in the local coordinate system
   *                           or the world coordinate system?
   */
  PsmrtsBulletClosestRayCallback::PsmrtsBulletClosestRayCallback( const btVector3 &observer, 
                                  const btVector3 &lookdir,
                                  const btCollisionWorld::RayResultCallback &source, 
                                  btCollisionWorld::LocalRayResult &rayResult, 
                                  bool normalInWorldSpace) :
                                  btCollisionWorld::ClosestRayResultCallback(observer, lookdir), 
                                  m_point(0.0, 0.0, 0.0), m_normal(0.0, 0.0, 0.0),
                                  m_triangleIndex(-1), m_partId(-1)  {
      copyRayResult(*this, source);
      addSingleResult(rayResult, normalInWorldSpace);
  }

  /**
   * Checks if the callback is valid/has a valid intersection.
   * 
   * @return @b bool If there is a valid intersection.
   * 
   * @see ClosestRayResultCallback::hasHit()
   */
  bool PsmrtsBulletClosestRayCallback::isValid() const {
    return ( hasHit() );
  }


  /**
   * Return the beginning of the ray.
   * 
   * @return @b btVector3 The beginning of the ray.
   */
  btVector3 PsmrtsBulletClosestRayCallback::observer() const {
    return ( m_rayFromWorld );
  }


  /**
   * Return the end of the ray.
   * 
   * @return @b btVector3 The end of the ray.
   */
  btVector3 PsmrtsBulletClosestRayCallback::lookdir() const {
    return ( m_rayToWorld );
  }

  /** Convert Bullet vector to Eigen vector */
  Eigen::Vector3d PsmrtsBulletClosestRayCallback::toStdVector( const btVector3 &v ) {
    return ( Eigen::Vector3d( { v[0], v[1], v[2] } ) );
  }

  /** Convert Bullet vector to Eigen vector */
  btVector3 PsmrtsBulletClosestRayCallback::toBtVector( const Eigen::Vector3d &v ) {
    return ( btVector3( v[0], v[1], v[2] ) );
  }


  /**
   * Return the intersection hit fraction or fractional distance along the ray
   * of the intersection. This is where along the ray the intersection was
   * found. If the hit fraction is 0, then the intersection was found at the
   * very beginning of the ray. If the hit fraction is 1, then the intersection
   * was found at the very end of the ray. The intersection point can be
   * calculates as:
   * 
   * (1 - hitFraction) * rayStart + hitFraction * rayEnd
   * 
   * @return @btScalar The closest intersection hit fraction.
   */
  btScalar PsmrtsBulletClosestRayCallback::fraction() const {
    return ( m_closestHitFraction );
  }


  /**
   * Return the intersection point, if one exists. This is the intersect point
   * along the look vector. So the body fixed vector from body origin to surface
   * is point() - observer().
   * 
   * @return @b btVector3 The intersection point in body fixed (x, y, z) kilometers.
   */
  btVector3 PsmrtsBulletClosestRayCallback::point() const {
    if ( hasHit() ) { 
      return ( m_point );
    }

    return ( btVector3( null(), null(), null() ) );
  }


  /** Return the surface intercept from origin to point() */
  btVector3 PsmrtsBulletClosestRayCallback::xyz() const {
    // return ( this->point() + this->observer() );
    return ( this->point() );
  }


  /**
   * Return the local surface normal at the intersection, if an intersection exists.
   * 
   * @return @b btVector3 The local surface normal in body fixed (x, y, z).
   */
  btVector3 PsmrtsBulletClosestRayCallback::normal() const {
    if ( hasHit() ) {
      return ( m_normal );
    }

    return ( btVector3( psmrts::null(), psmrts::null(), psmrts::null() ) );
  }


  /**
   * Return the 0-based index of the intersected triangle.
   * 
   * @return @b int The 0-based index of the intersected triangle.
   */
  int PsmrtsBulletClosestRayCallback::triangleIndex() const {
    return ( m_triangleIndex );
  }


  /**
   * Return the Bullet ID of the intersected collision object.
   * 
   * @return @b int The Bullet ID of the intersected collision object.
   */
  int PsmrtsBulletClosestRayCallback::partId() const {
    return ( m_partId );
  }

  /**
   * Return the default ray cast flags. The flags come from the EFlags
   * enumeration in the Bullet class btTriangleRaycastCallback.
   * 
   * @return @b int The bitwise default flags.
   */
  unsigned int PsmrtsBulletClosestRayCallback::defaultFlags() const {
    return ( (btTriangleRaycastCallback::kF_FilterBackfaces | 
              btTriangleRaycastCallback::kF_KeepUnflippedNormal |
              btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest) );
  }


  /**
   * Add a local intersection result to this callback during ray casting.
   * Because this callback  only stores the single closest result, this
   * overwrites any currently stored result.
   * 
   * @note This method is called automatically by btCollisionWorld::rayTest.
   * 
   * @param rayResult The local intersection result for the ray cast.
   * @param normalInWorldSpace If the normal stored in the local result is in
   *                           local coordinates or world coordinates.
   * 
   * @return @b btScalar The hit fraction, fractional distance along the ray,
   *                     of the intersections
   */
  btScalar PsmrtsBulletClosestRayCallback::addSingleResult( btCollisionWorld::LocalRayResult &rayResult,
                                    bool normalInWorldSpace ) {
    ZoneScopedN( "psmrts::PsmrtsBulletClosestRayCallback::addSingleResult" );
    btScalar hitFraction = ClosestRayResultCallback::addSingleResult( rayResult, normalInWorldSpace );

    m_point  = m_hitPointWorld;
    m_normal = m_hitNormalWorld;  // rayResult.m_hitNormalLocal;

    // Triangle information
    m_triangleIndex = rayResult.m_localShapeInfo->m_triangleIndex;
    m_partId        = rayResult.m_localShapeInfo->m_shapePart;
    return ( hitFraction );
  }

  /**
   * Easy way to copy one callback into another.
   * 
   * @param dest The callback to copy over.
   * @param source The callback to copy from.
   */
  void PsmrtsBulletClosestRayCallback::copyRayResult(btCollisionWorld::RayResultCallback &dest, 
                            const btCollisionWorld::RayResultCallback &source) {
    ZoneScopedN( "psmrts::PsmrtsBulletClosestRayCallback::copyRayResult" );
    dest = source;
    return;
  }
} // namespace psmrts::bullet
