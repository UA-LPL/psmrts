/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef NaifUtilities_hpp
#define NaifUtilities_hpp

#include <string>

#include <cspice/SpiceUsr.h>


namespace naif {

  inline void setReturnMode( const std::string &u_retmode = "RETURN" ) {

    int retmode_len = u_retmode.size();
    constexpr int MAXLEN = 1024;
    SpiceChar retmode[MAXLEN];

    int maxchars = std::min( retmode_len+1, MAXLEN-1 );
    std::strncpy( retmode, u_retmode.c_str(), maxchars );

    erract_c( "SET", MAXLEN, retmode );
    return;
  }

  inline void setPrintMode( const std::string &u_prtmode = "NONE" ) {

    int prtmode_len = u_prtmode.size();

    constexpr int MAXLEN = 1024;
    SpiceChar prtmode[MAXLEN];

    int maxchars = std::min( prtmode_len+1, MAXLEN-1 );
    std::strncpy( prtmode, u_prtmode.c_str(), maxchars );
  
    errprt_c( "SET", MAXLEN, prtmode );
    return;
  }

  inline void clearKernelSystem() {
    kclear_c();
    return;
  }

  inline void initKernelSystem( const bool clear_pool = true ) {
    setReturnMode();
    setPrintMode();
    if ( clear_pool ) clearKernelSystem();
    return;
  }

  inline void load_kernel( const std::string &kfile ) {
    furnsh_c( kfile.c_str() );
  }

  inline void unload_kernel( const std::string &kfile ) {
    unload_c( kfile.c_str() );
  }

  inline std::string get_naif_error_msg( ) {
    const int NAIF_ERROR_STRING_SIZE = 2000;
    SpiceChar errmsg[NAIF_ERROR_STRING_SIZE];
    getmsg_c("LONG", NAIF_ERROR_STRING_SIZE, errmsg );
    return ( std::string( errmsg ) );
  }

  /**
   * @brief Check for NAIF errors with behavior control
   * 
   * This function will check for a NAIF error and take requested action.
   * 
   * @see get_naif_error_msg()
   * 
   * @param b_reset        If an error has occurred, reset the error system
   * @param throw_on_error Throw a runtime_error if an error occured
   * @return true          If no errror occurs
   * @return false         If an error occured
   */
  inline bool check_naif_errors( const bool b_reset = true,
                                 const bool throw_on_error = true ) {

    // Check for an error condition                                  
    if ( !failed_c() ) return ( false );

    // Reset the system
    std::string naif_error = get_naif_error_msg();
    if ( b_reset ) {
      reset_c();
    }

    if ( throw_on_error ) {
      throw std::runtime_error( "*** NAIF::Error - " + naif_error + " ***" );
    }

    return ( true );
  }

  /** Convert a date string to ephemeris time */
  inline double isoc_to_et( const std::string &datestr ) {
    SpiceDouble et;
    str2et_c( datestr.c_str() , &et );
    return ( et );
  }


  inline std::string et_to_isoc( const double et, const int prec = 3 ) {
    SpiceChar utc[80] ;
    et2utc_c( et, "ISOC" , prec, sizeof(utc), utc);
    return ( std::string (utc) );
  }
} // namespace naif

#endif
