#ifndef TemporaryDirectoryFixture_hpp
#define TemporaryDirectoryFixture_hpp

// #define  _POSIX_C_SOURCE 200809L
// #define  _XOPEN_SOURCE 500L

#include <exception>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>

#include <ftw.h>
#include <string>

class TemporaryDirectoryFixture {
  public:
    TemporaryDirectoryFixture( ) : m_dirname( nullptr),
                                   m_keep( false ) {
      m_dirname = make_tmp_directory( );
    }

    TemporaryDirectoryFixture( const std::string &dirbase ) :
                               m_dirname( nullptr ),
                               m_keep( false ) {
      m_dirname = make_tmp_directory( dirbase );
    }

    virtual ~TemporaryDirectoryFixture() {
      teardown();
    }


    /** Return the temporary directory name */
    std::string tmpdir() const {
      if ( !m_dirname ) return ( std::string() );
      return ( std::string( m_dirname ) );
    }

    /** Construct a full file path in the temporary directory */
    std::string tmppath( const std::string &filename ) const {
      return ( std::string ( tmpdir() + "/" + filename ) );
    }

    protected:

      /** Set keep state of directory for debugging purposes - use cautiously! */
      bool setKeeptmpdir( const bool keepem = false ) {
        bool old_keep = m_keep;
        m_keep = keepem;
        return ( old_keep );
      }


    private:
    // The template for mkdtemp() needs to persist!
      char   m_template_d[L_tmpnam];
      char  *m_dirname;
      bool   m_keep;

      /** Remove the directory as needed */
      void teardown() {
        if ( m_dirname != nullptr ) {

          // Only remove if not being expressly being saved
          if ( !m_keep ) {
            int status_d = nftw( m_dirname, remove_callback, FOPEN_MAX, ( FTW_DEPTH | FTW_MOUNT | FTW_PHYS ) );
            if ( status_d != 0 ) {
              throw std::runtime_error( "TemporaryDirectoryFixture::teardown::Error: " + std::string( std::strerror( errno ) ) );
            }
          }
        }

         m_template_d[0] = '\0';
         m_dirname = nullptr;
         return;
      }


      /**
       * @brief Create a temporary directory
       *
       * @param d_tmproot
       * @return char*
       */
      char *make_tmp_directory( const std::string d_tmproot  = "psmrts_tmpdir" ) {

        std::string template_s( "/tmp/" + d_tmproot + "_XXXXXX");
        std::strcpy( m_template_d, template_s.c_str() );
        char *tmpdir_d = mkdtemp( m_template_d );

        if ( !tmpdir_d ) {
          throw std::runtime_error( "TemporaryDirectoryFixture::make_tmp_directory::Error: " + std::string( std::strerror( errno ) ) );
        }

        return ( tmpdir_d );
      }

      /** Static callback function to actually remove the file */
      static int remove_callback( const char *fpath,
                                  __attribute__((unused)) const struct stat *sbuf,
                                  __attribute__((unused)) int type,
                                  __attribute__((unused)) struct FTW *ftwb) {

        if ( fpath == nullptr ) return ( 0 );
        return ( std::remove ( fpath ) );
      }
  };

#endif
