

#ifndef PsmrtsBulletAllHitsRayCallback_hpp
#define PsmrtsBulletAllHitsRayCallback_hpp

#include "PsmrtsBulletClosestRayCallback.hpp"

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

namespace psmrts::bullet {

  /**
   * Bullet ray tracing callback to return all intersections along a ray's path.
   * 
   * @author 2017-03-17 Kris Becker 
   * @internal 
   *   @history 2017-03-17  Kris Becker  Original Version
   */
  class BulletAllHitsRayCallback : public btCollisionWorld::AllHitsRayResultCallback {
    public:

      /**
       * Default constructor. The ray beginning and end default to the origin.
       * The intersections vector defaults to empty.
       */
      PsmrtsBulletAllHitsRayCallback() : 
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
      PsmrtsBulletAllHitsRayCallback(const btVector3 &observer, 
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
       * Destroy this callback.
       */
      ~PsmrtsBulletAllHitsRayCallback() { }


      /**
       * Checks if the callback is valid/has a valid intersection.
       * 
       * @return @b bool If there is a valid intersection.
       * 
       * @see ClosestRayResultCallback::hasHit()
       */
      inline bool isValid() const {
        return ( hasHit() );
      }


      /**
       * Returns the number of intersections found.
       * 
       * @return @b int The number of stored intersections in this callback.
       */
      inline int size() const {
        return ( m_rayHits.size() );
      }


      /**
       * Return the beginning of the ray.
       * 
       * @return @b btVector3 The beginning of the ray.
       */
      inline btVector3 observer() const {
        return ( AllHitsRayResultCallback::m_rayFromWorld );
      }


      /**
       * Return the end of the ray.
       * 
       * @return @b btVector3 The end of the ray.
       */
      inline btVector3 lookdir() const {
        return ( AllHitsRayResultCallback::m_rayToWorld );
      }


      /**
       * Return a callback for the intersection at a given index.
       * 
       * @param index The index of the intersection.
       * 
       * @return @b BulletClosestRayCallback The callback for the intersection
       */
      inline const BulletClosestRayCallback &hit(const int &index) const {
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
      inline btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace) {
        btCollisionWorld::AllHitsRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
        m_rayHits.push_back(BulletClosestRayCallback(observer(), lookdir(), *this, rayResult, normalInWorldSpace));
        return (rayResult.m_hitFraction);
      }
  };
  
} // namespace Isis


#endif 


