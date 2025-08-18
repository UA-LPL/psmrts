#ifndef PsmrtsPsmrtsBulletClosestRayCallback_hpp
#define PsmrtsPsmrtsBulletClosestRayCallback_hpp

#include <string>
#include <memory>
#include <exception>

#include <Eigen/Geometry>

#include "BulletSystemModel.hpp"
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

namespace psmrts::bullet {

  class PsmrtsBulletClosestRayCallback : public btCollisionWorld::ClosestRayResultCallback {
    public:
      PsmrtsBulletClosestRayCallback() : 
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
      PsmrtsBulletClosestRayCallback(const PsmrtsBulletClosestRayCallback &result, 
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
      PsmrtsBulletClosestRayCallback( const Eigen::Vector3d &observer, 
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
      PsmrtsBulletClosestRayCallback( const btVector3 &observer, 
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
      PsmrtsBulletClosestRayCallback( const btVector3 &observer, 
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
       * Destory a callback.
       */
      ~PsmrtsBulletClosestRayCallback() { }


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
       * Return the beginning of the ray.
       * 
       * @return @b btVector3 The beginning of the ray.
       */
      inline btVector3 observer() const {
        return ( m_rayFromWorld );
      }


      /**
       * Return the end of the ray.
       * 
       * @return @b btVector3 The end of the ray.
       */
      inline btVector3 lookdir() const {
        return ( m_rayToWorld );
      }

      /** Convert Bullet vector to Eigen vector */
      inline static Eigen::Vector3d toStdVector( const btVector3 &v ) {
        return ( Eigen::Vector3d( { v[0], v[1], v[2] } ) );
      }

      /** Convert Bullet vector to Eigen vector */
      inline static btVector3 toBtVector( const Eigen::Vector3d &v ) {
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
      inline btScalar fraction() const {
        return ( m_closestHitFraction );
      }


      /**
       * Return the intersection point, if one exists. This is the intersect point
       * along the look vector. So the body fixed vector from body origin to surface
       * is point() - observer().
       * 
       * @return @b btVector3 The intersection point in body fixed (x, y, z) kilometers.
       */
      inline btVector3 point() const {
        if ( hasHit() ) { 
          return ( m_point );
        }

        return ( btVector3( null(), null(), null() ) );
      }


      /** Return the surface intercept from origin to point() */
      inline btVector3 xyz() const {
        // return ( this->point() + this->observer() );
        return ( this->point() );
      }


      /**
       * Return the local surface normal at the intersection, if an intersection exists.
       * 
       * @return @b btVector3 The local surface normal in body fixed (x, y, z).
       */
      inline btVector3 normal() const {
        if ( hasHit() ) {
          return ( m_normal );
        }

        return ( btVector3( null(), null(), null() ) );
      }


      /**
       * Return the 0-based index of the intersected triangle.
       * 
       * @return @b int The 0-based index of the intersected triangle.
       */
      inline int triangleIndex() const {
        return ( m_triangleIndex );
      }


      /**
       * Return the Bullet ID of the intersected collision object.
       * 
       * @return @b int The Bullet ID of the intersected collision object.
       */
      inline int partId() const {
        return ( m_partId );
      }


      /** 
       * Returns the distance from the intersection point to the beginning of the ray.
       * 
       * @return @b btScalar Distance between observer and point in kilometers. If
       *                     no intersection exists, then DBL_MAX is returned.
       */
      inline btScalar distance() const {
        if ( !isValid() ) return (DBL_MAX);
        return ( observer().distance( point() ) );
      }


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
      inline btScalar distance(const PsmrtsBulletClosestRayCallback &other) const {
        if ( !(isValid() && other.isValid()) ) return (DBL_MAX);
        return ( point().distance( other.point() ));
      }


      /** 
       * Returns the distance from the intersection point to another point.
       * 
       * @param other The point to calculate the distance to.
       * 
       * @return @b btScalar Distance between intersection and point in kilometers.
       *                     If no intersection exists, then DBL_MAX is returned.
       */
      inline btScalar distance(const btVector3 &other) const {
        if ( !isValid() ) return (DBL_MAX);
        return ( point().distance(other) );
      }


      /**
       * Equality operator to check if this callback is equivalent to another callback.
       * 
       * @param other The callback to compare with.
       * 
       * @return @b bool If both callbacks are valid and intersected the same
       *                 triangle on the same collision body.
       */
      inline bool operator==(const PsmrtsBulletClosestRayCallback &other) const {
        if ( !(isValid() && other.isValid()) ) return (false);
        if ( triangleIndex() != other.triangleIndex() ) return (false);
        if ( partId() != other.partId() ) return (false);
        return ( true );
      }

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
      inline const void *body() const {
        if ( !isValid() ) return (0);
        return ( m_collisionObject->getUserPointer() );
      }

      /**
       * Return the default ray cast flags. The flags come from the EFlags
       * enumeration in the Bullet class btTriangleRaycastCallback.
       * 
       * @return @b int The bitwise default flags.
       */
        inline unsigned int defaultFlags() const {
          return ( (btTriangleRaycastCallback::kF_FilterBackfaces | 
                    btTriangleRaycastCallback::kF_KeepUnflippedNormal |
                    btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest) );
        }

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
      inline btScalar addSingleResult( btCollisionWorld::LocalRayResult &rayResult,
                                       bool normalInWorldSpace ) {
        btScalar hitFraction = ClosestRayResultCallback::addSingleResult( rayResult, normalInWorldSpace );

        m_point  = m_hitPointWorld;
        m_normal = m_hitNormalWorld;  // rayResult.m_hitNormalLocal;

        // Triangle information
        m_triangleIndex = rayResult.m_localShapeInfo->m_triangleIndex;
        m_partId        = rayResult.m_localShapeInfo->m_shapePart;
        return ( hitFraction );
      }

    private:
      /**
       * Easy way to copy one callback into another.
       * 
       * @param dest The callback to copy over.
       * @param source The callback to copy from.
       */
      inline void copyRayResult(btCollisionWorld::RayResultCallback &dest, 
                                const btCollisionWorld::RayResultCallback &source) {
        dest = source;
        return;
      }
  };

} // namespace psmrts::bullet

#endif


