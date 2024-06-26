/**                                                                       
 * @file                                                                  
 * $Revision$
 * $Date$
 * $Id$
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */

#include "BulletShapeFactory.h"
#include "BulletWorldManager.h"
#include "BulletPrioritizedShapes.h"
#include "BulletTargetShape.h"
#include "IException.h"
#include "PvlKeyword.h"

#include <QMutex>
#include <QMutexLocker>

using namespace std;

namespace Isis {

  /**
   * Default empty constructor.
   */
  BulletPrioritizedShapes::BulletPrioritizedShapes() :  m_name(),
                                                        m_shapes(1),
                                                        m_raytraces(0),
                                                        m_mutex(new QMutex),
                                                        m_debug(false) {

  }

  /** Named construction */
  BulletPrioritizedShapes::BulletPrioritizedShapes(const QString &name) :
                                                   m_name(name),
                                                   m_shapes(),
                                                   m_raytraces(0),
                                                   m_mutex(new QMutex),
                                                   m_debug(false) {


  }

  BulletPrioritizedShapes::BulletPrioritizedShapes(BulletTargetShape *shape) :
                                                   m_name(shape->name()),
                                                   m_shapes(),
                                                   m_raytraces(0),
                                                   m_mutex(new QMutex),
                                                   m_debug(false) {

    add( shape );
  }


  BulletPrioritizedShapes::BulletPrioritizedShapes(const BulletWorldManager &world) :
                                                   m_name(world.name()),
                                                   m_shapes(),
                                                   m_raytraces(0),
                                                   m_mutex(new QMutex),
                                                   m_debug(false)  {
    add( world );

  }

  BulletPrioritizedShapes::BulletPrioritizedShapes(BulletWorldManager *world) :
                                                   m_name(world->name()),
                                                   m_shapes(),
                                                   m_raytraces(0),
                                                   m_mutex(new QMutex),
                                                   m_debug(false)  {
    add( world );
  }


  /**
   * Construct a model with one or more prioritized shapes.
   * 
   * @param shapes List of prioritized Bullet worlds
   */
  BulletPrioritizedShapes::BulletPrioritizedShapes(const BulletShapeLayers &prishapes,
                                                   const QString &name) :
                                                   m_name(name), 
                                                   m_shapes(prishapes),
                                                   m_raytraces(0),
                                                   m_mutex(new QMutex),
                                                   m_debug(false) {
  }

/**
 *  Construct a copy of an existing prioritized shape model 
 * 
 *  Only the list of shapes are copied.
 *  
 * @param prishape Prioritied model to copy/clone
 */
  BulletPrioritizedShapes::BulletPrioritizedShapes(const BulletPrioritizedShapes &pshape) : 
                                                   m_name(pshape.m_name),
                                                   m_shapes(pshape.m_shapes),
                                                   m_raytraces(0),
                                                   m_mutex(new QMutex),
                                                   m_debug(false) {
  }



  /** Need a legit copy constructor because of the scoped QMutex  */
  BulletPrioritizedShapes &BulletPrioritizedShapes::operator=(const BulletPrioritizedShapes &other) {

    if (this != &other) {
      m_name = other.m_name;
      m_shapes = other.m_shapes;
      m_raytraces = 0;
      m_mutex.reset(new QMutex);
      m_debug = other.m_debug;
    }
    return ( *this );
  }


  /**
   * Destroys the BulletPrioritizedShapes.
   */
  BulletPrioritizedShapes::~BulletPrioritizedShapes() { }


  /** Name associated with this model   */
  QString BulletPrioritizedShapes::name() const {
    return ( m_name );
  }

  /**
   * Number of collision objects in the world.
   * 
   * @return @b int The number of shapes in the queue
   */
  int BulletPrioritizedShapes::size() const {
    return ( m_shapes.size() );
  }

  /** Add a Bullet shape to the layers   */
   void BulletPrioritizedShapes::add(BulletTargetShape *shape) {
     add( BulletWorldManager(shape) );
   }

  /** Add a Bullet shape to the layers   */
   void BulletPrioritizedShapes::add(const BulletWorldManager &world) {
     SharedBulletWorld myWorld( new BulletWorldManager( world ) );
     m_shapes.append( myWorld );
   }


  /** Add a Bullet shape to the layers   */
   void BulletPrioritizedShapes::add(BulletWorldManager *world) {
     m_shapes.append( SharedBulletWorld(world) );
   }

   /** Retrieve the indicated model   */
   const BulletWorldManager &BulletPrioritizedShapes::get(const int &index) const {
     btAssert( index >= 0 );
     btAssert( index < m_shapes.size() );
     return ( *m_shapes[index] );
   }

   /** Return the maximum distance found in all the models   */
   double BulletPrioritizedShapes::maximumDistance() const {

     double dmax(0.0);
     for (int i = 0 ; i < size() ; i++) {
       BulletTargetShape *shape = get(i).getTarget();
       double wmax = shape->maximumDistance();
       if (wmax > dmax) dmax = wmax;
     }
     
     return ( dmax );
   }

/**
 * @brief Perform ray casting from a position and a look direction 
 *  
 * Its not clear if mutex locking is needed here for several reasons. Bullet is 
 * thread safe but only if it was compiled with BULLET2_USE_THREAD_LOCKS=ON. The 
 * default is to not turn on thread locking so invoke thread locking here. 
 * However, if its built with threading, it is not clear if thread locking is
 * needed and should to be tested. 
 * 
 * @author 2021-04-20 Kris Becker 
 * 
 * @param         rayStart The origin of the ray
 * @param         rayEnd   The end point of the ray  
 * @param[in,out] results  Ray intersection callback. holds the output results
 *                         of the ray cast. The type of callback determines
 *                         what happens when an intersection is found during
 *                         ray casting.
 * @param      prioritize  Run a prioritized trace through the list, otherwise 
 *                         run through all shapes 
 * 
 * @return @b bool Returns true if any intersections are detected
 * 
 * @see btCollisionWorld::rayTest
 */
  bool BulletPrioritizedShapes::raycast(const btVector3 &rayStart, const btVector3 &rayEnd, 
                                        btCollisionWorld::RayResultCallback &results, 
                                        const bool prioritize ) const {

#if defined(ISIS_BULLET_THREAD)
    /** Define the RayMapFunctor  */
    struct RayMapFunctor :  public btCollisionWorld::AllHitsRayResultCallback {
      public:
        typedef struct {
          RayTraceResult(btCollisionWorld::LocalRayResult &result,
                         bool normalInWorldSpace,btScalar hitFraction) : 
                         m_rayResult(result), 
                         m_normalInWorldSpace(normalInWorldSpace),
                         m_hitFraction(hitFraction) { }
          btCollisionWorld::LocalRayResult m_rayResult; 
          bool     m_normalInWorldSpace;
          btScalar m_hitFraction;
        } RayTraceResult;

        typedef QVector<RayTraceResult> RayTraceSet;

        typedef RayTraceResult result_type;

        RayMapFunctor(const btVector3 &rayStart, const btVector3 &rayEnd, 
                      btCollisionWorld::RayResultCallback &results) :
                      btCollisionWorld::AllHitsRayResultCallback(results), 
                      m_rayStart(rayStart), m_rayEnd(rayEnd), m_results(results),
                      m_myResult(results) { }
        ~RayMapFunctor() { }

        RayTraceFunctor operator()(const SharedBulletWorld &world) {
          world->raycast(m_rayStart, m_rayEnd, m_results);
        }

    };

#endif

    m_raytraces++;

    // This implementation just runs sequentially through the shapes
    // and tests if any hits were accepted by the callers callback
    // functor. If priority == true, upon the first occurance of 
    // a hasHit() == true, the loop terminates immediately.
    foreach ( const SharedBulletWorld &v_world, m_shapes ) {
      // QMutexLocker locker( m_mutex.data() );  // ALREADY in BulletWorldManager!!
      v_world->raycast(rayStart, rayEnd, results);
      if ( prioritize ) {
        if ( results.hasHit() ) break;
      }
    }

    return ( results.hasHit() );
  }

  /** Return the number of rays tested in this instance */
  BigInt BulletPrioritizedShapes::ntraces() const {
      return ( m_raytraces );
  }

  /** Set debug status   */
  void BulletPrioritizedShapes::setDebug(const bool &status) {
    m_debug = status;
  }

  /** Return debug status   */
  bool BulletPrioritizedShapes::isDebug() const {
    return ( m_debug );
  }

  void BulletPrioritizedShapes::reportModelParameters(PvlContainer &parameters) const {
    // Do this here, otherwise default behavior will ensue from here on out
    parameters.addKeyword(PvlKeyword("RayTraceEngine", "Bullet"), PvlContainer::Replace);
  }

}  // namespace Isis
