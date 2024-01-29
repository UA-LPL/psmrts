#ifndef NaifUtilities_hpp
#define NaifUtilities_hpp

#include <iostream>

#include <cspice/SpiceUsr.h>


namespace naif {

// #include <cspice/SpiceUsr.h>


  inline void setReturnMode( ) {
    SpiceChar retmode[32] = { "RETURN"};
    erract_c( "SET", sizeof( retmode ), retmode );
    return;
  }

  inline void setPrintMode( ) {
    SpiceChar prtmode[32] = { "NONE"};
    errprt_c( "SET", sizeof( prtmode ), prtmode );
    return;
  }

  inline void initKernelSystem() {
    setReturnMode();
    setPrintMode();
    return;
  }

  inline void clearKernelSystem() {
    kclear_c();
    return;
  }

  inline void load_kernel( const std::string &kfile ) {
    furnsh_c( kfile.c_str() );
  }

  inline void unload_kernel( const std::string &kfile ) {
    unload_c( kfile.c_str() );
  }

  inline std::string get_error_msg( ) {
    const int NAIF_ERROR_STRING_SIZE = 2000;
    SpiceChar errmsg[NAIF_ERROR_STRING_SIZE];
    getmsg_c("LONG", NAIF_ERROR_STRING_SIZE, errmsg );
    return ( std::string( errmsg ) );
  }

  /**
   * @brief Check for NAIF errors with behavior control
   * 
   * This function will check for a NAIF error and take requested action.
   * @see get_error_msg()
   * 
   * @param b_reset       If an error has occurred, reset the error system
   * @param throw_on_error Throw a runtime_error if an error occured
   * @return true  If no errror occurs
   * @return false If an error occured
   */
  inline bool check_naif_errors( const bool b_reset = true,
                                 const bool throw_on_error = true ) {

    // Check for an error condition                                  
    if ( !failed_c() ) return ( false );

    if ( throw_on_error ) {
      throw std::runtime_error( "*** NAIF::Error - " + get_error_msg() + " ***" );
    }

    if ( b_reset ) {
      reset_c();
    }
    return ( true );
  }

  /** Convert a date string to ephemeris time */
  inline double isoc_to_et( const std::string &datestr ) {
    SpiceDouble et;
    str2et_c( datestr.c_str() , &et );
    return ( et );
  }

} // namespace naif

#endif
