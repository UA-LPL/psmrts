#ifndef BulletDskShape_h
#define BulletDskShape_h
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

#include <QExplicitlySharedDataPointer>
#include <QObject>
#include <QSharedData>
#include <QScopedPointer>
#include <QString>
#include <QVector>

#include "BulletMeshMapper.h"
#include "BulletTargetShape.h"
#include "BulletClosestRayCallback.h"

namespace Isis {

/**
 * Bullet Target Shape for NAIF type 2 DSK models 
 *  
 * Note this class is designed to handle NAIF DSK shape models. However, it will 
 * accomodate other similarly constructed datasets such as OBJ rather easily by 
 * providing a new implementation of loadMeshFile() in deriving classes. 
 * 
 * @author 2017-03-17 Kris Becker 
 * @internal 
 *   @history 2017-03-17  Kris Becker  Original Version
 *   @history 2018-07-21 UA/OSIRIS-REx IPWG Team  - Add filename() method to
 *                          return name of DSK file associated with this object
 *   @history 2019-10-01 Kris Becker - Added multi-segment support; add mesh
 *                          mapper to partition mesh in subparts in the Bullet
 *                          world.
 *   @history 2020-01-15 Kris Becker - Removed writeOBJ() and added the method
 *                         to the BulletObjShape class.
 *  @history 2020-01-21 Kris Becker - Added setting of name in constructors
 *  @history 2021-04-21 Kris Becker - Added thread safety 
 */
  class BulletDskShape : public BulletTargetShape {
    public:
      BulletDskShape();
      BulletDskShape(const Pvl *conf);
      // BulletDskShape(const QString &dskfile);
      BulletDskShape(const QString &dskfile, const int nparts, const bool debug = false);
      BulletDskShape(const QString &dskfile, const Pvl *conf = 0);
      virtual ~BulletDskShape();

      QString filename() const;
      int getNumTriangles() const;
      int getNumVertices() const;

      virtual btVector3 getNormal(const int index) const;
      virtual btMatrix3x3 getTriangle(const int index) const;

      const BulletMeshMapper &getMeshMap() const;

      // Custom DSK reader 
      virtual void loadMeshFile(const QString &dskfile, const int nparts = 0, 
                                const bool debug = false);
      virtual void loadMeshFile(const QString &dskfile, const Pvl *conf);


    protected:
    /**
     * Wrapper for Bullet DSK Mesh data 
     *  
     * @see QSharedData 
     *  
     * @author 2018-09-13 UA/OSIRIS-REx IPWG Team - Original Version 
     *  
     * @internal 
     *   @history 2018-09-26 UA/OSIRIS-REx IPWG Team - Corrected order of array 
     *                          initializations in constructor
     *   @history 2018-09-26 UA/OSIRIS-REx IPWG Team - Fixed typo in last change
     *   @history 2019-09-26 Kris Becker - Added BulletMeshMapper object and
     *                          mapMesh() method. This replaces the direct use of
     *                          the mesh array in earlier versions. Renamed class
     *                          from BulletDskData to BulletMeshData to indicate
     *                          more generic use.
     *  
     */
      class BulletMeshData : public QSharedData {
        public:
            // Fundamental data types for Bullet buffers
          typedef QScopedPointer<btScalar, QScopedPointerArrayDeleter<btScalar> > BtVertexArray;
          typedef QScopedPointer<int, QScopedPointerArrayDeleter<int> >           BtIndexArray;

          /** Small index array class   */
         class btIndex3 { 
           public:
             btIndex3() { _index[0] = 0; _index[1] = 0; _index[2] =  0;  }
             btIndex3(int i0, int i1, int i2) {  _index[0] = i0; _index[1] = i1; _index[2] = i2;}
             ~btIndex3() { }
             int operator[](int i) { return ( _index[i] ); }
             int _index[3];
         };

          BulletMeshData() : QSharedData(), m_vertices(0), m_indexes(0),
                            m_btMesh(), m_btVertex(0), m_btIndex(0) { 
              // allocate(0, 0);
          }
          ~BulletMeshData() { }

          int nVertices() const {
              return ( m_vertices );
          }

          int nIndexes() const {
              return ( m_indexes );
          }

          /** Allocate buffer data - invokes complete reset   */
          void allocate(const int nVertices, const int nIndexes) {
            m_btVertex.reset( new btScalar[nVertices * 3] );
            m_btIndex.reset( new int[nIndexes * 3] );
             
            m_vertices = nVertices;
            m_indexes = nIndexes;

            m_btMesh.clear();
            return;
          }

          /** Remap the mesh only if data is populated   */
          int remapMesh(const int nparts = 0) {
            m_btMesh.clear();
            btAssert( m_vertices > 0);
            btAssert( m_indexes  > 0);
            btAssert( m_btVertex.data() != 0 );
            btAssert( m_btIndex.data()  != 0 );

            m_btMesh.addArray(m_vertices, m_btVertex.data(),
                              m_indexes, m_btIndex.data(),
                              nparts);
            return ( m_btMesh.nparts() );
          }

          btVector3 getVertex(const int v) const {
              btAssert(v >= 0);
              btAssert(v < m_vertices);
              btAssert( m_btVertex.data()  != 0 );
              btScalar *Vertex = m_btVertex.data();
              int vndx = v * 3;
              return ( btVector3(Vertex[vndx], Vertex[vndx+1], Vertex[vndx+2]) );
          }

          btIndex3 getIndex(const int n) const {
              btAssert(n >= 0);
              btAssert(n < m_indexes);
              btAssert( m_btIndex.data()  != 0 );
              int *Index = m_btIndex.data();
              int indx = n * 3;
              return ( btIndex3(Index[indx], Index[indx+1], Index[indx+2]) );
          }


          // Class variables
          int              m_vertices;   //!< Number verticies in DSK
          int              m_indexes;    //!< Number indexes in DSK
          BulletMeshMapper m_btMesh;     //!< Bullet mesh structure
          BtVertexArray    m_btVertex;   //!< Vertex array
          BtIndexArray     m_btIndex;    //!< Index array

        private:
          Q_DISABLE_COPY(BulletMeshData);  // Undefined for this usage

      };

      QString                                      m_meshfile; /**! Name of DSK/mesh file */
      QExplicitlySharedDataPointer<BulletMeshData> m_mesh; /**! Triangular mesh representation of
                                                              the target shape. The vertex ordering
                                                              is the same as in the DSK file,
                                                              except the DSK uses 1-based indexing
                                                              and this uses 0-based indexing. */



      virtual void addMeshToWorld(BulletMeshMapper &meshmap, 
                                  bool useCompression = true);

  };

} // namespace Isis

#endif

