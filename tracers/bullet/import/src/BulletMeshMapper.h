#ifndef BulletMeshMapper_h
#define BulletMeshMapper_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "Constants.h"
#include "IsisBullet.h"

// #define   BT_USE_DOUBLE_PRECISION   1 
// #define   B3_USE_DOUBLE_PRECISION   1 
// #include <btBulletDynamicsCommon.h>

#include <QString>


namespace Isis {

  /**
   * This class constructs a mesh array consisting of vertices and indexes and
   * produces a series of btTriangleIndexVertexArrays that are intended to be used
   * in the btBvhTriangleMeshShape class. You can add additional meshes to any
   * vertex array or break up larger ones into smaller ones to create and test for
   * nominal mapping conditions.
   * 
   * Should you apply the results of this object, you can keep it around to
   * provide access to individual parts of the data. See GenericMeshContainer for
   * specialized storage or mesh data.
   * 
   * Note this routine does *not own* the memory references of vertices and
   * indexes so the caller must manage the (presumably, but not required)
   * allocated memory references supplied to this class. 
   * 
   * Note vertices and indexes are always assumed to be triplets. Sizes are
   * computed accordingly.
   * 
   * Note this class in intended to describe a single body. You should construct a
   * separate mapper for unique bodies. It is intended for flexible mapping of
   * potential very large meshes to test and represent the most efficient mapping
   * while adhering to the limits imposed by the Bullet API in use. See the
   * specification of MAX_NUM_PARTS_IN_BITS in
   * https://github.com/bulletphysics/bullet3/blob/master/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.h
   * for details as this class maintains meshes constrained for bounding volume
   * include file that adjusted this definition such that more that 300 million
   * triangles could be stored/indexed mesh. This class eliminates this
   * requiremenet as long as the Bullet API used is built using double precision
   * (the default is float!). However, limits can be exceeded so strict compliance
   * with the limits as constructed during runtime are maintained through
   * exceptions.
   *
   * @author 2019-09-19 Kris J. Becker, University of Arizona (UA)
   *
   * @internal
   *   @history 2019-09-19 Kris J. Becker (UA) Original Version
   */
  class BulletMeshMapper {
    public:

      // Constructors
      BulletMeshMapper();
      BulletMeshMapper(const QString &name);
      BulletMeshMapper(const int nvertices,  const double *vertices, 
                       const int nindexes,  const int *indexes,
                       const int numparts = 0,
                       const QString &name = "");
      BulletMeshMapper(const btTriangleIndexVertexArray &mesh, 
                       const QString &name = "");
      BulletMeshMapper(const btTriangleIndexVertexArray &mesh, 
                       const int nvertices, const int nindexes,
                       const QString &name = "");

      // Destructor
      ~BulletMeshMapper();

      void setName(const QString &name);
      QString name() const;

      int nparts() const;
      int ntriangles() const;
      int nvertices() const;
 
      bool isIndexValid(int virtualIndex) const;

      int getVirtualIndex(const int partno, const int index) const;

      bool getIndexMap(const int virtualIndex, int &partno, int &index) const;

      // The Bullet components
      const btTriangleIndexVertexArray &mesh() const;
      btTriangleIndexVertexArray &mesh();

      const IndexedMeshArray &meshArray() const;
      const btIndexedMesh &getPart(const int partno) const;
      const btIndexedMesh &getPartFromVirtualIndex(const int virtual_index, int &pindex) const;

      // These methods are only valid as long as the memory and maps are valid
      btMatrix3x3 getTriangle(const int partno, const int index) const;
      btMatrix3x3 getTriangle(const int virtualIndex) const;
      btMatrix3x3 getFacet(const int virtualIndex) const;
      btVector3   getNormal(const int virtualIndex) const;

      void clear();

      // Populate with an array of vertices and indexes
      int addArray(const int nvertices,  const double *vertices, 
                   const int nindexes,  const int *indexes,
                   const int numparts);

      // Computed from the IsisBullet.h definitions for convenience
      static int    MaxTrianglesPerPart();
      static int    MaxPartsPerBody();
      static BigInt MaxFacets();

    private:
      typedef btTriangleIndexVertexArray MeshMap;

      QString m_name;      //!< Name of the mesh
      int     m_vertices;  //!< Total number of vertices mapped
      int     m_indexes;   //!< Total number of indexes mapped
      MeshMap m_mesh;      //!< Bullet Container of vertex maps

  };
}

#endif
