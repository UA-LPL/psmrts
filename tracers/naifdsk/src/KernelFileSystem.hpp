#ifndef KernelFileSystem_hpp
#define KernelFileSystem_hpp

#include <string>
#include <vector>

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

    inline bool isValid() const {
      return ( SPICETRUE == m_found );
    }

    inline SpiceInt handle() const {
      return ( m_handle );
    }

  } KernelDescriptor;


  /**
   * @brief API to NAIF's kernel file system
   * 
   */
  class KernelFileSystem {
    public:

      typedef std::vector<KernelDescriptor>     KernelFileList;


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

  };

} // namespace naif

#endif
