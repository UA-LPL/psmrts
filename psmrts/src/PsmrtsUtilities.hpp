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
#include <time.h>
#include <chrono>
#include <cmath>
#include <limits>
#include <mutex>

#include <Eigen/Geometry>

#include <psmrts_version.h>

// Different versions of the JSON library are not ABI compatible.
// When you enable JSON diagnostics, it will also cause similar
// issues because it adds debugging code to the classes at compile
// time. This results in different incompatible objects in other
// code files. Every file that uses JSON data must include this
// file first, and not allow any other direct include of 
// nlohmann/json.hpp or bad things could happen.
#ifndef JSON_DIAGNOSTICS
#define JSON_DIAGNOSTICS 1
#endif
#include <nlohmann/json.hpp>
using namespace nlohmann::literals;
using json         = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
using psmrts_json  = nlohmann::ordered_json;


namespace psmrts {

  inline const std::string &psmrts_version() {
    static const std::string psmrst_version_string( std::string(PROJECT_NAME) + " - [" + std::string(PSMRTS_VERSION) + "]" );
    return ( psmrst_version_string );
  }

  ////--- Timimg functions
  typedef std::chrono::time_point<std::chrono::steady_clock> SYSTEM_CLOCK_TIME;
  
  /** Returns current calendar time */
  inline std::time_t current_time() {
    return ( std::time( nullptr ) );
  }

  /** Returns string representing input time value */
  inline std::string to_time_str( const std::tm *my_tm )  {
    char buffer_t[128];
    strftime( buffer_t, sizeof( buffer_t ), "%FT%T %Z", my_tm );
    return ( std::string( buffer_t ) );
  }

  /** Returns string of time converted to UTC */
  inline std::string to_utc( const std::time_t &my_t )  {
    return ( to_time_str( gmtime( &my_t ) ) );
  }

  /** Returns string of time converted to local timezone */
  inline std::string to_localtime( const std::time_t &my_t )  {
    return ( to_time_str( localtime( &my_t ) ) );
  }

  /** Returns current system clock time */
  inline SYSTEM_CLOCK_TIME system_clock_time() {
    return ( SYSTEM_CLOCK_TIME( std::chrono::steady_clock::now() ) );
  }

  /** Returns elapsed time in seconds */
  inline double elapsed_clock_time_s( const SYSTEM_CLOCK_TIME &start_time,
                                      const SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::seconds>( end_time - start_time ).count() );
  }

  /** Returns elapsed time in milliseconds */
  inline double elapsed_clock_time_ms( const SYSTEM_CLOCK_TIME &start_time,
                                       const SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time ).count() );
  }  

  /** Returns elapsed time in microseconds */
  inline double elapsed_clock_time_microseconds( const SYSTEM_CLOCK_TIME &start_time,
                                                 const SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ).count() );
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

  /** Returns true if provided Eigen::Vector3d contains a null */
  inline bool isnull( const Eigen::Vector3d &v ) {
    for ( size_t i = 0 ; i < v.size() ; i++ ) {
      if ( isnull( v[i] ) ) { 
        return ( true );
      }
    }
    return ( false );
  }

  /** Convert degrees to radians */
  inline double degrees_to_radians( const double v_d ) {
    return ( ( v_d / 180.0 * M_PI ) );
  }

  /** Convert radians to degrees */
  inline double radians_to_degrees( const double v_r ) {
    return ( ( v_r / M_PI * 180.0 ) );
  }

  /** Ensure longitude is in the 360 domain */
  inline double to360LongitudeDomain_d( const double longitude_d ) {
    double lon_adj = std::remainder( longitude_d, 360.0 );
    if ( lon_adj < 0.0  ) {
      lon_adj += 360.0;
    }

    return ( lon_adj );
  }

  /** Ensure longitude is in the 180 domain */
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
  
  /** Returns true if the data of two Eigen::Vector3d have same relative values, with adjustable tolerance */
  inline bool isEqual( const Eigen::Vector3d &v1, 
                       const Eigen::Vector3d &v2,
                       const double v_tolerance = 1.0e-12 ) {
    return ( v1.isApprox( v2, v_tolerance ) );                    
  }

  /** Computes the resulting normal of three Eigen vectors (double) */
  inline Eigen::Vector3d compute_normal( const Eigen::Vector3d &v1, 
                                         const Eigen::Vector3d &v2,
                                         const Eigen::Vector3d &v3 ) {
    Eigen::Vector3d a = v2 - v1;
    Eigen::Vector3d b = v3 - v1;
    return ( a.cross( b ).normalized() );
  }


////---> String utlitities
  /** Returns string completely converted to lower case */
  inline std::string psmrts_tolower( const std::string &s ) {
    std::string s_t = s;
    std::locale locale;
    auto to_lower = [&locale] (char ch) { return ( std::use_facet<std::ctype<char>>(locale).tolower( ch ) ); };
    std::transform( s_t.begin(), s_t.end(), s_t.begin(), to_lower );
    return ( s_t );    
  }

  /** Returns string completely converted to upper case */
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

  /** Returns file type extension, ie. cpp, hpp, etc.. */
  inline std::string psmrts_file_extension( const std::string &pathname ) {
    std::string extension_f( "" );
    if ( pathname.length() > 0 ) {
      auto lastdot = pathname.find_last_of( '.' );
      if ( lastdot != std::string::npos ) {
        extension_f = pathname.substr( lastdot + 1 );
      }
    }

    return ( extension_f );
  }

  /** Returns direct parent directory of file */
  inline std::string psmrts_file_path( const std::string &pathname ) {
    std::string path_f( "" );
    if ( pathname.length() > 0 ) {

      // Find last directory path spec
      auto lastslash = pathname.find_last_of( '/' );
      if ( lastslash != std::string::npos ) {
        path_f = pathname.substr(0,  lastslash );
      }

    }
    return ( path_f );
  }

  /** Returns just the file name, with no preceeding path information */
  inline std::string psmrts_file_basename( const std::string &pathname ) {
    std::string base_f( "" );
    if ( pathname.length() > 0 ) {

      // Find any directory paths...
      auto lastslash = pathname.find_last_of( '/' );
      if ( lastslash != std::string::npos ) {
        base_f = pathname.substr( lastslash + 1 );
      }

      auto lastdot = pathname.find_last_of( '.' );
      if ( lastdot != std::string::npos ) {
        base_f = base_f.substr( 0, lastdot - 1 );
      }

    }
    return ( base_f );
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

    /**
     * @brief Thread safe counter and time class
     * 
     * This class can be used as an accurate system timer and a counter that
     * is available for a frequently used task of counting things.
     * 
     * It has no overhead if you just track runtimes by simply setting up
     * a method PsmrtsThreadSafeCounter varible. The counter is not changed
     *  unless a direct counter call has been made (using hitme() or the 
     * operator++).
     * 
     * By including this class as a method variable in your it is perpetuated
     * to other 
     */
    class PsmrtsThreadSafeCounter {
      public:
        PsmrtsThreadSafeCounter() { 
          init();
        }
        virtual ~PsmrtsThreadSafeCounter() { }

        inline size_t hitme() const {
          std::scoped_lock mylocker( m_counter->mutex() );
          return ( m_counter->datum() += 1 );
        }

        inline size_t operator++( int dummy ) const {
          return  ( this->hitme() );
        }

        inline size_t count() const {
          std::scoped_lock mylocker( m_counter->mutex() );
          return  ( m_counter->datum() );
        }

        inline std::time_t now() const {
          return ( psmrts::current_time() );
        }

        inline std::time_t born_on_date( ) const {
          return ( m_born_on_date );
        }

        inline SYSTEM_CLOCK_TIME start_time() const {
          return ( m_start_time );
        }

        inline double runtime_s() const {
          return ( psmrts::elapsed_clock_time_s( m_start_time, psmrts::system_clock_time() ) );
        }

        /**
         * @brief Clone a new counter from this instance
         * 
         * Cloning a new counter will preserve times but the cloned counter
         * will be severed from this instance and evolve independently. This
         * will also reset the use count as that can now be another 
         * consequence/benefit.
         * 
         * The born-on-date and the system clock is preserved.
         * 
         * @return PsmrtsThreadSafeCounter 
         */
        inline PsmrtsThreadSafeCounter clone() const {
          PsmrtsThreadSafeCounter counter_t( this->count() );
          counter_t.m_born_on_date          = m_born_on_date;
          counter_t.m_start_time            = m_start_time;
          return ( counter_t );
        }

        /** Return a JSON object with a time snapshot */
        inline ordered_json snapshot() const {

          // Get current data
          PsmrtsThreadSafeCounter timer_t;
          ordered_json json_t;

          json_t["start_time"]      = to_localtime( this->born_on_date() );
          json_t["elapsed_time_s"]  = elapsed_clock_time_s( this->start_time(), timer_t.start_time() );;
          json_t["elapsed_time_ms"] = elapsed_clock_time_ms( this->start_time(), timer_t.start_time() ); ;          
          json_t["count"]           = this->count();
          json_t["end_time"]        = to_localtime( timer_t.born_on_date() );

          return ( json_t );

        }

      protected:
        typedef DatumMutexWrapper<size_t>          ThreadSafeCounter;
        typedef std::shared_ptr<ThreadSafeCounter> SharedCounter;

        /** This constructor returns a new instance of a counter with a specified count >= 0 */
        PsmrtsThreadSafeCounter( const size_t counted ) {
          init( counted );
        }
        
      private:
        std::time_t               m_born_on_date;
        psmrts::SYSTEM_CLOCK_TIME m_start_time;
        mutable SharedCounter     m_counter;

        void init( const size_t counted = 0 ) {
          m_start_time   = psmrts::system_clock_time();
          m_born_on_date = psmrts::current_time();
          m_counter.reset( new ThreadSafeCounter( counted ) );
        }
    };

} // namespace psmrts

#endif
