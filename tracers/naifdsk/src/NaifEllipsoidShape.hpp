#ifndef NaifEllipsoidShape_hpp
#define NaifEllipsoidShape_hpp


#include <string>
#include <exception>
#include <Eigen/Geometry>

#include <NaifUtilities.hpp>

namespace naif {


  class NaifEllipsoidShape {
    public:
      // Constructors
      NaifEllipsoidShape() : m_a_radius( 1.0), 
                             m_b_radius( 1.0 ), 
                             m_c_radius( 1.0 ) { }
      NaifEllipsoidShape(const Eigen::Vector3d &radii ) : m_a_radius( radii[0] ), 
                                                          m_b_radius( radii[1] ), 
                                                          m_c_radius( radii[2] ) { }
      NaifEllipsoidShape(const double radius ) : m_a_radius( radius ), 
                                                 m_b_radius( radius ), 
                                                 m_c_radius( radius ) { }
      NaifEllipsoidShape(const double &a, const double &c ): m_a_radius( a ), 
                                                             m_b_radius( a ), 
                                                             m_c_radius( c ) { }
      NaifEllipsoidShape(const double &a, const double &b, const double &c ) : 
                         m_a_radius( a ), m_b_radius( b ), m_c_radius( c ) { }

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
        
        return ( found != SPICEFALSE );
      }

      inline Eigen::Vector3d normal( const Eigen::Vector3d &point ) const {  
        Eigen::Vector3d normvec;
        (void) surfnm_c( a(), b(), c(), point.data(), normvec.data() );
        return ( normvec );
      }

    private:
      double    m_a_radius;
      double    m_b_radius;
      double    m_c_radius;
  };
}

#endif
