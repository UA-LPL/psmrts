#ifndef PsmrtsBulletAllHitsRayCallback_hpp
#define PsmrtsBulletAllHitsRayCallback_hpp

#include "PsmrtsBulletClosestRayCallback.hpp"
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

namespace psmrts::bullet {

  /**
   * Bullet ray tracing callback to return all intersections along a ray's path.
   * 
   * @author 2017-03-17 Kris Becker 
   * @internal 
   *   @history 2017-03-17  Kris Becker  Original Version
   */
  class PsmrtsBulletAllHitsRayCallback : public btCollisionWorld::AllHitsRayResultCallback {
    public:

      /**
       * Default constructor. The ray beginning and end default to the origin.
       * The intersections vector defaults to empty.
       */
      PsmrtsBulletAllHitsRayCallback();


      /**
       * Construct a callback with a ray start and ray end.
       * 
       * @param observer The beginning of the ray.
       * @param lookdir The end of the ray.
       * @param cullBackfacers If back facing intersections should be culled.
       */
      PsmrtsBulletAllHitsRayCallback(const btVector3 &observer, 
                                      const btVector3 &lookdir,
                                      const bool cullBackfacers = true);


      /**
       * Destroy this callback.
       */
      virtual ~PsmrtsBulletAllHitsRayCallback() = default;


      /**
       * Checks if the callback is valid/has a valid intersection.
       * 
       * @return @b bool If there is a valid intersection.
       * 
       * @see ClosestRayResultCallback::hasHit()
       */
      bool isValid() const;


      /**
       * Returns the number of intersections found.
       * 
       * @return @b int The number of stored intersections in this callback.
       */
      int size() const;

      /**
       * Return the beginning of the ray.
       * 
       * @return @b btVector3 The beginning of the ray.
       */
      btVector3 observer() const;


      /**
       * Return the end of the ray.
       * 
       * @return @b btVector3 The end of the ray.
       */
      btVector3 lookdir() const;


      /**
       * Return a callback for the intersection at a given index.
       * 
       * @param index The index of the intersection.
       * 
       * @return @b PsmrtsBulletClosestRayCallback The callback for the intersection
       */
      const PsmrtsBulletClosestRayCallback &hit(const int &index) const;


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
      btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace);

    protected:
      std::vector<PsmrtsBulletClosestRayCallback> m_rayHits;
  };
  
} // namespace Isis


#endif 


