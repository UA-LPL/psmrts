#ifndef PsmrtsBulletMeshMap_hpp
#define PsmrtsBulletMeshMap_hpp

#include <string>
#include <memory>

#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>

#include "BulletSystemModel.hpp"


namespace psmrts::bullet {

  /**! Special definitions of the Bullet configuration */
  typedef int            BulletNativeIndexType; // Typo Fix - remove prior to push
  typedef btScalar       BulletNativeVertexType;

  typedef PsmrtsVector3<BulletNativeIndexType>       BulletIndexArray;
  typedef PsmrtsVector3<BulletNativeVertexType>      BulletVectorArray;

/**
 * @brief PsmrtsBulletMeshMap provides a Bullet mesh interface
 * 
 * This class is designed to be Bullet-proof! in that the pointer to the 
 * Bullet mesh must be valid for the life of the tracer instance of the
 * Bullet system. This class is designed to be copyable and still
 * retain valid references.
 * 
 * Use this class to create the Bullet mesh partitioning system of large
 * data sets. You can maintain a copyable array of the Bullet mesh
 * partition map.
 * 
 * Note it is up to the caller to ensure the mesh data buffer sizes do
 * not exceed any Bullet limits, e.g., facets/part.
 * 
 * This class is a template that is build upon the PsmrtsMeshData data buffer
 * API that is itself templated. 
 * 
 * See https://github.com/bulletphysics/bullet3/blob/master/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h
 * for a description of the Bullet mesh API used here.
 *
 * @author Kris J. Becker, University of Arizona
 * @history 2024-05-15 Kris J. Becker  Original Version
 */
  class PsmrtsBulletMeshMap : public PsmrtsMeshData {
    public:
      typedef PsmrtsMeshData::PsmrtsDataType    PsmrtsDataType;

      /** Default constructor */
      PsmrtsBulletMeshMap( );

      /** Construct an array of values */
      PsmrtsBulletMeshMap( const PsmrtsMeshData &mesh,
                           const std::string &name, 
                           const int mesh_id,
                           const int partno = 0 );

      PsmrtsBulletMeshMap( const PsmrtsOBJFormat &obj_t,
                           const PsmrtsDataType v_data_type = PsmrtsDouble,
                           const int mesh_id = 0, 
                           const int partno = 0 );

      /** Destructor */
      virtual ~PsmrtsBulletMeshMap() = default;

      /** Returns mesh name, ie. bullet */
      const std::string &name() const;

      /** Returns mesh's shape identifier */
      int id() const;

      /** Returns mesh part number */
      int partno() const;

      /** Returns the mesh's type - bullet */
      std::string mesh_type() const;

      /** Returns mesh's data name, type, shape id, and part number */
      std::string mesh_data_id() const;

      /** Returns a pointer to the mesh data */
      const btTriangleIndexVertexArray *mesh() const;

      /** Returns vector of partitioned shapes in the mesh */
      const std::vector<PsmrtsVector3i> &shapes() const;

      /** Creates a triangle mesh mapping with a designated max number of parts */
      btTriangleIndexVertexArray *create_map_mesh( const size_t maxparts = 0 ) const;


      /** [Static?] Create a specialized BVH static triangle mesh for ray tracing */
      static btBvhTriangleMeshShape *create_collision_shape( btTriangleIndexVertexArray *mesh,
                                                                    const bool useCompression = true,
                                                                    const bool buildBvh = true );

      /** Creates a triangle mesh based on internal mesh data */
      btBvhTriangleMeshShape *create_collision_shape( const bool useCompression = true,
                                                              const bool buildBvh = true ) const;

      using PsmrtsMeshData::elapsed_life_time_s;
      using PsmrtsMeshData::track_count;
      using PsmrtsMeshData::performance_snapshot;      

      private:
        /// Variables for the Bullet system
        std::string                                  m_name;     /**! The name of the Bullet shape model */
        int                                          m_shape_id; /**! Identifier of the shape */
        int                                          m_part_no;  /**! Bullet shape part number */
        std::vector<PsmrtsVector3i>                  m_shapes;   /**! Partitions that map shapes */
        mutable std::shared_ptr<btTriangleIndexVertexArray>  m_bullet_mesh; /**! Ensure persistent references */


       void init_mesh_map(const std::string &name = "bullet", 
                                  const int shapeid = 0, 
                                  const int partno = 0 );
        
  };


} // namespace psmrts:bullet

#endif // PsmrtsBulletMeshMap_hpp
