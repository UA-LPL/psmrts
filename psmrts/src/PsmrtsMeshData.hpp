#ifndef PsmrtsMeshData_hpp
#define PsmrtsMeshData_hpp

#include <string>
#include <memory>
#include <exception>

#include <Eigen/Geometry>

#include <PsmrtsDataModel.hpp>
#include <RayTrace.hpp>

namespace psmrts {

/**
 * @brief PsmrtsMeshData provides general storage for facet shape models
 *
 * This class provides the fundamentals of storage for a mesh-type, tessellated
 * plate model. It is designed to store the facet index (integer), vector
 * (double or float) or can also store most other arbitrary types.
 *
 * This design uses the Eigen data type to map the second dimension to make
 * it usable directly in vector oriented systems.
 *
 * The default expected types are:
 * @code
 * typedef int      MeshIndexType;
 * typedef double   MeshVectorType;
 * 
 * # Alternatively, Bullet can also do float data types as well
 * # typedef float   MeshVectorType
 * @endcode
 * 
 * @author Kris J. Becker, University of Arizona
 * @history 2024-04-25 Kris J. Becker  Original Version
 */
  template <typename MeshIndexType, typename MeshVectorType>
    class PsmrtsMeshData {
      public:
        typedef PsmrtsDataModel<MeshIndexType>                MeshIndexData;
        typedef PsmrtsDataModel<MeshVectorType>               MeshVectorData;

        typedef typename MeshIndexData::vector_type           index_type;
        typedef typename MeshVectorData::vector_type          vector_type;

        typedef typename MeshIndexData::const_data_reference  const_index_reference;
        typedef typename MeshVectorData::const_data_reference const_vector_reference;

        typedef RayTrace::FacetDatum                          MeshFacet;

        /** Default constructor */
        PsmrtsMeshData() {
          init( );
        }

        /** Construct an array of values */
        PsmrtsMeshData( const MeshIndexData &mesh_indexes, 
                        const MeshVectorData  &mesh_vectors ) :
                        m_mesh_indexes( ), 
                        m_mesh_vectors( ),
                        m_base_index ( 0 ),
                        m_min_axes( { 0.0, 0.0, 0.0 } ),
                        m_max_axes( { 0.0, 0.0, 0.0 } ), 
                        m_min_radius( 0.0 ),
                        m_max_radius( 0.0 ) {
          init( mesh_indexes, mesh_vectors );
        }

        /** Construct an array of values */
        PsmrtsMeshData( const MeshIndexData &mesh_indexes, 
                        const MeshVectorData  &mesh_vectors,
                        const size_t start_index,
                        const size_t n_data = 0 ) :
                        m_mesh_indexes( ), 
                        m_mesh_vectors(  ),
                        m_base_index( 0 ),
                        m_min_axes(  {0.0, 0.0, 0.0 } ),
                        m_max_axes( { 0.0, 0.0, 0.0 } ),
                        m_min_radius( 0.0 ),
                        m_max_radius( 0.0 ) { 
          init ( mesh_indexes.slice( start_index, n_data ), mesh_vectors );
        }
        
        PsmrtsMeshData( const PsmrtsMeshData &mesh, 
                        const size_t start_index, 
                        const size_t n_data = 0 ) : 
                        m_mesh_indexes( ),
                        m_mesh_vectors(  ),
                        m_base_index( 0 ),
                        m_min_axes( mesh.m_min_axes ),
                        m_max_axes( mesh.m_max_axes ),
                        m_min_radius( mesh.m_min_radius ),
                        m_max_radius( mesh.m_max_radius ) {
            init( mesh.m_mesh_indexes.slice( start_index, n_data ), mesh.m_mesh_vectors );
          }

        /** Destructor */
        virtual ~PsmrtsMeshData() { }

        /** Total number of data T allocated */
        inline size_t nvectors() const {
          return ( m_mesh_vectors.size() );
        }

        /** Total number of data T allocated */
        inline size_t nfacets() const {
          return ( m_mesh_indexes.size() );
        }

        inline size_t real_index( const MeshIndexType index ) const {
          return ( index - m_base_index );
        }

        inline Eigen::Vector3d toStdVector( const MeshVectorType &v )  const {
          return ( Eigen::Vector3d( { v[0], v[1], v[2] } ) );
        }

        inline Eigen::Vector3i toStdIndex( const MeshIndexType &v )  const {
          return ( Eigen::Vector3i( { v[0], v[1], v[2] } ) );
        }

        inline const_index_reference get_index( const MeshIndexType nth ) const {
          return ( this->indexes( ).ref( nth ) );
        }

        inline const_vector_reference get_vector( const MeshIndexType raw_index ) const {
          return ( this->vectors( ).ref( real_index( raw_index ) ) );
        }

        inline static Eigen::Vector3d facet_normal( const MeshFacet &facet ) {
          Eigen::Vector3d a = facet.m_vector2 - facet.m_vector1;
          Eigen::Vector3d b = facet.m_vector3 - facet.m_vector1;
          return ( a.cross( b ).normalized() );
        }

        inline MeshFacet get_facet( const int nth ) const {
            MeshFacet mf;
            const_index_reference vndx = get_index( nth );
            mf.m_indexes = toStdIndex( vndx );
            mf.m_vector1 = toStdVector( get_vector( vndx[0] ) );
            mf.m_vector2 = toStdVector( get_vector( vndx[1] ) );
            mf.m_vector3 = toStdVector( get_vector( vndx[2] ) );
            mf.m_normal  = facet_normal( mf );
            mf.m_has_facet = true;
            return ( mf );
        }

        inline int base_index() const {
          return ( m_base_index );
        }

        inline const MeshIndexData &indexes() const {
          return ( m_mesh_indexes );
        }

        inline const MeshVectorData &vectors() const {
          return ( m_mesh_vectors );
        }

        inline const Eigen::Vector3d &axis_mins() const {
          return ( m_min_axes );
        }

        inline const Eigen::Vector3d &axis_maxs() const {
          return ( m_max_axes );
        }

        inline double minimum_radius() const {
          return ( m_min_radius );
        }

        inline double maximum_radius() const {
          return ( m_max_radius );
        }        
    
        /**
         * @brief Construct a segment of the mesh data 
         * 
         * This method extracts a segement or slice of the facet indexes.
         * One of its primary purposes is to map multiple shapes within a
         * data format efficiently using virtual mapping through shared
         * slicing. 
         * 
         * This method produces a mapped slice of the facet indexes
         * to aid in efficient segmentation. A physical copy can be
         * made using deep_copy().
         * 
         * @param start_index   Starting index range from 0 to size()-1
         * @param n_indexes     Number of indexes to map. If 0, it will
         *                       compute the number using size() - start_index
         * @return PsmrtsMeshData Mesh data segment with shared data references
         */
        inline PsmrtsMeshData mesh_segment( const size_t start_index,
                                            const size_t n_indexes ) const {
                                        
          // Sanity checks are in the vector data
          return ( PsmrtsMeshData( *this, start_index, n_indexes ) );
        }

        /**
         * @brief Generate a deep copy of the mesh data
         * 
         * This method will allocate a new buffer and make a new copy of this
         * data buffer. There are no shared data resources from this object.
         * 
         * This is a convient way to create separate data areas from virtual
         * slices/segments.
         * 
         * @return PsmrtsMeshData Copy of this mesh dataset
         */
        inline PsmrtsMeshData deep_copy() const {
          return ( PsmrtsMeshData( this->m_mesh_indexes.deep_copy(), this->m_mesh_vectors.deep_copy() ) );
        }

      private:
        MeshIndexData   m_mesh_indexes;  // Facet indexes
        MeshVectorData  m_mesh_vectors;  // Vertex Data
        int             m_base_index;    // Facet indexes are 1-based

        Eigen::Vector3d m_min_axes;      // Minimum of axes
        Eigen::Vector3d m_max_axes;      // Maximum of axes

        double          m_min_radius;    // Minimum radius
        double          m_max_radius;    // Maximum radius


        /** Initialize the object */
        inline void init() {
          m_mesh_indexes = MeshIndexData();
          m_mesh_vectors = MeshVectorData();

          m_base_index = 0;

          m_min_axes = Eigen::Vector3d::Zero();
          m_max_axes = Eigen::Vector3d::Zero();

          m_min_radius = 0.0;
          m_max_radius = 0.0;

          return;
        }

        /** Compute initial conditions for the shape model */
        inline void init( const MeshIndexData  &indexes, 
                          const MeshVectorData &vectors )  {

          // Initialize parameters
          init();

          m_mesh_indexes = indexes;
          m_mesh_vectors = vectors;

          // Set up vectors
          if ( vectors.size() > 0 ) {
            m_min_axes = vectors.at( 0 );
            m_max_axes = vectors.at( 0 );

            // Compute and store current radii
            m_min_radius = vectors.at( 0 ).norm();
            m_max_radius = vectors.at( 0 ).norm();

            for ( size_t ndx = 1 ; ndx < vectors.size() ; ndx++ ) {
              auto v = vectors( ndx );
              double v_radius = v.norm();

              // Check min/max radius
              if ( v_radius < m_min_radius  ) {
                m_min_radius = v_radius;
              }
              
              if ( v_radius > m_max_radius ) {
                m_max_radius = v_radius;
              }

              // Check for min/max axis ranges
              for ( int n = 0 ; n < v.size() ; n++) {
                if ( v[n] < m_min_axes[n] ) {
                  m_min_axes[n] = v[n];
                }
                
                if ( v[n] > m_max_axes[n] ) {
                  m_max_axes[n] = v[n];
                }
              }
            }
          }

          // Now compute the starting index
          if ( indexes.size() > 0 ) {
            m_base_index = indexes( 0 )[0];
            for ( size_t ndx = 0; ndx < indexes.size() ; ndx++ ) {
              auto const &facet = indexes( ndx );
              for ( size_t n = 0 ; n < facet.size() ; n++ ) {
                if ( facet[n] < m_base_index ) {
                  m_base_index = facet[n];
                }
              }
            }
          }

        }
    };

    // A few convenient typedef types
    typedef PsmrtsMeshData<int, double>   PsmrtsDoubleMeshData;
    typedef PsmrtsMeshData<int, float>    PsmrtsFloatMeshData;

}  // namespace psmrts

#endif // PsmrtsMeshData_hpp
