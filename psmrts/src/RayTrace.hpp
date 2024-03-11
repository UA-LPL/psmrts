#ifndef RayTrace_hpp
#define RayTrace_hpp

#include <cmath>
#include <string>
#include <exception>
#include <Eigen/Geometry>

namespace psmrts {

  class RayTrace {
    public:

      typedef struct facet_datum {
        bool            m_has_facet;
        Eigen::Vector3i m_indexes;
        Eigen::Vector3d m_vector1;
        Eigen::Vector3d m_vector2;
        Eigen::Vector3d m_vector3;
      } FacetDatum;

      typedef struct ray_trace_datum {
        public:
          bool            m_hit;

          Eigen::Vector3d m_observer;
          Eigen::Vector3d m_lookdir;

          Eigen::Vector3d m_xyz;
          Eigen::Vector3d m_normal;

          int             m_plateid;
          int             m_segment;

          /** Generatized initialization */
          ray_trace_datum() { init( ); }
          ~ray_trace_datum() { } 

          inline bool hasHit() const {
            return ( m_hit );
          }

          inline void reset() {
            init( );
          }

          inline void reset( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir ) {
            init( );
            m_observer = observer;
            m_lookdir  = lookdir;
          }          

        private:
          /** Initialize */
          inline void init( ) {

            m_hit      = false;
            
            m_observer = Eigen::Vector3d::Zero();
            m_lookdir  = Eigen::Vector3d::Zero();
            m_xyz      = Eigen::Vector3d::Zero();
            m_normal   = Eigen::Vector3d::Zero();

            m_plateid  = -1;
            m_segment  = -1;

            return;
          }

      } RayTraceDatum;

    public:
      // Constructors
      RayTrace() : m_trace_datum() { }
      RayTrace( const Eigen::Vector3d &observer, 
                const Eigen::Vector3d &lookdir ) : 
                m_trace_datum() { 
        m_trace_datum.m_observer = observer;
        m_trace_datum.m_lookdir  = lookdir;
      }
      RayTrace(const RayTraceDatum &ray_t ) : m_trace_datum( ray_t ) { }
      virtual ~RayTrace( ) {  }

      inline bool hasHit() const {
        return ( datum().hasHit() );
      }

      inline const Eigen::Vector3d &observer( ) const {
        return ( datum().m_observer );
      }
      
      inline const Eigen::Vector3d &lookdir( ) const {
        return ( datum().m_lookdir );
      }

      inline const Eigen::Vector3d &normal( ) const {
        return ( datum().m_normal );
      }

      inline const Eigen::Vector3d surfpt( ) const {
        return ( xyz() - observer() );
      }

      inline const Eigen::Vector3d &xyz( ) const {
        return ( datum().m_xyz );
      }

      inline double slant_distance() const {
        return ( surfpt().norm() );
      }

      inline double distance( const RayTrace &other ) const {
        return ( ( xyz() - other.xyz() ).norm() );
      }

      static inline double separation_angle( const Eigen::Vector3d &v1, 
                                             const Eigen::Vector3d &v2 ) {

        Eigen::Vector3d v1_n = v1.normalized();
        if ( v1_n.norm() == 0.0 ) {
          return ( 0.0 );
        }

        Eigen::Vector3d v2_n = v2.normalized();
        if ( v2_n.norm() == 0.0 ) {
          return ( 0.0 );
        }

        double v_dot = v1_n.dot( v2_n );
        if ( v_dot > 0.0 ) {
          return ( 2.0 * std::asin( 0.5 * ( v1_n - v2_n).norm() ) );
        }
        else if ( v_dot < 0.0 ) {
          return ( M_PI - ( 2.0 * std::asin( 0.5 * ( v1_n - v2_n).norm() ) ) );
        }
        
        // ( v_dot == 0.0 ) == M_PI2
        return ( M_PI_2 );
      }

      inline bool isNear( const RayTrace &other,
                          const double tolerance_km = 1.0e-3 ) const {
        if ( !hasHit() )       { return ( false );  }
        if ( !other.hasHit() ) { return ( false );  }

        return ( distance( other ) <= tolerance_km );
      }

      inline double incidence( const RayTrace &other,
                               const double invalid = null() ) const {
        if ( !hasHit() )       { return ( invalid );  }
        if ( !other.hasHit() ) { return ( invalid );  }

        return ( separation_angle( this->normal(), -other.lookdir() ) );
      }

      inline double emission( const double invalid = null() ) const {
        if ( !hasHit() )       { return ( invalid );  }
        return ( separation_angle( this->normal(), -this->lookdir() ) );
      }

      /** Compute the phase angle from a surface point (lookdirs) to two observer positions */
      inline double phase( const RayTrace &other,
                           const double invalid = null() ) const {
        if ( !hasHit() )       { return ( invalid );  }
        if ( !other.hasHit() ) { return ( invalid );  }

        return ( separation_angle( -this->lookdir(), -other.lookdir() ) );
      }

      inline int plateid( ) const {
        return ( datum().m_plateid );
      }

      inline int segment_number( ) const {
        return ( datum().m_segment );
      }      

      inline const RayTraceDatum &datum() const {
        return ( m_trace_datum );
      }

      inline RayTraceDatum &datum() {
        return ( m_trace_datum );
      }

      inline void reset( const Eigen::Vector3d &observer, 
                         const Eigen::Vector3d &lookdir ) {
        datum().reset( observer, lookdir );
      }

    private:
      RayTraceDatum m_trace_datum;

  };
}

#endif
