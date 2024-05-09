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

#define TINYOBJLOADER_IMPLEMENTATION
#include "BulletObjShape.h"

#include <iostream>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>

#include <QFile>
#include <QTextStream>
#include <QTime>

#include "Application.h"
#include "FileName.h"
#include "IException.h"
#include "IsisBullet.h"
#include "IString.h"
#include "Pvl.h"
#include "PvlFlatMap.h"

using namespace std;

namespace Isis {

  /**
   * Default empty constructor.
   */
  BulletObjShape::BulletObjShape() : BulletDskShape() { }


  BulletObjShape::BulletObjShape(const Pvl *conf) : BulletDskShape(conf) { }

  /**
   * Default empty constructor.
   */
  BulletObjShape::BulletObjShape(const QString &objfile) : BulletDskShape() {
    Pvl conf = makeDefaultConfig(); 
    loadMeshFile(objfile, &conf);
    setName(objfile);
  }


  /**
   * Construct a BulletObjShape from a DSK file.
   * 
   * @param dskfile The DSK file to load into a Bullet target shape.
   */
  BulletObjShape::BulletObjShape(const QString &objfile, const int nparts, 
                                 const bool debug) : BulletDskShape() {
    Pvl conf = makeDefaultConfig(nparts, debug);
    loadMeshFile(objfile, &conf);
    setName(objfile);
  }

  BulletObjShape::BulletObjShape(const QString &objfile, const Pvl *conf) : 
                                 BulletDskShape(conf) {
      loadMeshFile(objfile, conf);
      setName(objfile);
  }

  /**
   * Desctructor
   */
  BulletObjShape::~BulletObjShape() { }



/**
 * @brief Load the contents of a NAIF DSK and create a Bullet triangle mesh  
 * 
 * @author 2017-03-28 Kris Becker 
 * 
 * @param dskfile The DSK file to load.
 */
  void BulletObjShape::loadMeshFile(const QString &meshfile, const Pvl *conf) {


    if ( isDebug() ) {
       std::cout << "\nLoading OBJ shape from " << meshfile << "\n";
    }

    // Process configuration parameters 
    PvlFlatMap parms(m_parameters);
    if ( conf)  parms.merge(PvlFlatMap(*conf));
    int nparts =  toInt(parms.get("BulletParts", "0"));
      
    // Set the filename...
    m_meshfile = meshfile;
   
    FileName objfile(meshfile);
    std::string filename = objfile.expanded().toStdString();

    tinyobj::callback_t cb;
    cb.vertex_cb = BulletObjShape::vertex_cb;
    cb.index_cb  = BulletObjShape::index_cb;
    cb.normal_cb = 0;
    cb.texcoord_cb = 0;
    cb.usemtl_cb = 0;
    cb.mtllib_cb = 0;
    cb.group_cb = 0;
    cb.object_cb = 0;


    TinyMesh objmesh;
    objmesh.minx = objmesh.miny = objmesh.minz = DBL_MAX;
    objmesh.maxx = objmesh.maxy = objmesh.maxz = DBL_MIN;

    QTime runTime = QTime::currentTime();
    runTime.start();

    std::ifstream ifs(filename.c_str());
    if (ifs.fail()) {
        QString mess = "Could not open file input file" + QString::fromStdString(filename);
        throw IException(IException::User, mess, _FILEINFO_);
    }


    std::string warn;
    std::string err;
    bool valid = tinyobj::LoadObjWithCallback(ifs, cb, &objmesh, 0, &warn, &err);

    double ttime = runTime.elapsed() / 1000.0;  // Convert to seconds
    if ( isDebug() ) {
        std::cout << "LoadTime: " << ttime << "\n";
    }

    // Check to see if we got a valid load
    if ( !valid ) {
        QString mess = "Load Failed in file " + meshfile + 
                       " - Error: " + QString::fromStdString(err);
        throw IException(IException::User, mess, _FILEINFO_);
    }

    // Sanity check
    int nvertices = objmesh.s_vertices.size() / 3;
    int nindexes  = objmesh.s_indices.size() / 3;

    if ( (unsigned int) (nvertices * 3)  != objmesh.s_vertices.size() ) {
        QString mess = "Invalid vertices count (" + 
                        QString::number(objmesh.s_vertices.size()) +
                        ") - not an even multiple of 3 in file " + meshfile; 
        throw IException(IException::User, mess, _FILEINFO_);
    }
    
    if ( (unsigned int) (nindexes * 3)  != objmesh.s_indices.size() ) {
        QString mess = "Invalid index count (" + 
                        QString::number(objmesh.s_indices.size()) +
                        ") - not an even multiple of 3 in file " + meshfile; 
        throw IException(IException::User, mess, _FILEINFO_);
    }

    // All good! Allocate internal storage. This is done as the vectors
    // used to store the data are typically much large than is necessary.
    m_mesh->allocate(nvertices, nindexes);
    btAssert( !m_mesh->m_btVertex.isNull() );
    btAssert( !m_mesh->m_btIndex.isNull() );

    // Transfer data
    double *vVertexBasePtr = m_mesh->m_btVertex.data();
    for (unsigned int v = 0 ; v < objmesh.s_vertices.size() ; v++) {
      vVertexBasePtr[v] = objmesh.s_vertices[v];
    }

    int    *vIndexBasePtr = m_mesh->m_btIndex.data();
    for (unsigned int n = 0 ; n < objmesh.s_indices.size() ; n++) {
      vIndexBasePtr[n] = objmesh.s_indices[n];
    }

    double ltime = (runTime.elapsed() / 1000.0) - ttime;  // Processing time
    if ( isDebug() ) {
        std::cout << "ProcessingTime: " << ltime << "\n";
    }

    // Now map the mesh using requested parts. This approach treats each 
    // segment equally and assumes the segments/meshes are unique parts of
    // the target body. If this is not the desired behavior, it is 
    // recommended to separate the segments into separate files or
    // reimplement this load method.
    m_mesh->m_btMesh.addArray(nvertices, m_mesh->m_btVertex.data(),  
                              nindexes, m_mesh->m_btIndex.data(), nparts);
    if ( isDebug() ) {
        std::cout << "\nBulletParts:       " << nparts << "\n";
        std::cout << "MaxParts:          " << m_mesh->m_btMesh.MaxPartsPerBody() << "\n";
        std::cout << "MaxTriangles/Part: " << m_mesh->m_btMesh.MaxTrianglesPerPart() << "\n";
        std::cout << "PartsUsed:         " << m_mesh->m_btMesh.nparts() << "\n";
        std::cout << "Vertices:          " << m_mesh->m_btMesh.nvertices() << "\n";
        std::cout << "Indexes/Facets:    " << m_mesh->m_btMesh.ntriangles() << "\n";
    }

    // Create the target (collision) body mesh and add to Bullet world.
    bool useQuantizedAabbCompression = true;
    addMeshToWorld(m_mesh->m_btMesh, useQuantizedAabbCompression);

    if ( isDebug() ) {
        std::cout << "TotalTime: " << (runTime.elapsed() / 1000.0) << "\n";
    }
    return;

  }

/**
 * @brief Write the mesh to an OBJ file
 * 
 * @param objFile Name of OBJ file to write the mesh to
 */
  void BulletObjShape::writeOBJ(const QString &objfile) const {
    typedef BulletMeshData::btIndex3 btIndex3;

    if ( isDebug() ) {
      std::cout << "\nWriting OBJ from shape to " << objfile << "\n";
    }
    
    QTime runTime = QTime::currentTime();
    runTime.start();

    FileName fname(objfile);
    QFile ofile(fname.expanded());
    if ( !ofile.open(QIODevice::WriteOnly | QIODevice::Text) ) {
      QString mess = "Unable to open mesh OBJ file " + fname.name();
      throw IException(IException::User, mess, _FILEINFO_);
    }

    QTextStream lout(&ofile);

    lout << "# Bullet Shape Model OBJ converter\n";
    lout << "# Source:    " << filename() << "\n";
    lout << "# NVertices: " << m_mesh->nVertices() << "\n";
    lout << "# NIndexes:  " << m_mesh->nIndexes() << "\n";
    lout << "# Date:      " << Application::DateTime() << "\n";
    lout << "#\n";

    // lout << qSetRealNumberPrecision(13);

    // First write vertices
    for (int v = 0 ; v < m_mesh->nVertices() ; v++) {
      btVector3 vector = m_mesh->getVertex(v);
      lout << "v " << fixed << qSetRealNumberPrecision(13) << vector[0] << " " << vector[1] << " " << vector[2] << "\n";
    }

    // Now indexes - note they must be 1-based by convention
    for (int f = 0 ; f < m_mesh->nIndexes() ; f++) {
      btIndex3 index = m_mesh->getIndex(f);
      lout << "f "  << index[0]+1 << " " << index[1]+1 << " " << index[2]+1 << "\n";
    }
 
    ofile.close();

    if ( isDebug() ) {
        std::cout << "TotalTime: " << (runTime.elapsed() / 1000.0) << "\n";
    }

    return;
  }


  void BulletObjShape::vertex_cb(void *user_data, tinyobj::real_t x, tinyobj::real_t y, 
                                 tinyobj::real_t z, tinyobj::real_t w) {
    TinyMesh *mesh = reinterpret_cast<TinyMesh *>(user_data);

  //  printf("v[%d] = %f, %f, %f (w %f)\n", mesh->vertices.size() / 3, x, y, z, w);

    mesh->s_vertices.push_back(x);
    mesh->s_vertices.push_back(y);
    mesh->s_vertices.push_back(z);
    // Discard w

    // This computes the bounding box
    if ( x < mesh->minx ) mesh->minx = x;
    if ( x > mesh->maxx ) mesh->maxx = x;
    if ( y < mesh->miny ) mesh->miny = y;
    if ( y > mesh->maxy ) mesh->maxy = y;
    if ( z < mesh->minz ) mesh->minz = z;
    if ( z > mesh->maxz ) mesh->maxz = z;
    return;
  }

  void BulletObjShape::index_cb(void *user_data, tinyobj::index_t *indices, int num_indices) {
    // NOTE: the value of each index is raw value.
    // For example, the application must manually adjust the index with offset
    // (e.g. v_indices.size()) when the value is negative(which means relative
    // index).
    // Also, the first index starts with 1, not 0.
    // See fixIndex() function in tiny_obj_loader.h for details.
    // Also, 0 is set for the index value which
    // does not exist in .obj
    TinyMesh *mesh = reinterpret_cast<TinyMesh *>(user_data);

    for (int i = 0; i < num_indices; i++) {
      tinyobj::index_t idx = indices[i];
  //    printf("idx[%d] = %d, %d, %d\n", mesh->v_indices.size(), idx.vertex_index,
  //         idx.normal_index, idx.texcoord_index);

      if ( idx.vertex_index != 0 ) {
         int v_idx = idx.vertex_index - 1;
         mesh->s_indices.push_back(v_idx);
      }
    }

    return;
  }

}  // namespace Isis


