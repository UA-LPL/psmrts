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
#include "BulletDskShape.h"


#include <iostream>
#include <iomanip>
#include <numeric>
#include <sstream>

#include "NaifDskApi.h"

#include <QMutexLocker>
#include <QTime>

#include "FileName.h"
#include "IException.h"
#include "IsisBullet.h"
#include "IString.h"
#include "Pvl.h"

#include "NaifDskPlateModel.h"
#include "NaifStatus.h"

using namespace std;

namespace Isis {

  /**
   * Default empty constructor.
   */
  BulletDskShape::BulletDskShape() :  BulletTargetShape(), 
                                      m_meshfile(), 
                                      m_mesh( new BulletMeshData() ) { }

  BulletDskShape::BulletDskShape(const Pvl *conf) : BulletTargetShape(conf), 
                                                    m_meshfile(), 
                                                    m_mesh( new BulletMeshData() ) { }

  /**
   * Construct a BulletDskShape from a DSK file.
   * 
   * @param dskfile The DSK file to load into a Bullet target shape.
   */
  BulletDskShape::BulletDskShape(const QString &dskfile, const int nparts,
                                 const bool debug) : BulletTargetShape(), 
                                 m_meshfile(dskfile), 
                                 m_mesh( new BulletMeshData() ) {
    Pvl conf = makeDefaultConfig(nparts, debug);
    setDebug(debug);
    loadMeshFile(dskfile, &conf);
    setName(dskfile);
  }

  BulletDskShape::BulletDskShape(const QString &dskfile, const Pvl *conf) : 
                                 BulletTargetShape(conf), 
                                 m_meshfile(dskfile), 
                                 m_mesh( new BulletMeshData() ) {
    loadMeshFile(dskfile, conf);
    setName(dskfile);
  }


  /**
   * Destructor
   */
  BulletDskShape::~BulletDskShape() { 

  }

/**
 * Returns the name of the DSK file
 * 
 * @return QString Name of input file
 */
  QString BulletDskShape::filename() const {
      return (m_meshfile);
  }


  /**
   * Return the number of triangles in the shape
   * 
   * @return @b int The number of triangles. If nothing has been loaded, then 0 is returned.
   */
  int BulletDskShape::getNumTriangles() const {
    return ( m_mesh->m_btMesh.ntriangles() );
  }


  /** 
   * Return the number of verticies in the shape
   * 
   * @return @b int The number of verticies. If nothing has been loaded, then 0 is returned.
   */
  int BulletDskShape::getNumVertices() const {
    return ( m_mesh->m_btMesh.nvertices() );
  }


  /**
  * @brief Return normal for a given triangle index
  *  
  * This method is particularly useful to return the normal of a triangle plate 
  * in a mesh-based target body.  
  * 
  * @author 2017-03-28 Kris Becker 
  *  
  * @param indexId The index of the triangle in the mesh.
  * 
  * @return @b btVector3 The local normal for the triangle.
  */
  btVector3 BulletDskShape::getNormal(const int index) const {
    btVector3 normal = m_mesh->m_btMesh.getNormal(index);
    return ( normal );
  }


  /**
   * Get the vertices of a triangle in the mesh.
   * 
   * @param index The index of the triangle in the mesh.
   * 
   * @return @b btMatrix3x3 Matrix with each row containing the coordinate of a
   *                        vertex. The vertices are ordered counter-clockwise
   *                        around the surface normal of the triangle.
   */
  btMatrix3x3 BulletDskShape::getTriangle(const int index) const {
    btMatrix3x3 triangle = m_mesh->m_btMesh.getTriangle(index);
    return ( triangle );
  }

/** 
 * @brief Return the Bullet mesh mapper object 
 *  
 * @return const BulletMeshMapper& Returns a const reference to the mesh map
 */
  const BulletMeshMapper &BulletDskShape::getMeshMap() const {
      return ( m_mesh->m_btMesh );
  }



/**
 * @brief Load the contents of a NAIF DSK and create a Bullet triangle mesh  
 *  
 * This method may be suitable for most derived classes 
 *  
 * @param dskfile The DSK file to load.
 */
  void BulletDskShape::loadMeshFile(const QString &meshfile, const int nparts,
                                    const bool debug) { 
    Pvl conf = makeDefaultConfig(nparts, debug);
    loadMeshFile(meshfile, &conf);
  }


/**
 * @brief Load the contents of a NAIF DSK and create a Bullet triangle mesh  
 * 
 * @author 2017-03-28 Kris Becker 
 *  
 * @internal 
 *   @history 2021-04-21 Kris Becker Made this method thread safe from
 *                          interacting with the NAIF static environment
 * 
 * @param dskfile The DSK file to load.
 */
  void BulletDskShape::loadMeshFile(const QString &meshfile, const Pvl *conf) {

    // Ok, in order to thread the load of a NAIF DSK, we must lock any NAIF
    // interactions to provide exclusive access to this operation. A static 
    // mutex should do it. The NAIF read and load is pretty fast. Its the
    // Bullet processing that is costly.
    static QMutex naifMutex;


    if ( isDebug() ) {
      std::cout << "\nLoading DSK shape from " << meshfile << "\n";
    }

    // Process configuration parameters 
    PvlFlatMap parms(m_parameters);
    if ( conf)  parms.merge(PvlFlatMap(*conf));
    int nparts =  toInt(parms.get("BulletParts", "0"));
      
    // Set the filename...
    m_meshfile = meshfile;

    /** NAIF DSK parameter setup   */
    SpiceInt      v_handle;   //!< The DAS file handle of the DSK file.

    // Sanity check if file exists
    FileName dskFile(meshfile);
    if ( !dskFile.fileExists() ) {
      QString mess = "NAIF DSK file [" + meshfile + "] does not exist.";
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Lets lock up all NAIF interactions here to serialized access. This 
    // approach could be improved.
    QMutexLocker naif(&naifMutex);

    // Start load timing
    QTime runTime = QTime::currentTime();
    runTime.start();
  
    // Open the NAIF Digital Shape Kernel (DSK)
    dasopr_c( dskFile.expanded().toLatin1().data(), &v_handle );
    NaifStatus::CheckErrors();

    SpiceBoolean found(1);
    SpiceDLADescr v_dladsc;  

    // Find first segment
    dlabfs_c(v_handle, &v_dladsc, &found);
    NaifStatus::CheckErrors();
    if ( !found ) {
      QString mess = "No segments found in DSK file " + meshfile ; 
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Parameters for DSK states
    SpiceDSKDescr v_dskdsc; 
    QVector<SpiceDLADescr> v_segments;

    SpiceInt      v_plates (0);   //!< Number of Plates in the model.
    SpiceInt      v_vertices(0); //!< Number of vertices defining the plate.

    while ( found ) {

        // Save the this segment and evaluate contents
        v_segments.push_back(v_dladsc);

        dskgd_c( v_handle, &v_dladsc, &v_dskdsc );
        NaifStatus::CheckErrors();

        // Get size/counts
        SpiceInt nvertices, nplates;
        dskz02_c( v_handle, &v_dladsc, &nvertices, &nplates );
        NaifStatus::CheckErrors();
       
        v_vertices += nvertices;
        v_plates   += nplates;

        // Check for next one...
        dlafns_c(v_handle, &v_segments.back(), &v_dladsc, &found);
        NaifStatus::CheckErrors();
    }

    // Now allocate a new indexed mesh to contain all the DSK data
    m_mesh->allocate(v_vertices, v_plates);
    btAssert( !m_mesh->m_btVertex.isNull() );
    btAssert( !m_mesh->m_btIndex.isNull() );

    double *vVertexBasePtr = m_mesh->m_btVertex.data();
    int    *vIndexBasePtr = m_mesh->m_btIndex.data();

    // Now read in all mesh and reset the indexes so that 
    // they are sequential
    SpiceInt indexOffset = -1;
    for (int s = 0 ; s < v_segments.size() ; s++) {
        
       // Get size/counts...again
        SpiceInt nvertices, nplates;
        dskz02_c( v_handle, &v_segments[s], &nvertices, &nplates );
        NaifStatus::CheckErrors();

        SpiceInt n;
        (void) dskv02_c(v_handle, &v_dladsc, 1, nvertices, &n, 
                        ( SpiceDouble(*)[3] ) (vVertexBasePtr));
        NaifStatus::CheckErrors();

        // Read the indexes from the DSK
        (void) dskp02_c(v_handle, &v_dladsc, 1, nplates, &n, 
                        ( SpiceInt(*)[3] ) (vIndexBasePtr));
        NaifStatus::CheckErrors();

        // Got to reset the vertex indexes to 0-based
        int nverts = nplates * 3;
        for (int i = 0 ; i < nverts ; i++) {
          vIndexBasePtr[i] += indexOffset;
          btAssert ( vIndexBasePtr[i] >= 0 );
          btAssert ( vIndexBasePtr[i] < v_vertices );
        }

        // Update offset for next segment
        indexOffset += nvertices;
        vVertexBasePtr += (nvertices * 3);
        vIndexBasePtr  += (nplates * 3);
    }

    // Ok, close the DSK, which also means we can unlock the mutex from
    // anytime beyond this point.
    dascls_c(v_handle);
    double ttime = runTime.elapsed() / 1000.0;

    if ( isDebug() ) {
        std::cout << "Load/ProcessTime: " << ttime << "\n";
    }
    
    // Now map the mesh using requested parts. This approach treats each 
    // segment equally and assumes the segments/meshes are unique parts of
    // the target body. If this is not the desired behavior, it is 
    // recommended to separate the segments into separate files or
    // reimplement this load method.
    m_mesh->m_btMesh.addArray(v_vertices, m_mesh->m_btVertex.data(),  
                              v_plates, m_mesh->m_btIndex.data(), nparts);
    if ( isDebug() ) {
        std::cout << "\nBulletParts:       " << nparts << "\n";
        std::cout << "MaxParts:          " << m_mesh->m_btMesh.MaxPartsPerBody() << "\n";
        std::cout << "MaxTrangles:       " << m_mesh->m_btMesh.MaxTrianglesPerPart() << "\n";
        std::cout << "MaxTriangles/Part: " << m_mesh->m_btMesh.MaxTrianglesPerPart() << "\n";
        std::cout << "PartsUsed:         " << m_mesh->m_btMesh.nparts() << "\n";
        std::cout << "Vertices:          " << m_mesh->m_btMesh.nvertices() << "\n";
        std::cout << "Indexes/Facets:    " << m_mesh->m_btMesh.ntriangles() << "\n";
    }

    // Location of this lock matters. One of the most costly operations is to
    // compute the quantitized AABB compression in Bullet. Lets unlock the timer
    // for this opperation and get a little more serialized I/O reporting.
    naif.unlock();

    // Create the target (collision) body mesh and add to Bullet world.
    bool useQuantizedAabbCompression = true;
    addMeshToWorld(m_mesh->m_btMesh, useQuantizedAabbCompression);

    if ( isDebug() ) {
        std::cout << "TotalTime load/processing of "  << meshfile << ": " << (runTime.elapsed() / 1000.0) << "\n\n";
    }

    return;

  }


/**
 * @brief Add the mesh to the Bullet world
 * 
 * @param meshmap        Mapped mesh to add
 * @param useCompression Use BVH compression (recommended!)
 */
  void BulletDskShape::addMeshToWorld(BulletMeshMapper &meshmap, 
                                      const bool useCompression) {

      if ( isDebug() ) {
        std::cout << "\nInit/Adding Mesh to World - Compression: " << useCompression << "\n";
      }

      // Start load timing
      QTime runTime = QTime::currentTime();
      runTime.start();

      btBvhTriangleMeshShape *v_triShape = new btBvhTriangleMeshShape(&meshmap.mesh(), 
                                                                      useCompression);
      v_triShape->setUserPointer(this);
      btCollisionObject *vbody = new btCollisionObject();
      vbody->setCollisionShape(v_triShape);
      setTargetBody(vbody);

      double ttime = runTime.elapsed() / 1000.0;
      if ( isDebug() ) {
        std::cout << "BulletMeshInitTime: " << ttime << "\n\n";
      }

      // Determine maximum distance
      setMaximumDistance();
      return;
  }

}  // namespace Isis
