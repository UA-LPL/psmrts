#ifndef BulletTargetShape_h
#define BulletTargetShape_h
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

#include <QSharedPointer>
#include <QScopedPointer>
#include <QString>

#include "IsisBullet.h"
#include "BulletClosestRayCallback.h"
#include "PvlFlatMap.h"

namespace Isis {


  class Pvl; 

/**
 * @brief Bullet Target Shape for planetary bodies 
 *  
 * This class contains the in memory representation of a body for use with the
 * Bullet library. For each type of file that can be used to create a bullet
 * target body, this class should be extended to manage that type of file.
 * 
 * @author 2017-03-17 Kris Becker 
 * @internal 
 *   @history 2017-03-17  Kris Becker  Original Version
 *   @history 2019-05-30  Kris Becker Corrected lat/lon intercepts in regional,
 *                           non-global, shape models. Rays eminating from the
 *                           body origin were not properly being scaled beyond
 *                           the surface for inverted ray target body intercepts.
 *   @history 2019-10-03 Kris Becker Added scale parameter to
 *                           setMaximumDistance(). This is intended to provide a
 *                           safe radius for determination of intersections from
 *                           virtual positions above the target body (e.g., for
 *                           intersections of lat/lon coordinates). Added
 *                           support for OBJ and Bullet native formats. Added
 *                           new generic loader and constructor with a
 *                           PvlFlatMap parameter.
 * 
 *
 */
  class BulletTargetShape {
    public:
      BulletTargetShape();
      BulletTargetShape(const Pvl *conf);
      BulletTargetShape(const PvlFlatMap &params);
      BulletTargetShape(btCollisionObject *btbody, const QString &name = "");
      virtual ~BulletTargetShape();

      QString name() const;

      // Special constructors
      static BulletTargetShape *load(const QString &dem, const PvlFlatMap &params);
      static BulletTargetShape *load(const QString &dem, const Pvl *conf = 0);
      static BulletTargetShape *loadOBJ(const QString &dem, const Pvl *conf = 0);
      static BulletTargetShape *loadPC(const QString &dem, const Pvl *conf = 0);
      static BulletTargetShape *loadDSK(const QString &dem, const Pvl *conf = 0);
      static BulletTargetShape *loadBullet(const QString &dem, const Pvl *conf = 0);
      static BulletTargetShape *loadCube(const QString &dem, const Pvl *conf = 0);

      void writeBullet(const QString &btName) const;
      btCollisionObject *body() const;

      btScalar maximumDistance() const;

      virtual void reportModelParameters(PvlContainer &parameters) const;

      void setDebug( const bool debug);
      bool isDebug() const;

    protected:
      PvlFlatMap  m_parameters;  /**! Shape initialization parameters. 
                                      Deriving classes can modify at will */

      void setName(const QString &name);
      Pvl makeDefaultConfig(const int nparts = 0, 
                            const bool debug = false) const;
      void setTargetBody(btCollisionObject *body);
      void setMaximumDistance(const double scale = 1.5);

    private:
      QString                           m_name; /**! The name of the body */
      QSharedPointer<btCollisionObject> m_btbody; /**! The Bullet collision object
                                                       for the body */
      btScalar                          m_maximumDistance; /**! The distance from the minimum
                                                                x, y, z values to the maximum
                                                                x, y, z values. */

      bool                              m_debug; /**! Debugging status */

      void init(const PvlFlatMap &conf);

  };

} // namespace Isis

#endif

