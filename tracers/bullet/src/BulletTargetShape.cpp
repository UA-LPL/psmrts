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

#include "BulletTargetShape.h"
#include "BulletDskShape.h"
#include "BulletObjShape.h"

#include <iostream>
#include <iomanip>
#include <numeric>
#include <sstream>

#include <QTime>

#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Pvl.h"

namespace Isis {

  /**
   * @brief Default empty constructor.
   * 
   * The filename defaults to an empty string and the maximum distance defaults to 0.
   */
  BulletTargetShape::BulletTargetShape() : m_parameters(), 
                                           m_name(""), m_btbody(), 
                                           m_maximumDistance(0), 
                                           m_debug(false) { }



  BulletTargetShape::BulletTargetShape(const Pvl *conf) : 
                                       m_parameters(), 
                                       m_name(""), m_btbody(), 
                                       m_maximumDistance(0), 
                                       m_debug(false) { 

    if  ( conf ) {
      init( PvlFlatMap(*conf) );
    }

  }

  BulletTargetShape::BulletTargetShape(const PvlFlatMap &params) :
                                       m_parameters(),
                                       m_name(""), m_btbody(), 
                                       m_maximumDistance(0), 
                                       m_debug(false) { 
      init(params);
  }

  /** 
   * Constructs a BulletTargetShape for a given a Bullet object
   * 
   * @param btbody The Bullet collision object to contain.
   * @param name The name of the object.
   */
  BulletTargetShape::BulletTargetShape(btCollisionObject *btbody, const QString &name) :
                                       m_parameters(), m_name(name), m_btbody(btbody),
                                       m_maximumDistance(0), m_debug(false) {
    setMaximumDistance();
  }


  /**
   * Desctructor
   */
  BulletTargetShape::~BulletTargetShape() { }


  /**
   * Calculate and save the maximum distance across the body. This is
   * calculated as the distance from the x, y, z minimum to x, y, z maximum.
   *
   * @history 2019-05-30 Kris Becker The bounding sphere was computed in funky
   *                       way that did not work for regional shape models.
   *                       Provide accurate determination of maximum radius/BB.
   */
  void BulletTargetShape::setMaximumDistance(const double scale) {
    if (m_btbody) {
#if 0     // Old  Stuff
      btVector3 center;
      m_btbody->getCollisionShape()->getBoundingSphere(center, m_maximumDistance);
      m_maximumDistance *= 2;
#else
       btTransform tr;
       tr.setIdentity();
       btVector3 aabbMin,aabbMax;
   
       m_btbody->getCollisionShape()->getAabb(tr,aabbMin,aabbMax);
       m_maximumDistance = ( aabbMin.length() < aabbMax.length() ) ? aabbMax.length() : aabbMin.length();
       m_maximumDistance *= scale;
   
#endif
    }
    else {
      m_maximumDistance = 1.0 * scale;
    }
  }


  /**
   * Return name of the target shape
   * 
   * @return @b QString The target name
   */
  QString BulletTargetShape::name() const {
    return ( m_name );
  }

  /**
   * Load a DEM file into the target shape.
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load. Currently unused.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::load(const QString &dem, const PvlFlatMap &pvlmap) {
     Pvl pvl;
     foreach ( const PvlKeyword &key, pvlmap) {
         pvl.addKeyword( key );
     }
     return ( load(dem, &pvl) );
  }


  /**
   * Load a DEM file into the target shape.
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load. Currently unused.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::load(const QString &dem, const Pvl *conf) {
    FileName v_file(dem);
    
    QString ext = v_file.extension().toLower();

    if ( "bds" == ext)    return ( loadDSK(dem, conf) );
    if ( "obj" == ext)    return ( loadOBJ(dem, conf) );
    if ( "bullet" == ext) return ( loadBullet(dem, conf) );
    if ( "cub" == ext)    return ( loadCube(dem, conf) );
    return ( loadPC(dem, conf) );
  }


  /** 
   * Load a point cloud type DEM in Bullet.
   * 
   * @note Currently not implemented
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load. Currently unused.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::loadPC(const QString &dem, const Pvl *conf) {
    return (0);
  }

  /**
   * Load a DSK in Bullet
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::loadDSK(const QString &dem, const Pvl *conf) {
    return ( new BulletDskShape(dem, conf) );
  }


  /**
   * Load a OBJ file in Bullet
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::loadOBJ(const QString &dem, const Pvl *conf) {
    return ( new BulletObjShape(dem, conf) );
  }


  /** Load an ISIS cube type DEM in Bullet.
   * 
   * @note Currently not implemented
   * 
   * @param dem The DEM file to load.
   * @param conf PVL config for the DEM load. Currently unused.
   * 
   * @return @b BulletTargetShape A target shape containing the DEM
   */
  BulletTargetShape *BulletTargetShape::loadCube(const QString &dem, const Pvl *conf) {
    return (0);
  }

  BulletTargetShape *BulletTargetShape::loadBullet(const QString &dem, const Pvl *conf) {
      return (0);
  }

  /** 
   * Write a serialized version of the target shape to a Bullet file
   * 
   * @param btName The name of the file to write the target shape to.
   */
  void BulletTargetShape::writeBullet(const QString &btName) const {

    if ( isDebug() ) {
       std::cout << "\nWriting Bullet Format shape to " << btName << "\n";
    }

    // Start load timing
    QTime runTime = QTime::currentTime();
    runTime.start();

    btCollisionObject *object = body();

    int maxSerializeBufferSize = 1024*1024*5;
    btDefaultSerializer* serializer = new btDefaultSerializer(maxSerializeBufferSize);
    serializer->startSerialization();

    object->serializeSingleObject(serializer);
    serializer->finishSerialization();

    FileName btfile(btName);
    std::string fname = btfile.expanded().toStdString();
    FILE* f2 = fopen(fname.c_str(),"wb");
    fwrite(serializer->getBufferPointer(),serializer->getCurrentBufferSize(),1,f2);
    fclose(f2);

    if ( isDebug() ) {
        double ttime = runTime.elapsed() / 1000.0;
        std::cout << "Done. Elasped Time: " << ttime << "\n";
    }
    
    return;
  }

  /** 
   * Return a pointer to the Bullet target object/shape
   * 
   * @return @b btCollisionObject A target to the Bullet collision object.
   */
  btCollisionObject *BulletTargetShape::body() const {
    return ( m_btbody.data() );
  }

/**
 * Return the mesh associated with this target shape
 *  
 * If the deriving class has a mesh, this should be reimplemented 
 * 
 * @return const BulletMeshMapper& 
 */
  const BulletMeshMapper &BulletTargetShape::getMeshMap() const {
    QString mess = "There is no meshmap associated with target " + name();
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }

  /**
   * @brief Make default configuration file with sub parts specificiation option 
   * 
   * @param nparts  Number of Bullet subparts to use in mesh
   * 
   * @return Pvl  Bullet configuration file
   */
  Pvl BulletTargetShape::makeDefaultConfig(const int nparts, const bool debug) const {
      Pvl conf;
      conf.addKeyword(PvlKeyword("BulletParts", toString(nparts)));
      conf.addKeyword(PvlKeyword("BulletDebug", toString(debug)));
      return ( conf );
  }

  void BulletTargetShape::reportModelParameters(PvlContainer &parameters) const {
    // Do this here, otherwise default behavior will ensue from here on out
    parameters.addKeyword(PvlKeyword("RayTraceEngine", "Bullet"), PvlContainer::Replace);

    if ( m_parameters.exists("OnError") ) {
      parameters.addKeyword( m_parameters.keyword("OnError"), PvlContainer::Replace );
    }

    if ( m_parameters.exists("Tolerance") ) {
      parameters.addKeyword( m_parameters.keyword("Tolerance"), PvlContainer::Replace );
    }

  }


  /** Set the internal dubugging state   */
  void BulletTargetShape::setDebug( const bool debug) {
      m_debug = debug;
      return;
  }


  /** Return the internal debugging state  */
  bool BulletTargetShape::isDebug() const {
      return ( m_debug );
  }

  /** Set name of shape   */
  void BulletTargetShape::setName(const QString &name) {
      m_name = name;
  }

  /** Set the Bullet shape object to this object instance   */
  void BulletTargetShape::setTargetBody(btCollisionObject *body) {
    m_btbody.reset(body);
    m_btbody->setUserPointer(this);
    setMaximumDistance();
    return;
  }

  btScalar BulletTargetShape::maximumDistance() const {
    return m_maximumDistance;
  }

  void BulletTargetShape::init(const PvlFlatMap &conf) {
     if ( conf.exists("RayTraceEngine") ) m_parameters.add(conf.keyword("RayTraceEngine"));
     if ( conf.exists("OnError") )        m_parameters.add(conf.keyword("OnError"));
     if ( conf.exists("Tolerance") )      m_parameters.add(conf.keyword("Tolerance"));
     if ( conf.exists("BulletParts") )    m_parameters.add(conf.keyword("BulletParts"));

     // Check to see if debugging is desired
     setDebug(toBool(conf.get("BulletDebug", "false")));

     return;
  }

}  // namespace Isis
