/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef KernelFileSystem_hpp
#define KernelFileSystem_hpp

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <shared_mutex>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <cspice/SpiceUsr.h>

namespace naif {

  /** Structure to define the NAIF kernel file */
  typedef struct kernel_descriptor {
    static constexpr size_t K_MAX_STRING_LENGTH = 1024;

    kernel_descriptor() : 
                         m_kernel_file(),
                         m_kernel_type(), 
                         m_source_file(),
                         m_handle( 0 ),
                         m_found( 0 ) { }

    kernel_descriptor( const std::string &kernelfile ) : 
                       m_kernel_file( kernelfile ),
                       m_kernel_type(), 
                       m_source_file(),
                       m_handle( 0 ),
                       m_found( 0 ) { }

    std::string  m_kernel_file;
    std::string  m_kernel_type;
    std::string  m_source_file;
    SpiceInt     m_handle;
    SpiceBoolean m_found;

    /** Returns true if specified file could be located */
    inline bool found() const {
      return ( SPICETRUE == m_found );
    }

    /** Returns validity state of kernel - true if located/accessible */
    inline bool isValid() const {
      return ( found() );
    }

    /** Returns handle attached to file */
    inline SpiceInt handle() const {
      return ( m_handle );
    }

    /** Returns file name */
    inline const std::string &filename() const {
      return ( m_kernel_file );
    }

    /** Returns name of source file */
    inline const std::string &source() const {
      return ( m_source_file );
    }

    /** Returns type of kernel */
    inline const std::string &type() const {
      return ( m_kernel_type );
    }

  } KernelDescriptor;


  /**
   * @brief API to NAIF's kernel file system
   * 
   */
  class KernelFileSystem {
    public:
      typedef psmrts::DatumMutexWrapper<KernelDescriptor>  SharedDskDescriptor;
      typedef std::map<std::string, SharedDskDescriptor>   KernelInventory;
      typedef std::vector<KernelDescriptor>                KernelFileList;

      KernelFileSystem() { }
      virtual ~KernelFileSystem() { }

      /**
       * @brief Retrives information for a NAIF kernel loaded with furnsh_c()
       * 
       * See https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/kinfo_c.html
       * 
       * @param kernelfile         Name of the kernel to find
       * @return KernelDescriptor  Structure containing results from NAIF file system
       */
      inline static KernelDescriptor kernel_info( const std::string &kernelfile ) {
        std::unique_lock<std::shared_mutex> mylocker( s_mutex );
        return ( KernelFileSystem::get_kernel_info( kernelfile) );
      }

      /**
       * @brief Return count of files loaded in the system
       * 
       * This will query the NAIF file system and return the count of the
       * number of kernels currently loaded via furnsh_c() in NAIF system.
       * 
       * You can can check a list of various types of kernels by providing
       * as string of kernel types separated by spaces. For example 
       * ktype="CK SPK" will return the count of both SPK and CK type
       * kernels are currently open/loaded. Default is "ALL"
       * 
       * See https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/ktotal_c.html.
       * 
       * @param ktype Type of kernels to retrieve counts for. This can be
       *              list of types separated by spaces or a single string
       *              of the type to check. Default: "ALL".
       * @return int Number of kernels
       */
      inline static int kernel_count( const std::string &ktype = "ALL" ) {
        std::unique_lock<std::shared_mutex> mylocker( s_mutex );
        SpiceInt n_kernels;
        (void) ktotal_c( ktype.c_str(), &n_kernels );
        return ( n_kernels );
      }

      /**
       * @brief Retrieve a typed list of NAIF kernels currently loaded in NAIF
       * 
       * See https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/kdata_c.html.
       * 
       * @param kerneltypes Type of kernels to retrieve counts for. This can be
       *              list of types separated by spaces or a single string
       *              of the type to check. Default: "ALL".
       * 
       * @see kernel_count()
       * @see kernel_info()
       */
      inline static KernelFileList kernel_filetype_info( const std::string &kerneltypes = "ALL" ) {

        SpiceChar file[KernelDescriptor::K_MAX_STRING_LENGTH];
        SpiceChar filtyp[KernelDescriptor::K_MAX_STRING_LENGTH];
        SpiceChar srcfil[KernelDescriptor::K_MAX_STRING_LENGTH];
        SpiceInt  handle;
        SpiceBoolean found;
        
        KernelFileList k_list;
        int n_kernels = kernel_count( kerneltypes );
        std::unique_lock<std::shared_mutex> mylocker( s_mutex );

        for ( int which = 0; which < n_kernels ; which++ ) {
          (void) kdata_c( which, kerneltypes.c_str(), 
                          sizeof( file ), sizeof( filtyp ), sizeof( srcfil),
                          file, filtyp, srcfil, 
                          &handle, &found );
                      
          KernelDescriptor k_descriptor( { std::string( file ) } );
          k_descriptor.m_source_file = std::string( srcfil );
          k_descriptor.m_kernel_type = std::string( filtyp );
          k_descriptor.m_handle      = handle;
          k_descriptor.m_found       = found;

          k_list.push_back( k_descriptor );
        }

        return ( k_list );
      }

      /**
       * @brief Fundamental initialization of NAIF toolkit library
       * 
       * This function is needed to prevent NAIF's error system from crashing
       * with a fatal abort. This clears the cache and sets appropriate
       * NAIF internals to avoid default behavior which is to abort the
       * application. The PSMRTS NAIF implemenation requires use of
       * the check_naif_error() method to check and report occurances of
       * errors.
       * 
       * This method is rentrant in that it will only initialize once.
       * Users can also call the initKernelSystem() directly at any point
       * to initialize as needed.
       * 
       * Below is the required flag to initialize at startup that must be in
       * global scope.
       */
      static inline std::once_flag naif_initialized;      
      inline static void initialize_naif_system( ) {
        std::call_once( naif_initialized, [&]( ) { 
             KernelFileSystem::initKernelSystem( );
           } 
        );

        return;
      }

      /**
       * @brief Open a kernel that does not exist in the managed (DSK) pool
       * 
       * @param kfile 
       */
      inline static void open_kernel( const std::string &kfile ) {
        std::unique_lock<std::shared_mutex> mylocker( s_mutex );

        auto kern = s_kernel_inventory.find( kfile );
        if ( kern == s_kernel_inventory.end() ) {
          KernelFileSystem::load_kernel( kfile.c_str() );
          KernelFileSystem::check_naif_status();
        }
      }

      /**
       * @brief Close a kernel that is not in the managed (DSK) pool
       * 
       * @param kfile 
       */
      inline static void close_kernel( const std::string &kfile ) {
        std::unique_lock<std::shared_mutex> mylocker( s_mutex );

        // If its not in the inventory, unload it
        auto kern = s_kernel_inventory.find( kfile );
        if ( kern == s_kernel_inventory.end() ) {
          KernelFileSystem::unload_kernel( kfile.c_str() );
          KernelFileSystem::check_naif_status();
        }
      }

      //***** Shared Kernel Descriptor APi *****
      inline static size_t size( ) {
        std::shared_lock<std::shared_mutex> mylocker( s_mutex );
        return ( s_kernel_inventory.size() );
      }

      /**
       * @brief Open or share an existing NAIF descriptor for a kernel with handles
       * 
       * This method may need thread locking since it searches the inventory that is
       * thread volatile. Currently, locking here will deadlock the application.
       * 
       * @param kernelfile 
       * @return true 
       * @return false 
       */
      inline static bool has_kernel( const std::string &kernelfile ) {
        // Lock up inventory access for thread safety ( >=c++17 )
        std::shared_lock<std::shared_mutex> mylocker( s_mutex );  

        auto kern = s_kernel_inventory.find( kernelfile );
        return ( kern != s_kernel_inventory.end() );
      }

      inline static SharedDskDescriptor get_shared_descriptor( const std::string &kernelfile ) {

        // Lock up inventory access for thread safety ( >=c++17 )
        std::unique_lock<std::shared_mutex> mylocker( s_mutex );  

        // Check to see if it exists
        auto kern = s_kernel_inventory.find( kernelfile );

        if ( s_kernel_inventory.end() == kern ) {

          // Not in inventory, see if its open and put a wrapper about it
          KernelDescriptor kdescr = KernelFileSystem::get_kernel_info( kernelfile );
          if ( !kdescr.isValid() ) {
            // Load it a get a new descriptor for it
            auto kern = s_kernel_inventory.find( kernelfile );
            if ( kern == s_kernel_inventory.end() ) {
              KernelFileSystem::load_kernel( kernelfile );
            }
            KernelFileSystem::check_naif_status();
            kdescr = KernelFileSystem::get_kernel_info( kernelfile );
          }

          // If its not valid here the file cannot be found
          if ( !kdescr.isValid() ) {
            std::string mess = "*** ERROR: KernelFileSystem::get_shared_descriptor() - Kernel file " + kernelfile + " does not exist of is invalid";
            throw std::runtime_error( mess );
          }

          // Insert the kernel descriptor in the inventory
          auto kernresult = s_kernel_inventory.insert_or_assign( kernelfile, SharedDskDescriptor( kdescr ) );
          kern = kernresult.first;
        }


        return ( kern->second );
      }

      inline static bool safe_disposal_of( const std::string &kfile ) {

        // Lock up inventory access for thread safety ( >=c++17 )
        std::unique_lock<std::shared_mutex> mylocker( s_mutex );           

        // Check to see if it exists and unload only if there are no references
        auto kern = s_kernel_inventory.find( kfile );
        if ( kern != s_kernel_inventory.end() ) {
          if ( kern->second.use_count() == 1 ) {
            s_kernel_inventory.erase( kern );
            KernelFileSystem::unload_kernel( kfile );
            return ( true );
          }
        }
        else {
          auto file_info = naif::KernelFileSystem::get_kernel_info( kfile );
          if ( file_info.found() == true ) {
            KernelFileSystem::unload_kernel( kfile );
            return ( true );
          }
        }
        // returns find status
        return ( false );
      }

      /** Reset the entire Kernel pool system, which closes all kernels and flushes pool */
      inline static void reset_kernel_system( const bool ResetPoolSystem = false ) {

        // Lock up inventory access for thread safety ( >=c++17 )
        std::unique_lock<std::shared_mutex> mylocker( s_mutex );

        s_kernel_inventory.clear();
        KernelFileSystem::initKernelSystem( ResetPoolSystem );

        return;
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
      inline static bool check_naif_errors( const bool b_reset = true,
                                            const bool throw_on_error = true ) {

        std::unique_lock<std::shared_mutex> mylocker( s_mutex );
        return ( KernelFileSystem::check_naif_status( b_reset, throw_on_error ) );
      }
      
      
    private:
      inline static std::shared_mutex s_mutex{ };
      inline static KernelInventory   s_kernel_inventory =  { };

      inline static void setReturnMode( const std::string &u_retmode = "RETURN" ) {

        int retmode_len = u_retmode.size();
        constexpr int MAXLEN = 1024;
        SpiceChar retmode[MAXLEN];

        int maxchars = std::min( retmode_len+1, MAXLEN-1 );
        std::strncpy( retmode, u_retmode.c_str(), maxchars );

        erract_c( "SET", MAXLEN, retmode );
        return;
      }

      inline static void setPrintMode( const std::string &u_prtmode = "NONE" ) {

        int prtmode_len = u_prtmode.size();

        constexpr int MAXLEN = 1024;
        SpiceChar prtmode[MAXLEN];

        int maxchars = std::min( prtmode_len+1, MAXLEN-1 );
        std::strncpy( prtmode, u_prtmode.c_str(), maxchars );
      
        errprt_c( "SET", MAXLEN, prtmode );
        return;
      }

      inline static std::string get_naif_error_msg( ) {
        const int NAIF_ERROR_STRING_SIZE = 2000;
        SpiceChar errmsg[NAIF_ERROR_STRING_SIZE];
        getmsg_c("LONG", NAIF_ERROR_STRING_SIZE, errmsg );
        return ( std::string( errmsg ) );
      }

      inline static bool check_naif_status( const bool b_reset = true,
                                            const bool throw_on_error = true ) {
        // Check for an error condition                                  
        if ( !failed_c() ) return ( false );

        // Reset the system
        std::string naif_error = KernelFileSystem::get_naif_error_msg();
        if ( b_reset ) {
          reset_c();
        }

        if ( throw_on_error ) {
          throw std::runtime_error( "*** NAIF::Error - " + naif_error + " ***" );
        }

        return ( true );        
      }

      inline static void clearKernelSystem() {
        kclear_c();
        return;
      }

      inline static void initKernelSystem( const bool clear_pool = false ) {
        setReturnMode();
        setPrintMode();
        if ( clear_pool ) clearKernelSystem();
        return;
      }

      inline static void load_kernel( const std::string &kfile ) {
        initialize_naif_system();
        furnsh_c( kfile.c_str() );
      }

      inline static void unload_kernel( const std::string &kfile ) {
        unload_c( kfile.c_str() );
      }    
        
      inline static KernelDescriptor get_kernel_info( const std::string &kernelfile ) {

        SpiceChar filtyp[KernelDescriptor::K_MAX_STRING_LENGTH];
        SpiceChar srcfil[KernelDescriptor::K_MAX_STRING_LENGTH];
        SpiceInt  handle;
        SpiceBoolean found;

        (void) kinfo_c( kernelfile.c_str(), sizeof(filtyp), sizeof(srcfil), filtyp, srcfil, &handle, &found );

        KernelDescriptor k_descriptor( kernelfile );
        k_descriptor.m_source_file = std::string( srcfil );
        k_descriptor.m_kernel_type = std::string( filtyp );
        k_descriptor.m_handle      = handle;
        k_descriptor.m_found       = found;

        return ( k_descriptor );
      }

  
  };

} // namespace naif

#endif
