#ifndef PsmrtsMeshData_hpp
#define PsmrtsMeshData_hpp

#include <string>
#include <memory>
#include <functional>
#include <exception>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsStridingBuffer.hpp>
#include <psmrts/core/PsmrtsVector3.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>

namespace psmrts {

/**
 * @brief PsmrtsMeshData provides general storage for facet shape models
 *
 * This class provides the fundamentals of storage for a mesh-type, tessellated
 * plate model. It is designed to store the facet index (integer), vector
 * (double or float).
 *
 * The constructors provide discreet type instantion but stores the data
 * generically. Methods provide access to striding buffer access and users
 * can create effective access.
 * 
 * @author Kris J. Becker, University of Arizona
 * @history 2024-04-25 Kris J. Becker  Original Version
 */
  class PsmrtsMeshData {

    public:
      typedef enum {PsmrtsNullType, PsmrtsDouble, PsmrtsFloat, PsmrtsInteger } PsmrtsDataType;

    private:
      typedef struct vector_surrogate {
        public:
          typedef PsmrtsMeshData::PsmrtsDataType PsmrtsDataType;
          
          vector_surrogate(  ) {
            m_data_type = PsmrtsNullType;
            d_vectors = PsmrtsVector3d();
            f_vectors = PsmrtsVector3f();
          }

          vector_surrogate( const PsmrtsVector3d &dvectors ) {
            m_data_type = PsmrtsDouble;
            d_vectors = dvectors;
            f_vectors = PsmrtsVector3f();
          }

          vector_surrogate( const PsmrtsVector3f &fvectors ) {
            m_data_type = PsmrtsFloat;
            d_vectors   = PsmrtsVector3d();
            f_vectors   = fvectors;
          }

          ~vector_surrogate() { }

          inline bool isValid() const {
            return ( d_vectors.isValid() || f_vectors.isValid() );
          }

          inline size_t size() const {
            return ( this->buffer().size() );
          }

          inline PsmrtsDataType type() const {
            return ( m_data_type );
          }

          inline bool isDouble() const {
            return ( d_vectors.isValid() );
          }

          inline bool isFloat() const {
            return ( f_vectors.isValid() );
          }

          inline Eigen::Vector3d operator()( const int index ) const {
            if ( this->isDouble() ) {
              return ( d_vectors( index ) );
            }
            else {
              auto fv = f_vectors( index );
              return ( Eigen::Vector3d( { fv[0], fv[1], fv[2] } ) );
            }
          } 

          inline const PsmrtsStridingBuffer &buffer() const {
            if ( isDouble() ) return ( d_vectors.buffer() );

            // Assume float...
            return ( f_vectors.buffer() ) ;  
          }

          inline const PsmrtsVector3d &double_vectors() const {
            return ( d_vectors );
          }

          inline const PsmrtsVector3f &float_vectors() const {
            return ( f_vectors );
          }



        private:
          // Only one type should be active
          PsmrtsDataType m_data_type;
          PsmrtsVector3d d_vectors;
          PsmrtsVector3f f_vectors;

      } VectorSurrogate;

    public:
      typedef struct vector_surrogate                       MeshVector;
      typedef PsmrtsRayTrace::FacetDatum                    FacetDatum;

      /** Default constructor */
      PsmrtsMeshData() {
        reset_mesh( );
      }

      PsmrtsMeshData( const PsmrtsMeshData &mesh ) = default;

      /** Construct an array of values */
      PsmrtsMeshData( const PsmrtsVector3i &mesh_indexes, 
                      const PsmrtsVector3d &mesh_vectors_d ) {
        init_mesh( mesh_indexes, mesh_vectors_d );
      }

      /** Construct an array of values */
      PsmrtsMeshData( const PsmrtsVector3i &mesh_indexes, 
                      const PsmrtsVector3f &mesh_vectors_f ) {
        init_mesh( mesh_indexes, mesh_vectors_f );
      }
      
      /** Destructor */
      virtual ~PsmrtsMeshData() { }

      /** Returns validity state of the mesh */
      inline bool isValid() const {
        return ( this->vectors().isValid() );
      }

      /** Total number of data T allocated */
      inline size_t nvectors() const {
        return ( m_vectors.buffer().size() );
      }

      /** Returns the data type of the vectors - float / double */
      inline PsmrtsDataType vector_type( ) const {
        return ( m_vectors.type() );
      }

      /** Returns boolean confirmation if vectors are double */
      inline bool isVectorDouble() const {
        return ( this->vector_type() == PsmrtsDouble );
      }

      /** Returns reference to vector data */
      inline const MeshVector &vectors() const {
        return ( m_vectors );
      }

      /** Total number of data T allocated */
      inline size_t nfacets() const {
        return ( m_indexes.size() );
      }

      /** Returns reference to index data */
      inline const PsmrtsVector3i &indexes() const {
        return ( m_indexes );
      }

      /** Returns index value specified by parameter-designated index position */
      inline Eigen::Vector3i get_index( const int index = 0 ) const {
        return ( m_indexes( index ) );
      }

      /** Returns vector value specified by parameter-designated index position */
      inline Eigen::Vector3d get_vector( const int index = 0  ) const {
        return ( m_vectors( index ) );
      }

      /** Returns the computed normal vector of the facet parameter */
      inline static Eigen::Vector3d facet_normal( const FacetDatum &facet ) {
        Eigen::Vector3d a = facet.m_vector2 - facet.m_vector1;
        Eigen::Vector3d b = facet.m_vector3 - facet.m_vector1;
        return ( a.cross( b ).normalized() );
      }

      /** Returns the nth facet of the mesh */
      inline FacetDatum get_facet( const int nth, const int segmentid = -1 ) const {
          FacetDatum mf;

          const bool ThrowIfInvalid = false;
          if ( true == m_indexes.validate_index( nth, ThrowIfInvalid ) ) {
            auto vndx = m_indexes( nth );

            mf.m_indexes = vndx;
            mf.m_plateid = nth;
            mf.m_segment = segmentid;
            mf.m_vector1 = this->get_vector( vndx[0] );
            mf.m_vector2 = this->get_vector( vndx[1] );
            mf.m_vector3 = this->get_vector( vndx[2] );
            mf.m_normal  = facet_normal( mf );
            mf.m_has_facet = true;
          }
          return ( mf );
      }

      /** Returns reference to the mesh's minimum axis */
      inline const Eigen::Vector3d &axis_mins() const {
        return ( m_min_axes );
      }

      /** Returns reference to the mesh's maximum axis */
      inline const Eigen::Vector3d &axis_maxs() const {
        return ( m_max_axes );
      }

      /** Returns the minimum radius of the mesh */
      inline double minimum_radius() const {
        return ( m_min_radius );
      }

      /** Returns the maximum radius of the mesh */
      inline double maximum_radius() const {
        return ( m_max_radius );
      }     

    protected:

      /** Initialize the object */
      inline void reset_mesh() {
        m_indexes = PsmrtsVector3i();
        m_vectors = VectorSurrogate();

        m_min_axes = Eigen::Vector3d::Zero();
        m_max_axes = Eigen::Vector3d::Zero();

        m_min_radius = 0.0;
        m_max_radius = 0.0;

        m_tracker    = PsmrtsThreadSafeCounter();

        return;
      }

      /** Compute initial conditions for the shape model */
      inline void init_mesh( const PsmrtsVector3i &indexes, 
                             const PsmrtsVector3d &vectors_d )  {

        // Initialize parameters
        reset_mesh();

        m_indexes = indexes;
        m_vectors = VectorSurrogate( vectors_d );
       
        compute_radii();
      }

      /** Compute initial conditions for the shape model */
      inline void init_mesh( const PsmrtsVector3i &indexes, 
                             const PsmrtsVector3f &vectors_f )  {

        // Initialize parameters
        reset_mesh();

        m_indexes = indexes;
        m_vectors = VectorSurrogate( vectors_f );

        compute_radii();
      }

      inline double elapsed_life_time_s() const {
        return ( m_tracker.runtime_s() );
      }

      inline size_t track_count() const {
        return ( m_tracker.count() );
      }

      /**
       * @brief Return a standalone clone of the current tracker stats
       *  
       * Get a snapshot of the performance at this moment. I'd immediately get
       * an end_time = system_clock_time
       * 
       * @return PsmrtsThreadSafeCounter 
       */
      inline PsmrtsThreadSafeCounter performance_snapshot() const {
        return ( m_tracker.clone() );
      }

      // product specification function TODO
  
    private:
      VectorSurrogate         m_vectors;  // Vertex data (float or double)
      PsmrtsVector3i          m_indexes;  // Facet/triangle/plate indexes

      Eigen::Vector3d         m_min_axes;      // Minimum of axes
      Eigen::Vector3d         m_max_axes;      // Maximum of axes

      double                  m_min_radius;    // Minimum radius
      double                  m_max_radius;    // Maximum radius

      PsmrtsThreadSafeCounter m_tracker;       // Tracks times and copy counts


      inline void compute_radii() {
        
        // Set up vectors
        if ( this->vectors().size() > 0 ) {

          // Initialize with first element
          Eigen::Vector3d v0 = this->get_vector( 0 );
          m_min_axes = { v0[0], v0[1], v0[2] };
          m_max_axes = m_min_axes;

          // Compute and store current radii
          m_min_radius = v0.norm();
          m_max_radius = v0.norm();

          // Consider the remaining vertices
          for ( size_t ndx = 1 ; ndx < this->vectors().size() ; ndx++ ) {
            auto v = this->get_vector( ndx );
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
      }
  };

}  // namespace psmrts

#endif // PsmrtsMeshData_hpp
