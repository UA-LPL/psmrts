#ifndef KernelFileSystem_hpp
#define KernelFileSystem_hpp

#include <string>
#include <vector>
#include <map>
#include <mutex>

#include <PsmrtsUtilities.hpp>
#include <NaifUtilities.hpp>

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

    inline bool found() const {
      return ( SPICETRUE == m_found );
    }

    inline bool isValid() const {
      return ( found() );
    }

    inline SpiceInt handle() const {
      return ( m_handle );
    }

    inline const std::string &filename() const {
      return ( m_kernel_file );
    }

    inline const std::string &source() const {
      return ( m_source_file );
    }

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
      static inline KernelDescriptor kernel_info( const std::string &kernelfile ) {

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
      static inline int kernel_count( const std::string &ktype = "ALL" ) {
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
      static inline KernelFileList kernel_filetype_info( const std::string &kerneltypes = "ALL" ) {

        SpiceChar file[KernelDescriptor::K_MAX_STRING_LENGTH];
        SpiceChar filtyp[KernelDescriptor::K_MAX_STRING_LENGTH];
        SpiceChar srcfil[KernelDescriptor::K_MAX_STRING_LENGTH];
        SpiceInt  handle;
        SpiceBoolean found;
        
        KernelFileList k_list;
        int n_kernels = kernel_count( kerneltypes );
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
       * @brief Open a kernel that does not exist in the managed (DSK) pool
       * 
       * @param kfile 
       */
      static void open_kernel( const std::string &kfile ) {
        if ( !KernelFileSystem::has_kernel( kfile ) ) {
          load_kernel( kfile.c_str() );
        }
      }

      /**
       * @brief Close a kernel that is not in the managed (DSK) pool
       * 
       * @param kfile 
       */
      static void close_kernel( const std::string &kfile ) {
        // If its not in the inventory, unload it
        if ( !KernelFileSystem::has_kernel( kfile ) ) {
          unload_kernel( kfile.c_str() );
        }
      }

      //***** Shared Kernel Descriptor APi *****
      static size_t size( ) {
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
      static bool has_kernel( const std::string &kernelfile ) {
        // Lock up inventory access for thread safety ( >=c++17 )
        // std::scoped_lock mylocker( s_mutex );  

        auto kern = s_kernel_inventory.find( kernelfile );
        return ( kern != s_kernel_inventory.end() );
      }

      static SharedDskDescriptor get_shared_descriptor( const std::string &kernelfile ) {

        // Lock up inventory access for thread safety ( >=c++17 )
        std::scoped_lock mylocker( s_mutex );  

        // Check to see if it exists
        auto kern = s_kernel_inventory.find( kernelfile );

        if ( s_kernel_inventory.end() == kern ) {

          // Not in inventory, see if its open and put a wrapper about it
          KernelDescriptor kdescr = KernelFileSystem::kernel_info( kernelfile );
          if ( !kdescr.isValid() ) {
            // Load it a get a new descriptor for it
            KernelFileSystem::open_kernel( kernelfile );
            check_naif_errors();

            kdescr = KernelFileSystem::kernel_info( kernelfile );
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

      static bool safe_disposal_of( const std::string &kfile ) {

        // Lock up inventory access for thread safety ( >=c++17 )
        std::scoped_lock mylocker( s_mutex );           

        // Check to see if it exists and unload only if there are no references
        auto kern = s_kernel_inventory.find( kfile );
        if ( kern != s_kernel_inventory.end() ) {
          if ( kern->second.use_count() == 1 ) {
            s_kernel_inventory.erase( kern );
            KernelFileSystem::close_kernel( kfile );
            return ( true );
          }
        }
        else {
          auto file_info = naif::KernelFileSystem::kernel_info( kfile );
          if ( file_info.found() == true ) {
            KernelFileSystem::close_kernel( kfile );
            return ( true );
          }
        }
        // returns find status
        return ( false );
      }

      /** Reset the entire Kernel pool system, which closes all kernels and flushes pool */
      static void reset_kernel_system() {

        const bool ResetPoolSystem = true;

        // Lock up inventory access for thread safety ( >=c++17 )
        std::scoped_lock mylocker( s_mutex );

        s_kernel_inventory.clear();
        initKernelSystem( ResetPoolSystem );

        return;
      }

    private:
      inline static std::mutex      s_mutex = { };
      inline static KernelInventory s_kernel_inventory =  { };

  };

} // namespace naif

#endif
