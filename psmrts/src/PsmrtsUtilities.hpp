#ifndef PsmrtsUtilities_hpp
#define PsmrtsUtilities_hpp

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif

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
#include <stdexcept>

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
  typedef std::chrono::high_resolution_clock                PSMRTS_SYSTEM_CLOCK_TYPE;
  typedef std::chrono::time_point<PSMRTS_SYSTEM_CLOCK_TYPE> PSMRTS_SYSTEM_CLOCK_TIME;
  
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
  inline PSMRTS_SYSTEM_CLOCK_TIME system_clock_time() {
    return ( PSMRTS_SYSTEM_CLOCK_TIME(  PSMRTS_SYSTEM_CLOCK_TYPE::now() ) );
  }

  /** Returns elapsed time in seconds */
  inline double elapsed_clock_time_hours( const PSMRTS_SYSTEM_CLOCK_TIME &start_time,
                                          const PSMRTS_SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::minutes>( end_time - start_time ).count() / 60.0 );
  }

  /** Returns elapsed time in seconds */
  inline double elapsed_clock_time_s( const PSMRTS_SYSTEM_CLOCK_TIME &start_time,
                                      const PSMRTS_SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time ).count() / 1000.0 );
  }

  /** Returns elapsed time in milliseconds */
  inline double elapsed_clock_time_ms( const PSMRTS_SYSTEM_CLOCK_TIME &start_time,
                                       const PSMRTS_SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ).count() / 1000.0 );
  }  

  /** Returns elapsed time in microseconds */
  inline double elapsed_clock_time_microseconds( const PSMRTS_SYSTEM_CLOCK_TIME &start_time,
                                                 const PSMRTS_SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - start_time ).count() / 1000.0 );
  } 

    /** Returns elapsed time in nanoseconds */
  inline double elapsed_clock_time_nanoseconds( const PSMRTS_SYSTEM_CLOCK_TIME &start_time,
                                                 const PSMRTS_SYSTEM_CLOCK_TIME &end_time ) {
    return ( std::chrono::duration_cast<std::chrono::nanoseconds>( end_time - start_time ).count() );
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
  
  /**
   * @brief latlonrad_to_xyz_d - Convert latitudinal coordinates (longitude,
   *                             latitude, radius) to rectangular (xyz).
   *
   * Given a Eigen::Vector3d containing longitude, latitude, and radius
   * coordinates, this function converts it to rectangular coordinates.
   *
   * Input angular coordinates are assumed to be in degrees. Longitude is
   * converted to the 360 longitude domain if necessary.
   *
   * @param Eigen::Vector3d containing longitude, latitude, radius coordinates.
   * @return Eigen::Vector3d Vector converted to xyz coordinates.
   */
  inline Eigen::Vector3d latlonrad_to_xyz_d( const Eigen::Vector3d &llr_deg ) {

      double lon_r  = degrees_to_radians( to360LongitudeDomain_d( llr_deg[0] ) );
      double lat_r  = degrees_to_radians( llr_deg[1] ) ;
      double radius = llr_deg[2];

      double x = radius * std::cos( lon_r ) * std::cos( lat_r );
      double y = radius * std::sin( lon_r ) * std::cos( lat_r );
      double z = radius * std::sin( lat_r  );

      return ( Eigen::Vector3d( { x, y, z } ) );

      // still need to add toLatitudeDomain
      // Kris below
      // double lon_r = deg2rad( to360Domain( llr_d[0] ) );
      //*** double lat_r = deg2rad( toLatitudeDomain( llr_d[1] ) );
      // double rad_m = llr_d[2];
  }

  /**
   * @brief xyz_to_latlonrad_d - Convert rectangular coordinates (x,y,z) to
   *                             latitudinal coordinates (longitude, latitude,
   *                             radius). Angular coordinates are in degrees.
   *
   * Given a Eigen::Vector3d containing xyz coordinates, this function converts
   * it to longitude, latitude, radius coordinates. Resulting angular
   * coordinates are in degrees.
   *
   * @param Eigen::Vector3d containing xyz coordinates.
   * @param bool Flag to convert resulting longitude coordinate to 360 longitude
   *             domain if necessary. Defaults to true.
   * @return Eigen::Vector3d Vector converted to longitude, latitude, and radius
   *                         coordinates.
   */
  /*  input x,y,z are km, output lat, lon in radians, radius in km */
  inline Eigen::Vector3d xyz_to_latlonrad_d( const Eigen::Vector3d &xyz,
                                             const bool to360 = true ) {

      Eigen::Vector3d llr;

      double vector_max = std::max( { std::abs(xyz[0]),
                                      std::abs(xyz[1]),
                                      std::abs(xyz[2]) } );

      double x = xyz[0] / vector_max;
      double y = xyz[1] / vector_max;
      double z = xyz[2] / vector_max;

      llr[2] = vector_max * sqrt( x*x + y*y + z*z ); // radius
      llr[1] = radians_to_degrees( atan2(z, sqrt( x*x + y*y ) ) );         // latitude

      if (x == 0.0 && y == 0.0 ) {                   // longitude
        llr[0] = 0.0;
      }
      else {
        llr[0] = radians_to_degrees( atan2(y, x) );
      }

      // Adjust longitude for -180, 180 domain if requested
      if ( to360 && ( llr[0] < 0.0 ) ) {
        llr[0] += 360.0;
      }

      return ( llr );
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

    /** Remove leading white space */
    inline std::string psmrts_ltrim( const std::string &s ) {
      std::string strim = s;
      auto bad_c =  std::find_if( strim.begin(), strim.end(), []( const char ch ) -> bool { return !std::isspace<char>(ch , std::locale::classic() ) ; } );
      strim.erase( strim.begin(), bad_c );
      return ( strim );
    }
  
    /** Remove trailing white space */
    inline std::string psmrts_rtrim( const std::string &s ) {
      std::string strim = s;
      auto bad_c =  std::find_if( strim.rbegin(), strim.rend(), []( const char ch ) -> bool { return !std::isspace<char>(ch , std::locale::classic() ) ; } );
      strim.erase( bad_c.base(), strim.end() );
      return ( strim );
    }
  
  /** Remove leading and trailing white space */
    inline std::string psmrts_trim( const std::string &s ) {
      return ( psmrts_ltrim( psmrts_rtrim( s ) ) );
    }

    
  /**
   * @brief tokenization of an Ellipsoid string of a/b/c values
   * 
   * This function is meant to tokenize the string with multiple value
   * character seperators as provided in a string. 
   *
   * It is desiged to accept strings similar to the following forms:
   * - "Tracer:value"
   * - "Tracer:value,value"
   * - "Tracer:value,value,value"
   * 
   * This code parses the string format above and creates a string vector:
   *  - { "Tracer", "value" }
   * @code 
   * auto tokens = string_tokenizer(s, ":,");
   * @endcode
   * 
   * 
   * @param s                         An string in the forms detailed above
   * @param t_sep                     Default separator character, ','.
   * @return std::vector<std::string> Returns a string vector of one or more values, unless provided
   *                                  an incorrectly formatted string input
   */
  inline std::vector<std::string> string_tokenizer(const std::string &s,
                                                   const std::string &t_sep = ",") {
    std::vector<std::string> values;
    std::string::size_type spos = 0;
    size_t s_size = s.length();

    for (std::string::size_type i=0; i != std::string::npos; spos = i+1) {
      i = s.find_first_of(t_sep, spos);
      std::string::size_type slen = (std::string::npos == i) ? i : i - spos;
      if (slen > 0 ) {
        values.push_back( s.substr(spos, slen) );
      }
    }
    return values;
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

  /** Checks if a string is an acceptable boolean value, returning the equivalent if found */
  inline bool isBool( const std::string &val ) {
    std::string target = psmrts_tolower(val);
    static const std::vector<std::string> trues = {"true", "t", "yes", "y", "on", "1"};
    static const std::vector<std::string> falses = {"false", "f", "no", "n", "off", "0"};

    if (std::find(trues.begin(), trues.end(), target) != trues.end() ) {
      return true;
    }
    else if (std::find(falses.begin(), falses.end(), target) != falses.end() ) {
      return false;
    }
    else {
      throw std::runtime_error("Error: Acceptable boolean value not found - " + val);
    }
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

        inline PSMRTS_SYSTEM_CLOCK_TIME start_time() const {
          return ( m_start_time );
        }

        /** Return elapsed time in seconds */
        inline double runtime_s() const {
          return ( psmrts::elapsed_clock_time_s( m_start_time, psmrts::system_clock_time() ) );
        }

        /** Return elapsed time in milliseconds */
        inline double runtime_ms() const {
          return ( psmrts::elapsed_clock_time_ms( m_start_time, psmrts::system_clock_time() ) );
        }

        /** Return elapsed time in nanoseconds */
        inline double runtime_ns() const {
          return ( psmrts::elapsed_clock_time_nanoseconds( m_start_time, psmrts::system_clock_time() ) );
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

        inline void reset_timer() {
          m_born_on_date  = psmrts::current_time();          
          m_start_time    = psmrts::system_clock_time();
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
        psmrts::PSMRTS_SYSTEM_CLOCK_TIME m_start_time;
        mutable SharedCounter     m_counter;

        void init( const size_t counted = 0 ) {
          m_counter.reset( new ThreadSafeCounter( counted ) );
          reset_timer();
        }
    };

} // namespace psmrts

#endif
