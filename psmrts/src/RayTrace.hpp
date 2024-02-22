#ifndef RayTrace_hpp
#define RayTrace_hpp


#include <string>
#include <exception>
#include <Eigen/Geometry>

#include <NaifUtilities.hpp>

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

    private:
      RayTraceDatum m_trace_datum;

  };
}

#endif
