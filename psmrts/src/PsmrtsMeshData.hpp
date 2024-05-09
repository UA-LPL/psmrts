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
 * @author Kris J. Becker, University of Arizona
 * @history 2024-04-25 Kris J. Becker  Original Version
 */

  template < typename MeshIndexData = PsmrtsDataModel<Eigen::Vector3i>,
             typename MeshVectorData = PsmrtsDataModel<Eigen::Vector3d>
           >
    class PsmrtsMeshData {
      public:
        typedef typename MeshIndexData::data_type    MeshFacetIndex;
        typedef typename MeshVectorData::data_type   MeshFacetVector;

        typedef typename MeshFacetIndex::value_type  index_type;
        typedef typename MeshFacetVector::value_type  vector_type;

        typedef RayTrace::FacetDatum                 MeshFacet;

        /** Default constructor */
        PsmrtsMeshData() {
          init( );
        }

        /** Construct an array of values */
        PsmrtsMeshData( const MeshIndexData &mesh_indexes, 
                        const MeshVectorData  &mesh_vectors ) :
                        m_mesh_indexes( mesh_indexes ), 
                        m_mesh_vectors( mesh_vectors ),
                        m_base_index ( 0 ), 
                        m_min_radius( 0.0 ),
                        m_max_radius( 0.0 ) {
          init( mesh_indexes, mesh_vectors );
        }

        /** Construct an array of values */
        PsmrtsMeshData( const MeshIndexData &mesh_indexes, 
                        const MeshVectorData  &mesh_vectors,
                        const int start_index ) :
                        m_mesh_indexes( mesh_indexes ), 
                        m_mesh_vectors( mesh_vectors ),
                        m_base_index ( start_index ), 
                        m_min_radius( 0.0 ),
                        m_max_radius( 0.0 ) {
          init( mesh_indexes, mesh_vectors );
          m_base_index = start_index;
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

        inline size_t real_index( const index_type &index ) const {
          return ( index - m_base_index );
        }

        inline Eigen::Vector3d toStdVector( const MeshFacetVector &v )  const {
          return ( Eigen::Vector3d( { v[0], v[1], v[2] } ) );
        }

        inline Eigen::Vector3i toStdVector( const MeshFacetIndex &v )  const {
          return ( Eigen::Vector3i( { v[0], v[1], v[2] } ) );
        }

        inline MeshFacetIndex &get_index( const index_type nth ) const {
          return ( this->indexes( nth ) );
        }

        inline MeshFacetVector &get_vector( const index_type raw_index ) const {
          return ( this->vectors( real_index( raw_index ) ) );
        }

        inline static Eigen::Vector3d facet_normal( const MeshFacet &facet ) {
          Eigen::Vector3d a = facet.m_vector2 - facet.m_vector1;
          Eigen::Vector3d b = facet.m_vector3 - facet.m_vector1;
          return ( a.cross( b ).normalized() );
        }

        inline MeshFacet get_facet( const int nth ) const {
            MeshFacet mf;
            const MeshFacetIndex &vndx = get_index( nth );
            mf.m_indexes = toStdVector( vndx );
            mf.m_vector1 = toStdVector( get_vector( vndx[0] ) );
            mf.m_vector2 = toStdVector( get_vector( vndx[1] ) );
            mf.m_vector3 = toStdVector( get_vector( vndx[2] )) ;
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

        inline const MeshVectorData vectors() const {
          return ( m_mesh_vectors );
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
          m_mesh_indexes = MeshFacetIndex();
          m_mesh_vectors = MeshFacetVector();

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

          // Set up vectors
          if ( vectors.size() > 0 ) {
            m_min_axes = vectors.at( 0 );
            m_max_axes = vectors.at( 0 );

            // Compute and store current radii
            m_min_radius = m_min_axes.norm();
            m_max_radius = m_max_axes.norm();

            for ( size_t ndx = 1 ; ndx < vectors.size() ; ndx++ ) {
              auto const &v = vectors( ndx );
              double v_radius = v.norm();

              // Check min/max radius
              if ( v_radius < m_min_radius  ) {
                m_min_radius = v_radius;
              }
              else if ( v_radius > m_max_radius ) {
                m_max_radius = v_radius;
              }

              // Check for min/max axis ranges
              for ( int n = 0 ; n < v.size() ; n++) {
                if ( v[n] < m_min_axes[n] ) {
                  m_min_axes[n] = v[n];
                }
                else if ( v[n] > m_max_axes[n] ) {
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
}  // namespace psmrts

#endif // PsmrtsMeshData_hpp
