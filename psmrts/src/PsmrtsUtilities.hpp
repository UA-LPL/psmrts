#ifndef PsmrtsUtilities_hpp
#define PsmrtsUtilities_hpp

#include <algorithm>
#include <functional>
#include <exception>
#include <iterator>
#include <string>
#include <locale>
#include <vector>
#include <ctime>
#include <chrono>
#include <cmath>
#include <limits>
#include <mutex>

#include <Eigen/Geometry>

#include <psmrts_version.h>
namespace psmrts {

  ////--- Timimg functions
  typedef std::chrono::time_point<std::chrono::steady_clock> SYSTEM_CLOCK_TIME;
  
  inline std::time_t current_time() {
    return ( std::time( nullptr ) );
  }

  inline SYSTEM_CLOCK_TIME system_clock_time() {
    return ( SYSTEM_CLOCK_TIME( std::chrono::steady_clock::now() ) );
  }

  inline double elapsed_clock_time_s( const SYSTEM_CLOCK_TIME &start_time,
                                      const SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::seconds>( end_time - start_time ).count() );
  }

  inline double elapsed_clock_time_ms( const SYSTEM_CLOCK_TIME &start_time,
                                       const SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time ).count() );
  }  

  /**
   * @brief Standard, typesafe method to cast shared pointer to another type
   * 
   * This function provides a standard method for casting a shared pointer
   * from one type to another. The returned data type is a shared pointer
   * of type T that is recast from the shared data in this object.
   * 
   * Note this method has no effect if the data contained in this
   * method is a user defined const reference to data within the 
   * callers space.
   * 
   * @tparam T    Data type to cast to, e.g., double, float, int
   * @tparam U    Data type of original shared pointer to cast
   * @param data  Shared pointer of type U
   * @return std::shared_ptr<T> shared pointer to the data_u buffer
   */
  template <class T, class U>
    std::shared_ptr<T> cast_shared_ptr( const std::shared_ptr<U> &data_u )  {
      return ( std::reinterpret_pointer_cast<T>( data_u ) );
    }

  template <class T, class U>
    T cast_to_type( U data_u )  {
      return ( reinterpret_cast<T>( data_u ) );
    }

  ////--- General use functions
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


////---> String utlitities

  inline std::string psmrts_tolower( const std::string &s ) {
    std::string s_t = s;
    std::locale locale;
    auto to_lower = [&locale] (char ch) { return ( std::use_facet<std::ctype<char>>(locale).tolower( ch ) ); };
    std::transform( s_t.begin(), s_t.end(), s_t.begin(), to_lower );
    return ( s_t );    
  }

  inline std::string psmrts_toupper( const std::string &s ) {
    std::string s_t = s;
    std::locale locale;
    auto to_upper = [&locale] (char ch) { return ( std::use_facet<std::ctype<char>>(locale).toupper( ch ) ); };
    std::transform( s_t.begin(), s_t.end(), s_t.begin(), to_upper );
    return ( s_t );
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
        Datum  m_datum;

        /** Fundamental initialization of the object */
        void init( const Datum &datum = Datum() ) {
          m_mutex.reset( new std::mutex() );
          m_datum = datum;
          return;
        }
    };

} // namespace psmrts

#endif
