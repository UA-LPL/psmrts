#ifndef PsmrtsUtilities_hpp
#define PsmrtsUtilities_hpp

#include <functional>
#include <mutex>

#include <psmrts_version.h>
namespace psmrts {


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

        inline std::mutex &mutex() const {
          return ( *m_mutex );
        }

        inline Datum &datum() {
          return ( m_datum );
        }

        inline const Datum &datum() const {
          return ( m_datum );
        }

        inline size_t use_count() const {
          return ( m_mutex.use_count() );
        }

      private:
        // Needs to be mutable to lock in const methods
        mutable std::shared_ptr<std::mutex> m_mutex;
        Datum                       m_datum;

        void init( const Datum &datum = Datum() ) {
          m_mutex.reset( new std::mutex() );
          m_datum = datum;
          return;
        }
    };

} // namespace psmrts

#endif
