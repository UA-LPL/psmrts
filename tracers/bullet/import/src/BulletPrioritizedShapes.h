#ifndef BulletPrioritizedShapes_h
#define BulletPrioritizedShapes_h
/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/08/25 01:37:55 $
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

#include <QCache>
#include <QMutex>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QtGlobal>
#include <QVector>

#include "PvlContainer.h"
#include "BulletWorldManager.h"

namespace Isis {

/**
 * @brief Provide support for prioritized list of one or more Bullet shapes
 *  
 * @author 2021-04-22 Kris Becker 
 *  
 * @internal 
 *   @history 2021-04-22  Kris Becker  Original Version
 *   @history 2021-04-25  Kris Becker - Added the add() method
 */
  class BulletPrioritizedShapes {
    public:
      /** Define a Bullet Shape queue   */
      typedef QSharedPointer<BulletWorldManager> SharedBulletWorld;
      typedef QVector<SharedBulletWorld>         BulletShapeLayers;

      BulletPrioritizedShapes();
      BulletPrioritizedShapes(const QString &name);
      BulletPrioritizedShapes(BulletTargetShape *shape);
      BulletPrioritizedShapes(const BulletWorldManager &world);
      BulletPrioritizedShapes(BulletWorldManager *world);
      BulletPrioritizedShapes(const BulletShapeLayers &prishapes,
                              const QString &name = "PrioritizedShape");
      BulletPrioritizedShapes(const BulletPrioritizedShapes &pshape);

      BulletPrioritizedShapes &operator=(const BulletPrioritizedShapes &other);

      virtual ~BulletPrioritizedShapes();

      QString name() const;
      int size() const;

      void add(BulletTargetShape *shape);
      void add(const BulletWorldManager &world);
      void add(BulletWorldManager *world);
      const BulletWorldManager &get(const int &index = 0) const;

      double maximumDistance() const;

      bool raycast( const btVector3 &observer, const btVector3 &lookdir, 
                    btCollisionWorld::RayResultCallback &hits,
                    const bool prioritize = true ) const;

      BigInt ntraces() const;

      void setDebug(const bool &status = true);
      bool isDebug() const;

      void reportModelParameters(PvlContainer &parameters) const;

    private:
      QString                           m_name;
      BulletShapeLayers                 m_shapes;
      mutable BigInt                    m_raytraces; //!< Ray trace counter
      mutable QScopedPointer<QMutex>    m_mutex;  //!< Thread lock
      bool                              m_debug;  //!< Debug mode

  };


} // namespace Isis

//  Improves Qt container performance by notifying Qt's subsystem of its
//  movable nature
Q_DECLARE_TYPEINFO(Isis::BulletPrioritizedShapes, Q_MOVABLE_TYPE);
#endif

