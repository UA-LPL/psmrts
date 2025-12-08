

#include "PsmrtsBulletAllHitsRayCallback.hpp"
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

namespace psmrts::bullet {

  /**
   * Default constructor. The ray beginning and end default to the origin.
   * The intersections vector defaults to empty.
   */
  PsmrtsBulletAllHitsRayCallback::PsmrtsBulletAllHitsRayCallback() : 
    btCollisionWorld::AllHitsRayResultCallback(btVector3(0,0,0), btVector3(0,0,0)),
    m_rayHits() {
    m_flags = (btTriangleRaycastCallback::kF_KeepUnflippedNormal |
                btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest);
  }


  /**
   * Construct a callback with a ray start and ray end.
   * 
   * @param observer The beginning of the ray.
   * @param lookdir The end of the ray.
   * @param cullBackfacers If back facing intersections should be culled.
   */
  PsmrtsBulletAllHitsRayCallback::PsmrtsBulletAllHitsRayCallback(const btVector3 &observer, 
                                  const btVector3 &lookdir,
                                  const bool cullBackfacers) : 
                                  btCollisionWorld::AllHitsRayResultCallback(observer, lookdir),
                                  m_rayHits() {
    m_flags = (btTriangleRaycastCallback::kF_KeepUnflippedNormal |
                btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest);
    if ( cullBackfacers ) {
      m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
    }
  }

  /**
   * Checks if the callback is valid/has a valid intersection.
   * 
   * @return @b bool If there is a valid intersection.
   * 
   * @see ClosestRayResultCallback::hasHit()
   */
  bool PsmrtsBulletAllHitsRayCallback::isValid() const {
    return ( this->hasHit() );
  }


  /**
   * Returns the number of intersections found.
   * 
   * @return @b int The number of stored intersections in this callback.
   */
  int PsmrtsBulletAllHitsRayCallback::size() const {
    return ( m_rayHits.size() );
  }


  /**
   * Return the beginning of the ray.
   * 
   * @return @b btVector3 The beginning of the ray.
   */
  btVector3 PsmrtsBulletAllHitsRayCallback::observer() const {
    return ( AllHitsRayResultCallback::m_rayFromWorld );
  }


  /**
   * Return the end of the ray.
   * 
   * @return @b btVector3 The end of the ray.
   */
  btVector3 PsmrtsBulletAllHitsRayCallback::lookdir() const {
    return ( AllHitsRayResultCallback::m_rayToWorld );
  }


  /**
   * Return a callback for the intersection at a given index.
   * 
   * @param index The index of the intersection.
   * 
   * @return @b BulletClosestRayCallback The callback for the intersection
   */
  const PsmrtsBulletClosestRayCallback &PsmrtsBulletAllHitsRayCallback::hit(const int &index) const {
    btAssert( index >= 0 );
    btAssert( index < size() );
    return ( m_rayHits[index] );
  }


  /**
   * Add a local intersection result to this callback during ray casting.
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
  btScalar PsmrtsBulletAllHitsRayCallback::addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace) {
    btCollisionWorld::AllHitsRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
    m_rayHits.push_back(PsmrtsBulletClosestRayCallback(observer(), lookdir(), *this, rayResult, normalInWorldSpace));
    return (rayResult.m_hitFraction);
  }

} // namespace Isis
