#ifndef PsmrtsUtilities_hpp
#define PsmrtsUtilities_hpp

namespace psmrts {


  /** Convert a date string to ephemeris time */
  inline double to360LongitudeDomain_d( const double longitude_d ) {
    double lon_adj = longitude % 360.0;
    if ( lon_adj < 0.0  ) {
      lon_adj += 360.0;
    }

    return ( lon_adj );
  }

  /** Convert a date string to ephemeris time */
  inline double to180LongitudeDomain_d( const double longitude_d ) {
    double lon_adj = longitude % -180;
    if ( lon_adj > 180.0 ) {
      lon_adj -= 360.0;
    }

    return ( lon_adj );
  }

} // namespace psmrts

#endif
