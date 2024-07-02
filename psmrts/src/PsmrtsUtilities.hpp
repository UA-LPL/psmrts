#ifndef PsmrtsUtilities_hpp
#define PsmrtsUtilities_hpp

#include <functional>
#include <cmath>
#include <limits>
#include <mutex>

#include <Eigen/Geometry>

#include <psmrts_version.h>
namespace psmrts {


  /** Standardize the double NULL value*/
  inline double null() {
    return ( std::numeric_limits<double>::quiet_NaN() );
  }

  /** Test for the NULL value */
  inline bool isnull( const double &v ) {
    return ( std::isnan( v ) );
  }

  /** Convert degrees to radians */
  inline double degrees_to_radians( const double v_d ) {
    return ( ( v_d / 180.0 * M_PI ) );
  }

  /** Convert radians to degrees */
  inline double radians_to_degrees( const double v_r ) {
    return ( ( v_r / M_PI * 180.0 ) );
  }

  /** Convert a date string to ephemeris time */
  inline double to360LongitudeDomain_d( const double longitude_d ) {
    double lon_adj = std::remainder( longitude_d, 360.0 );
    if ( lon_adj < 0.0  ) {
      lon_adj += 360.0;
    }

    return ( lon_adj );
  }

  /** Convert a date string to ephemeris time */
  inline double to180LongitudeDomain_d( const double longitude_d ) {
    double lon_adj = longitude_d;
    if ( lon_adj >= 360.0 ) {
      lon_adj = to360LongitudeDomain_d( lon_adj );
    }
    else if ( lon_adj < -180.0 ) {
      lon_adj = to360LongitudeDomain_d( lon_adj + 180.0 ) - 180.0;
    }

    if ( lon_adj >= 180.0 ) {
      lon_adj -= 360.0;
    }

    return ( lon_adj );
  }

  inline bool isEqual( const Eigen::Vector3d &v1, 
                       const Eigen::Vector3d &v2,
                       const double v_tolerance = 1.0e-12 ) {
    return ( v1.isApprox( v2, v_tolerance ) );                    
  }

  inline Eigen::Vector3d compute_normal( const Eigen::Vector3d &v1, 
                                         const Eigen::Vector3d &v2,
                                         const Eigen::Vector3d &v3 ) {
    Eigen::Vector3d a = v2 - v1;
    Eigen::Vector3d b = v3 - v1;
    return ( a.cross( b ).normalized() );
  }

  /**
   * @brief Constructs a path that is OS sensitive
   * 
   * @param directory    Top level directory 
   * @param pathpart     An optional subdirectory for file
   * @return std::string The constructed path
   */
  inline std::string psmrts_make_path( const std::string &directory, 
                                        const std::string &pathpart = "" ) {

    if ( pathpart.size() == 0 ) {
      return ( directory );
    }

    if ( directory.size() == 0 ) {
      return ( pathpart );
    }

    size_t dirlen = directory.size();
    std::string dpathdelim = ( directory[dirlen-1] == '/' ) ? "" : "/";
    return ( directory + dpathdelim + pathpart );
  }

/**
 * @brief Mutex wrapper for arbitrary data type
 * 
 * This template class provides a copyable (i.e., shared) class object that
 * is designed to help manage data that needs to exist in a threaded
 * environment. 
 * 
 * The Datum type is retained in a local copy within this class. In addtion,
 * a shared mutex is allocated in this class. Any copies of the class will
 * copy both the Datum instance and the shared pointer to the mutex.
 * 
 * @tparam Datum Data type to store and associate with a shared thread locker
 */
  template <typename Datum> 
    class DatumMutexWrapper {
      public:
        DatumMutexWrapper( ) {
          init();
        }
        DatumMutexWrapper( const Datum &datum ) {
          init( datum );
        }
        DatumMutexWrapper( const DatumMutexWrapper &dmm ) :
                          m_mutex( dmm.m_mutex),
                          m_datum( dmm.m_datum ) { }

        DatumMutexWrapper( const std::shared_ptr<std::mutex> &p_mutex, 
                          const Datum &p_datum ) :
                          m_mutex( p_mutex ), m_datum( p_datum ) { }

        ~DatumMutexWrapper()  { }

        /** Return a reference to the mutex for locking purposes */
        inline std::mutex &mutex() const {
          return ( *m_mutex );
        }

        /** Return a reference to the stored Datum */
        inline Datum &datum() {
          return ( m_datum );
        }

        /** Return a const refernce to the stored Datum */
        inline const Datum &datum() const {
          return ( m_datum );
        }

        /** Returns the use count of the shared mutex */
        inline size_t use_count() const {
          return ( m_mutex.use_count() );
        }

      private:
        // Needs to be mutable to lock in const methods
        mutable std::shared_ptr<std::mutex> m_mutex;
        Datum                       m_datum;

        /** Fundamental initialization of the object */
        void init( const Datum &datum = Datum() ) {
          m_mutex.reset( new std::mutex() );
          m_datum = datum;
          return;
        }
    };

} // namespace psmrts

#endif
