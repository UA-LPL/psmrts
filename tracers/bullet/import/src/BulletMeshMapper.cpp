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
#include "BulletMeshMapper.h"


#include <iostream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <sstream>

#include <QVector>

#include "IException.h"
// #include "IsisBullet.h"
#include "IString.h"

using namespace std;

namespace Isis {

  /**
   * Default empty constructor.
   */
  BulletMeshMapper::BulletMeshMapper() : m_name("mesh"), m_vertices(0), 
                                         m_indexes(0), m_mesh()  { }


/**
 * @brief Construct a named mesh 
 *  
 * This allows you to set a name to the mesh. It is useful to set it to the 
 * source file to be able to track its origin. 
 * 
 * @author 2019-09-20 Kris Becker
 * 
 * @param name  Name of the mesh
 */
  BulletMeshMapper::BulletMeshMapper(const QString &name) : m_name(name), 
                                                            m_vertices(0), 
                                                            m_indexes(0),
                                                            m_mesh()  { }


/**
 * @brief Construct and map a mesh 
 *  
 * This constructor will map mesh data provided in the parameters.  
 * 
 * @author 2019-09-20 Kris Becker
 * 
 * @param nvertices Number of 3-elemnent verticies in the mesh
 * @param vertices  An array of vertices of (nvertices * 3) 0 elements
 * @param nindexes  Thee indexes 
 * @param indexes 
 * @param name 
 * @param config 
 */
  BulletMeshMapper::BulletMeshMapper(const int nvertices,  const double *vertices, 
                                     const int nindexes,  const int *indexes,
                                     const int numparts, const QString &name) : 
                                     m_name(name), m_vertices(0), m_indexes(0), 
                                     m_mesh() {

      // Add the vertices/index set
        addArray(nvertices, vertices, nindexes, indexes, numparts);
    }

  /**
   * @brief Construct from an existing Bullet mesh 
   *  
   * This constructor can be used to provide a map into an existing Bullet mesh as 
   * represented in an indexed array object. The mesh array and all its 
   * components are assumed to be valid and are not checked or validated. 
   *  
   * Note there are important assumptions made in the determination of vertex and 
   * index counts with an existing map. It is assumed that if a base address of 
   * the vertices is contained in multiple parts, then the first occuring part 
   * with the base vertex memory address provides all the verticies related to 
   * this address. In other words, if the same vertex address occurs on more than 
   * one part, on the count of vertices in the first part is added to the total 
   * counts. This is not so for the index counts as these considered unique or 
   * redundant but all indexes in all parts contribute to the total count of 
   * indexes. If the existing mesh has any other situation, consider using the 
   * altertive constructor that provides total counts for indexes and vertices. 
   * 
   * @author 2019-10-02 Kris Becker
   * 
   * @param mesh  An existing Bullet mesh index object
   * @param name  Name of the mesh
   */
  BulletMeshMapper::BulletMeshMapper(const btTriangleIndexVertexArray &mesh, 
                                     const QString &name) : 
                                     m_name(name), m_vertices(0), m_indexes(0), 
                                     m_mesh(mesh) {

      // If the vertex and indexes are not provided, 
      // determine the vertex and index counts
      QVector<const unsigned char *> basememory;
      const IndexedMeshArray &v_parts = mesh.getIndexedMeshArray();
      for (int p = 0 ; p < m_mesh.getNumSubParts() ; p++) {
          m_indexes += (v_parts[p].m_numTriangles);

          // Only add vertex count if new memory address
          if ( !basememory.contains( v_parts[p].m_vertexBase ) ) {
              m_vertices += v_parts[p].m_numVertices;
              basememory.push_back( v_parts[p].m_vertexBase );
          }
       }
  }


  /**
   * @brief Construct from an existing Bullet mesh 
   *  
   * This constructor can be used to provide a map into an existing Bullet mesh as 
   * represented in an indexed array object. The mesh array and all its 
   * components are assumed to be valid and are not checked or validated. 
   *  
   * @author 2019-10-02 Kris Becker
   * 
   * @param mesh  An existing Bullet mesh index object
   * @param name  Name of the mesh
   */
  BulletMeshMapper::BulletMeshMapper(const btTriangleIndexVertexArray &mesh, 
                                     const int nvertices, const int nindexes,
                                     const QString &name) : 
                                     m_name(name), 
                                     m_vertices(nvertices),
                                     m_indexes(nindexes), 
                                     m_mesh(mesh) {

  }
      

  /**
   * Destructor
   */
  BulletMeshMapper::~BulletMeshMapper() { 

  }

  void BulletMeshMapper::setName(const QString &name) {
      m_name = name;
  }


  /** Return the name of the mesh   */
  QString BulletMeshMapper::name() const {
    return ( m_name );
  }


  /** Return the number of parts used in the current state of the map   */
  int BulletMeshMapper::nparts() const {
    return ( m_mesh.getNumSubParts() );
  }


  /**
   * @brief Return the number of triangles/facets in the mesh
   * 
   * @return @b int The number of triangles. If nothing has been loaded, then 0 is returned.
   */
  int BulletMeshMapper::ntriangles() const {
    return ( m_indexes );
  }

  /**
   * Return the number of indexes in the shape
   * 
   * @return @b int The number of indexes. If nothing has been loaded, then 0 is 
   *         returned.
   */
  int BulletMeshMapper::nvertices() const {
    return ( m_vertices );
  }


/**
 * @brief Determine if the virtual index is valid in the current mesh 
 *  
 * This mapper uses 0-based indexes. Valid virtual indexes are 0 to 
 * (ntriangles() - 1). 
 *  
 * @param virtualIndex The (linear) index to test for validity
 * 
 * @return bool True if is valid, false otherwise.
 */
  bool BulletMeshMapper::isIndexValid(int virtualIndex) const {
      if (virtualIndex < 0) return ( false );
      if (virtualIndex >= ntriangles() ) return ( false );
      return ( true );
  }

/** 
 * @brief Computes the virtual (linear) index into mesh given part and index 
 * 
 * @param partno Part number into index is mapped to
 * @param index  Index into part 
 * 
 * @return int   If valid, returns the virtual index. If invalid it returns -1. 
 *               The returned index shoula always be verified 
 */
  int BulletMeshMapper::getVirtualIndex(const int partno, const int index) const {

    const IndexedMeshArray &mesh = meshArray();

    // Should this be an exception?
    if ( (partno < 0) || ( partno > mesh.size()) || (index < 0) ) {
        return ( -1 );
    }

    int vndx(0);
    for ( int p = 0 ; p < partno; p++) {
      vndx += mesh[p].m_numTriangles;
    }

    // Exception??
    if ( index > mesh[partno].m_numTriangles ) {
        return ( -1 );
    }
    
    return ( vndx + index );
  }


/**
 * @brief Compute part number and index in map from virtual index 
 *  
 * This method will compute the index into the mesh array (part) and the offset 
 * into the indexes in that part as it pertains to the virtual (linear) index 
 * accross the whole mesh index set. 
 * 
 * @param virtualIndex Virtual (linear) index into map
 * @param partno       Returns the part (mesh index) that maps to the virtual 
 *                     index
 * @param index        Offset in the part (mesh index) to the veretx of the 
 *                     virtual index
 * 
 * @return bool        Returns true if the virtual index maps properly 
 *                     into the mesh arrays otherwise returns false
 */
  bool BulletMeshMapper::getIndexMap(const int virtualIndex, int &partno, 
                                  int &index) const {

    partno = index = -1;
    if ( virtualIndex >=  0 ) {
      int ndx(0);
      const IndexedMeshArray &mesh = meshArray();
      for (partno = 0 ; partno < mesh.size() ; partno++) {
        if ( ( ndx + mesh[partno].m_numTriangles ) > virtualIndex ) {
          index = (virtualIndex - ndx);
          return ( true );
        }
        ndx += mesh[partno].m_numTriangles;
      }
    }
      
    // Real indexes undetermined - invalid virtualIndex
    return ( false );
  }

  const btTriangleIndexVertexArray &BulletMeshMapper::mesh() const {
    return ( m_mesh );
  }


  btTriangleIndexVertexArray &BulletMeshMapper::mesh() {
    return ( m_mesh );
  }

/**
 * @brief Returns a reference to the Bullet index mesh array 
 * 
 * @return const IndexedMeshArray& Reference to mesh array
 */
  const IndexedMeshArray &BulletMeshMapper::meshArray() const {
    return ( m_mesh.getIndexedMeshArray() );
  }

/**
 * @brief Returns a mesh structure (part) based upon part number
 * 
 * @param partno Array index (part) to return
 * 
 * @return const btIndexedMesh& The part
 */
  const btIndexedMesh &BulletMeshMapper::getPart(const int partno) const {
    btAssert ( partno >= 0 );
    btAssert ( partno < nparts() ); 
    return ( m_mesh.getIndexedMeshArray()[partno] );
  }

  const btIndexedMesh &BulletMeshMapper::getPartFromVirtualIndex(const int virtual_index,
                                                                 int &index) const {
     int partno;
     if ( !getIndexMap(virtual_index, partno, index) ) {
       QString mess = "Invalid index (" + QString::number(virtual_index) +
                      ") in mesh map";
       throw IException(IException::Programmer, mess, _FILEINFO_);
     }
     return (getPart(partno) );
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
  btMatrix3x3 BulletMeshMapper::getTriangle(const int partno, const int index) const {

    // std::cout << "getTriangle(" << partno << "," << index << ")\n";
    const btIndexedMesh &v_mesh = getPart(partno);

     // Set up pointers to triangle indexes
    const int *t_index = static_cast<int32_t *> ((void *) v_mesh.m_triangleIndexBase);
    int p_index = 3 * index;
    int vndx0 = t_index[p_index]   * 3;
    int vndx1 = t_index[p_index+1] * 3;
    int vndx2 = t_index[p_index+2] * 3;
    // std::cout << "Indexes("<< vndx0 <<","<<vndx1 <<","<<vndx2 <<")\n";

    const btScalar *t_vertex = static_cast<const btScalar *> ((void *) v_mesh.m_vertexBase);

    // std::cout << "Tvertex("<< t_vertex[vndx0] <<","<<t_vertex[vndx0+1]<<","<<t_vertex[vndx0+2]<<")\n";
    btMatrix3x3 triangle(t_vertex[vndx0+0], t_vertex[vndx0+1], t_vertex[vndx0+2], 
                         t_vertex[vndx1+0], t_vertex[vndx1+1], t_vertex[vndx1+2],
                         t_vertex[vndx2+0], t_vertex[vndx2+1], t_vertex[vndx2+2]);
    return ( triangle );
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
  btMatrix3x3 BulletMeshMapper::getTriangle(const int virtualIndex) const {
    int partno, index;
     if ( !getIndexMap(virtualIndex, partno, index) ) {
       QString mess = "Invalid index (" + QString::number(virtualIndex) +
                      ") for triangle request";
       throw IException(IException::Programmer, mess, _FILEINFO_);
     }

     // std::cout << "getTriangle( " << virtualIndex << ")  = " << partno << "," << index << "\n";

    return ( getTriangle(partno, index) );
  }

  /**
   * @brief Convenience method for retrieving a facet (i.e., triangle)
   *  
   * This method simply utilizes familar terminology for those preferring this 
   * context. 
   *  
   */
  btMatrix3x3 BulletMeshMapper::getFacet(const int virtualIndex) const {
      return ( getTriangle( virtualIndex ) );
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
  btVector3 BulletMeshMapper::getNormal(const int virtualIndex) const {
    btMatrix3x3 triangle = getTriangle(virtualIndex);
    btVector3 edge1 = triangle.getRow(1) - triangle.getRow(0);
    btVector3 edge2 = triangle.getRow(2) - triangle.getRow(0);
    return ( edge1.cross( edge2 ).normalize() );
  }


  void BulletMeshMapper::clear() {
      m_vertices = m_indexes = 0;
      m_mesh = MeshMap();
  } 


/**
 * @brief Add a new array of vertices and indexes to map 
 *  
 * This method creates a series of meshes from a large array of vertices and 
 * indexes. It will add to the exist array as long as it this set does not 
 * exceed the limits of parts and vertices of the Bullet constraints or that 
 * which builds on existing meshes. 
 * 
 * @author 2019-09-21 Kris J. Becker, University of Arizona 
 * 
 * @param nvertices 
 * @param vertices 
 * @param nIndexes 
 * @param indexes 
 * @param nparts 
 * 
 * @return int Number of parts added
 */
  int BulletMeshMapper::addArray(const int nvertices, const double *vertices, 
                                 const int nindexes,  const int *indexes,
                                 const int numparts) {

#if defined(DEBUG)
      std::cout << "---> addArray <---\n";
      std::cout << "Vertices: " << nvertices << "\n";

      for (int i = 0 ; i < nvertices ; i++) {
          std::cout << "v[" << i << "]: " << vertices[i] << "\n";
      }

      std::cout << "\nIndexes:  " << nindexes << "\n";
      for (int v = 0 ; v < nindexes ; v++) {
          std::cout << "i[" << v << "]: " << indexes[v] << "\n";
      }
#endif
      
    // Determine the number of triangles/part for the map
    int v_triangles  = nindexes;
    if ( v_triangles != nindexes ) {
        QString mess = "Number of indexes" + QString::number(nindexes) + 
                       " must be even multiple of 3!";
        throw IException(IException::Programmer, mess, _FILEINFO_);
    }

    int v_vrt_stride = sizeof(double) * 3;
    int v_ndx_stride = sizeof(int) * 3;

    int unused_parts     = MaxPartsPerBody() - nparts();
    int min_parts_needed = ( ( v_triangles - 1 ) / MaxTrianglesPerPart() ) + 1;
    // std::cout << "UnusedParts: " << unused_parts << "\nMinParts: " << min_parts_needed << "\n";

    // Check to see if we can provide the mapping
    if ( min_parts_needed > unused_parts ) {
        QString mess = "Cannot map " + QString::number(v_triangles) + 
                       " triangles as it needs " + QString::number(min_parts_needed) +
                       " parts but have " + QString::number(unused_parts) +
                       " available!";
        throw IException(IException::Programmer, mess, _FILEINFO_);
    }

    // Now compute the number parts based upon requested. If few traingles, 
    // don't oversubscribe.
    int v_parts = max(min_parts_needed, numparts);
    v_parts     = min(v_triangles, v_parts);

    // Might as well distribute the triangles equitably. This is an estimate
    // and discreet triangle mapping will need to refined in the subsequent
    // processing loop.
    // int v_triangles_per_part = (v_triangles + v_parts - 1)/ v_parts;
    int v_triangles_per_part = (v_triangles - 1)/ v_parts + 1;
    // std::cout << "Triangles/Part: " << v_triangles_per_part << "\n";

    int nt_mapped(0);
    const unsigned char *base_vert = reinterpret_cast<const unsigned char *> (vertices);
    const unsigned char *base_ndx  = reinterpret_cast<const unsigned char *> (indexes);
    int nparts(0);
    for (int p = 0 ; nt_mapped < v_triangles ; p++, nparts++) {

        // Set mesh parameters appropriately
        btIndexedMesh v_mesh;
        v_mesh.m_vertexType = PHY_DOUBLE;

        // Set data for triangle indexes
        int ntm = v_triangles - nt_mapped;
        int v_plates = min(ntm, v_triangles_per_part);
        // std::cout << "Part: " << p << ", Plates: " << v_plates << "\n";

        v_mesh.m_numTriangles = v_plates;
        v_mesh.m_triangleIndexBase = base_ndx;
        v_mesh.m_triangleIndexStride = v_ndx_stride;

        // Set and allocate vertex data
        v_mesh.m_numVertices = nvertices;
        v_mesh.m_vertexBase = base_vert;
        v_mesh.m_vertexStride = v_vrt_stride;

        m_mesh.addIndexedMesh(v_mesh, PHY_INTEGER);

        // Only need to update the index pointer for each part
        base_ndx += (v_plates * v_ndx_stride);
        nt_mapped += v_plates;
    }

    m_vertices += nvertices;
    m_indexes  += nindexes;

    return (nparts);
  }


  int BulletMeshMapper::MaxTrianglesPerPart() {
      return ( 1<<(31-MAX_NUM_PARTS_IN_BITS)  );
  }

  int BulletMeshMapper::MaxPartsPerBody() {
     return ( 1<<MAX_NUM_PARTS_IN_BITS );
  }

  BigInt BulletMeshMapper::MaxFacets() {
    return ( (BigInt) MaxTrianglesPerPart() * (BigInt) MaxPartsPerBody() );
  }
}  // namespace Isis
