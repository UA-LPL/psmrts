#ifndef PsmrtsRayTrace_hpp
#define PsmrtsRayTrace_hpp

#include <cmath>
#include <string>
#include <exception>
#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>

namespace psmrts {

/**
 * @brief Ray trace result status class
 * 
 * This class is used to retain the state of a ray trace issued by a PSMRTS
 * ray tracer. This container is designed to retain the state of a ray
 * trace from an observer sufficient to compute other useful parameters.
 * 
 * It is mainly use for tessellated plate/facet model based shapes.
 */
  class PsmrtsRayTrace {
    public:


      /** Facet data struture contains the elements defining a single facet */
      typedef struct facet_datum {
        facet_datum( ) : m_has_facet( false ),
                         m_indexes( { -1, -1, -1 } ),
                         m_vector1( { 0.0, 0.0, 0.0 } ),
                         m_vector2( { 0.0, 0.0, 0.0 } ),
                         m_vector3( { 0.0, 0.0, 0.0 } ),
                         m_normal(  { 0.0, 0.0, 0.0 } ) { }
        ~facet_datum() { }

        inline bool isValid() const {
          return ( m_has_facet );
        }
                       
        bool            m_has_facet;
        Eigen::Vector3i m_indexes;
        Eigen::Vector3d m_vector1;
        Eigen::Vector3d m_vector2;
        Eigen::Vector3d m_vector3;
        Eigen::Vector3d m_normal;
      } FacetDatum;

      /** Fundamental ray trace data structure for a trace result */
      typedef struct ray_trace_datum {
        public:
          bool            m_hit;      //! Records if the ray intersects the target

          Eigen::Vector3d m_observer; //! Position of observer (s/c) in body-fixed
          Eigen::Vector3d m_lookdir;  //! Look direction of ray from observer

          Eigen::Vector3d m_xyz;      //! XYZ coordinate in shape model of intersection
          Eigen::Vector3d m_normal;   //! Surface normal at surface intercept

          int             m_plateid;  //! 1-baed plate id/index of facet intercepted
          int             m_segment;  //! Segement (DSK)/identifier of shape source

          /** Generalized initialization */
          ray_trace_datum() { init( ); }
          ~ray_trace_datum() { } 

          /** Returns true ray contains an intercept on the shape */
          inline bool hasHit() const {
            return ( m_hit );
          }

          /** Resets the state to a default condition of no intercept */
          inline void reset() {
            init( );
          }

          /** Resets the state to with observer and look direction vectors */
          inline void reset( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir ) {
            init( );
            m_observer = observer;
            m_lookdir  = lookdir;
          }          

        private:
          /** Initialize the datum to default condition */
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
      PsmrtsRayTrace() : m_trace_datum() { }
      PsmrtsRayTrace( const Eigen::Vector3d &observer, 
                      const Eigen::Vector3d &lookdir ) : 
                m_trace_datum() { 
        m_trace_datum.m_observer = observer;
        m_trace_datum.m_lookdir  = lookdir;
      }
      PsmrtsRayTrace(const RayTraceDatum &ray_t ) : m_trace_datum( ray_t ) { }

      virtual ~PsmrtsRayTrace( ) {  }

      /** Returns true if the trace intercepted the target, false if no intersection */
      inline bool hasHit() const {
        return ( datum().hasHit() );
      }

      /** Returns the position of the observer in bodyfixed frame */
      inline const Eigen::Vector3d &observer( ) const {
        return ( datum().m_observer );
      }
      
      /** Returns the look direction from the observer in bodyfixed frame */
      inline const Eigen::Vector3d &lookdir( ) const {
        return ( datum().m_lookdir );
      }

      /** Returns the normal if the ray trace has an intercept */
      inline const Eigen::Vector3d &normal( ) const {
        return ( datum().m_normal );
      }

      /** Returns vector along look direction to surface */
      inline const Eigen::Vector3d surfpt( ) const {
        return ( xyz() - observer() );
      }

      /** Returns the surface point of intercept relative to body origin */
      inline const Eigen::Vector3d &xyz( ) const {
        return ( datum().m_xyz );
      }

      /** Returns the radius of the surface intercept point */
      inline double radius() const {
        return ( xyz().norm() );
      }

      /** Slant distance is distance from observer to surface intercept point */
      inline double slant_distance() const {
        return ( surfpt().norm() );
      }

      /** Compute the distance between to surface ray trace intercepts */
      inline double distance( const PsmrtsRayTrace &other ) const {
        return ( ( xyz() - other.xyz() ).norm() );
      }

      /** Compute the angle of separation between two vectors */
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

      /** Determines if two surface intercept points are sufficiently near one another */
      inline bool isNear( const PsmrtsRayTrace &other,
                          const double tolerance_km = 1.0e-3 ) const {
        if ( !hasHit() )       { return ( false );  }
        if ( !other.hasHit() ) { return ( false );  }

        return ( distance( other ) <= tolerance_km );
      }

      /** Computes the incidence angle (radians) between two traces */
      inline double incidence( const PsmrtsRayTrace &other,
                               const double invalid = psmrts::null() ) const {
        if ( !hasHit() )       { return ( invalid );  }
        if ( !other.hasHit() ) { return ( invalid );  }

        return ( separation_angle( this->normal(), -other.lookdir() ) );
      }

      /** Computes the emission angle at the surface point intercept */
      inline double emission( const double invalid = psmrts::null() ) const {
        if ( !hasHit() )       { return ( invalid );  }
        return ( separation_angle( this->normal(), -this->lookdir() ) );
      }

      /** Compute the phase angle from a surface point (lookdirs) to two observer positions */
      inline double phase( const PsmrtsRayTrace &other,
                           const double invalid = null() ) const {
        if ( !hasHit() )       { return ( invalid );  }
        if ( !other.hasHit() ) { return ( invalid );  }

        return ( separation_angle( -this->lookdir(), -other.lookdir() ) );
      }

      /** Returns the id of the plate/facet of intercept */
      inline int plateid( ) const {
        return ( datum().m_plateid );
      }

      /** Returns the segment or partition of the target body of intercept */
      inline int segment_number( ) const {
        return ( datum().m_segment );
      }      

      /** Return a const reference to the ray trace data */
      inline const RayTraceDatum &datum() const {
        return ( m_trace_datum );
      }

      /** Return a reference to the ray trace data */
      inline RayTraceDatum &datum() {
        return ( m_trace_datum );
      }

      /** Resets/clears the last result and sets to default state */
      inline void reset( ) {
        datum().reset( );
      }

      /** Resets/clears the last result and sets to observer state */
      inline void reset( const Eigen::Vector3d &observer, 
                         const Eigen::Vector3d &lookdir ) {
        datum().reset( observer, lookdir );
      }

      /** Ensures look direction value is not null or zero, returns false/throws error if case of either condition */
      inline bool validate_lookdir( const bool throwOnError = true ) const {
        if ( psmrts::isnull( this->lookdir() ) || psmrts::isEqual( this->lookdir(), Eigen::Vector3d::Zero() ) ) {
          if ( true == throwOnError ) {
            throw std::runtime_error( "Invalid look direction is zero/undefined" );
          }
          return ( false );
        }

        return ( true );
      }

    private:
      RayTraceDatum m_trace_datum;    //!! The ray trace data

  };
}

#endif
