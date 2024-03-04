#ifndef NaifEllipsoidShape_hpp
#define NaifEllipsoidShape_hpp


#include <string>
#include <exception>
#include <Eigen/Geometry>

#include <NaifUtilities.hpp>
#include <RayTrace.hpp>

namespace naif {


  class NaifEllipsoidShape {
    public:
      // Constructors
      NaifEllipsoidShape() : m_a_radius( 1.0), 
                             m_b_radius( 1.0 ), 
                             m_c_radius( 1.0 ) {  }
      NaifEllipsoidShape(const Eigen::Vector3d &radii ) : m_a_radius( radii[0] ), 
                                                          m_b_radius( radii[1] ), 
                                                          m_c_radius( radii[2] ) { 
        validate();
      }
      NaifEllipsoidShape(const double radius ) : m_a_radius( radius ), 
                                                 m_b_radius( radius ), 
                                                 m_c_radius( radius ) { 
        validate();
      }
      NaifEllipsoidShape(const double &a, const double &c ): m_a_radius( a ), 
                                                             m_b_radius( a ), 
                                                             m_c_radius( c ) { 
        validate();
      }
      NaifEllipsoidShape(const double &a, const double &b, const double &c ) : 
                         m_a_radius( a ), m_b_radius( b ), m_c_radius( c ) {  
        validate();
      }

      // Destructor
      virtual ~NaifEllipsoidShape() { }

      const double &a() const {
        return ( m_a_radius );
      }

      const double &b() const {
        return ( m_b_radius );
      }

      const double &c() const {
        return ( m_c_radius );
      }

      inline Eigen::Vector3d radii() const {
        return ( Eigen::Vector3d( { a(), b(), c() } ) );
      }

      inline double minimum_radius() const {
        return ( std::min( a(), std::min( b(), c() ) ) );
      }

      inline double maximum_radius() const {
        return ( std::max( a(), std::max( b(), c() ) ) );
      }


      inline bool ray_trace( const Eigen::Vector3d &observer, const Eigen::Vector3d &lookdir,
                             Eigen::Vector3d &point) const {
        
        SpiceBoolean found;
        (void) surfpt_c( observer.data(), lookdir.data(), a(), b(), c(), point.data(), &found );
        check_naif_errors();

        return ( found == SPICETRUE );
      }

      inline Eigen::Vector3d normal( const Eigen::Vector3d &point ) const {  
        Eigen::Vector3d normvec;
        (void) surfnm_c( a(), b(), c(), point.data(), normvec.data() );
        return ( normvec );
      }

      inline bool ray_trace( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir,
                             psmrts::RayTrace::RayTraceDatum &raytrace ) const {

        raytrace.reset();
        raytrace.m_observer = observer;
        raytrace.m_lookdir  = lookdir;
        raytrace.m_segment  = 0;
        raytrace.m_plateid  = 0;

        raytrace.m_hit = this->ray_trace( observer, lookdir, raytrace.m_xyz );
        if ( raytrace.hasHit() ) {
          raytrace.m_normal = this->normal( raytrace.m_xyz );
        }

        // Returns intercept state
        return ( raytrace.hasHit() );
      }


    private:
      double    m_a_radius;
      double    m_b_radius;
      double    m_c_radius;

      inline void validate() const {
        if ( minimum_radius() <= 0.0 ) {
          std::string mess = "Invalid radii (" + std::to_string(a()) + "," + 
                              std::to_string(b()) + "," + std::to_string(c()) + " - must be > 0";
          throw std::runtime_error( mess );
        }
        return;
      }
  };
}

#endif
