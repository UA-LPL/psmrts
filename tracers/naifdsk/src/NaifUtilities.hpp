#ifndef NaifUtilities_hpp
#define NaifUtilities_hpp

#include <cspice/SpiceUsr.h>

namespace naif {

// #include <cspice/SpiceUsr.h>

  inline void initKernelSystem() {
    kclear_c();
    return;
  }

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
  inline bool check_for_errors( const bool b_reset = true,
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

  typedef struct kernel_descriptor {
    kernel_descriptor() : 
                         m_kernel_file(),
                         m_kernel_type(), 
                         m_handle(-1),
                         m_source(),
                         m_found( 0 ) { }

    std::string  m_kernel_file;
    std::string  m_kernel_type;
    SpiceInt     m_handle;
    std::string  m_source;
    SpiceBoolean m_found;

    inline bool isgood() const {
      return ( 0 != m_found );
    }

  } KernelDescriptor;


  typedef struct dsk_segment {

  } DskSegment;



} // namespace naif

#endif
