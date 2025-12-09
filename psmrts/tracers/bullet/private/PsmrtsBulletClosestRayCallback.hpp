#ifndef PsmrtsBulletClosestRayCallback_hpp
#define PsmrtsBulletClosestRayCallback_hpp

#include <Eigen/Geometry>

#include "BulletSystemModel.hpp"
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>

namespace psmrts::bullet {

  class PsmrtsBulletClosestRayCallback : public btCollisionWorld::ClosestRayResultCallback {
    public:
      PsmrtsBulletClosestRayCallback();

      /**
       * Construct a callback from another callback, an intersection point, and a
       * surface normal.
       * 
       * @param result The callback to to construct a copy of. The intersection
       *               point and normal will not be copied.
       * @param point The intersection point to store.
       * @param normal The surface normal to store.
       */
      PsmrtsBulletClosestRayCallback(const PsmrtsBulletClosestRayCallback &result, 
                                     const btVector3 &point,
                                     const btVector3 &normal );


      /**
       * Construct a callback from a ray start and end.
       * 
       * @param observer The beginning of the ray.
       * @param lookdir The end of the ray.
       */
      PsmrtsBulletClosestRayCallback( const Eigen::Vector3d &observer, 
                                      const Eigen::Vector3d &lookdir);


      /**
       * Construct a callback from a ray start and end.
       * 
       * @param observer The beginning of the ray.
       * @param lookdir The end of the ray.
       */
      PsmrtsBulletClosestRayCallback( const btVector3 &observer, 
                                      const btVector3 &lookdir);


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
      PsmrtsBulletClosestRayCallback( const btVector3 &observer, 
                                      const btVector3 &lookdir,
                                      const btCollisionWorld::RayResultCallback &source, 
                                      btCollisionWorld::LocalRayResult &rayResult, 
                                      bool normalInWorldSpace);


      /**
       * Destory a callback.
       */
      virtual ~PsmrtsBulletClosestRayCallback() = default;


      /**
       * Checks if the callback is valid/has a valid intersection.
       * 
       * @return @b bool If there is a valid intersection.
       * 
       * @see ClosestRayResultCallback::hasHit()
       */
      bool isValid() const;


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

      /** Convert Bullet vector to Eigen vector */
      static Eigen::Vector3d toStdVector( const btVector3 &v );

      /** Convert Bullet vector to Eigen vector */
      static btVector3 toBtVector( const Eigen::Vector3d &v );


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
      btScalar fraction() const;


      /**
       * Return the intersection point, if one exists. This is the intersect point
       * along the look vector. So the body fixed vector from body origin to surface
       * is point() - observer().
       * 
       * @return @b btVector3 The intersection point in body fixed (x, y, z) kilometers.
       */
      btVector3 point() const;

      /** Return the surface intercept from origin to point() */
      btVector3 xyz() const;


      /**
       * Return the local surface normal at the intersection, if an intersection exists.
       * 
       * @return @b btVector3 The local surface normal in body fixed (x, y, z).
       */
      btVector3 normal() const;

      /**
       * Return the 0-based index of the intersected triangle.
       * 
       * @return @b int The 0-based index of the intersected triangle.
       */
      int triangleIndex() const;


      /**
       * Return the Bullet ID of the intersected collision object.
       * 
       * @return @b int The Bullet ID of the intersected collision object.
       */
      int partId() const;


      /** 
       * Returns the distance from the intersection point to the beginning of the ray.
       * 
       * @return @b btScalar Distance between observer and point in kilometers. If
       *                     no intersection exists, then DBL_MAX is returned.
       */
      btScalar distance() const;


      /** 
       * Returns the distance from the intersection point of this callback to the
       * intersection point of another callback.
       *  
       * @param other The other callback to find the distance to.
       * 
       * @return @b btScalar Distance between the intersection point and the other
       *                     callback's intersection point in kilometers. If this
       *                     callback or the other callback does not have an
       *                     intersection, then DBL_MAX is returned.
       */
      btScalar distance(const PsmrtsBulletClosestRayCallback &other) const;


      /** 
       * Returns the distance from the intersection point to another point.
       * 
       * @param other The point to calculate the distance to.
       * 
       * @return @b btScalar Distance between intersection and point in kilometers.
       *                     If no intersection exists, then DBL_MAX is returned.
       */
      btScalar distance(const btVector3 &other) const;


      /**
       * Equality operator to check if this callback is equivalent to another callback.
       * 
       * @param other The callback to compare with.
       * 
       * @return @b bool If both callbacks are valid and intersected the same
       *                 triangle on the same collision body.
       */
      bool operator==(const PsmrtsBulletClosestRayCallback &other) const;

      /**
       * @breif Return pointer to target shape 
       *  
       * This method assumes the creators have properly set the user pointer in (at 
       * least) the btCollisionObject. 
       * 
       * @author 2017-03-17 Kris Becker
       * 
       * @return const BulletTargetShape* 
       */
      const void *body() const;

      /**
       * Return the default ray cast flags. The flags come from the EFlags
       * enumeration in the Bullet class btTriangleRaycastCallback.
       * 
       * @return @b int The bitwise default flags.
       */
        unsigned int defaultFlags() const;

    protected:
      btVector3 m_point;         /**! The intersection point in body fixed (x, y, z) kilometers. */
      btVector3 m_normal;        /**! The local surface normal at the intersection point in
                                       body fixed (x, y, z). */
      int       m_triangleIndex; /**! The 0-based index of the intersected triangle. */
      int       m_partId;        /**! The Bullet ID of the intersected collision object. */
        

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
      btScalar addSingleResult( btCollisionWorld::LocalRayResult &rayResult,
                                       bool normalInWorldSpace );

    private:
      /**
       * Easy way to copy one callback into another.
       * 
       * @param dest The callback to copy over.
       * @param source The callback to copy from.
       */
      void copyRayResult(btCollisionWorld::RayResultCallback &dest, 
                                const btCollisionWorld::RayResultCallback &source);
  };

} // namespace psmrts::bullet

#endif


