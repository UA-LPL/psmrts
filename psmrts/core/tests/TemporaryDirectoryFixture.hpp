/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef TemporaryDirectoryFixture_hpp
#define TemporaryDirectoryFixture_hpp

#include <exception>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

/**
 * @brief Provide a temporary directory system for Catch2 testing
 * 
 * This class creates and maintains a directory in the system temp directory for
 * isolated file creation and usage.
 * 
 */
class TemporaryDirectoryFixture {
  public:
    TemporaryDirectoryFixture( ) : m_dirname( ),
                                   m_keep( false ) {
      m_dirname = TemporaryDirectoryFixture::sys_tmp_dir();
      m_keep = true;
    }

    TemporaryDirectoryFixture( const std::string &dirbase ) :
                               m_dirname( ),
                               m_keep( false ) {
      m_dirname = make_tmp_directory( dirbase );
    }

    virtual ~TemporaryDirectoryFixture() {
      teardown();
    }

    static inline std::string sys_tmp_dir() {
      return ( fs::temp_directory_path().u8string() );
    }

    /** Return the temporary directory name */
    inline std::string tmpdir() const {
      return (  m_dirname.u8string() );
    }

    /** Construct a full file path in the temporary directory */
    inline std::string tmppath( const std::string &filename ) const {
      return ( ( m_dirname / fs::path{filename} ).u8string() );
    }

    protected:

      /** Set keep state of directory for debugging purposes - use cautiously! */
      inline bool setKeeptmpdir( const bool keepem = false ) {
        bool old_keep = m_keep;
        m_keep = keepem;
        return ( old_keep );
      }


    private:
      static inline unsigned long m_dir_id = 0;  // Unique dir id
      fs::path m_dirname;
      bool     m_keep;

      /** Remove the directory as needed */
      inline void teardown() {
        // Only remove if it exists
        if ( fs::exists( m_dirname ) ) {

          // Only remove if not being expressly being saved
          if ( !m_keep  && ( m_dirname != fs::temp_directory_path() ) ) {
            // std::cout << "Removing dir: " << m_dirname  << std::endl;
            (void) fs::remove_all( m_dirname );
          }
        }
         return;
      }

      /**
       * @brief Create a temporary directory for testing purposes
       *
       * @param d_tmproot Base directory name of temporary directory
       * @return char*    Returns full path to created temporary directory
       */
      inline fs::path make_tmp_directory( const std::string &d_tmproot  = "psmrts_tmpdir" ) const {
        fs::path sys_dir_t = fs::temp_directory_path();
        fs::path tmpdir_t = sys_dir_t / ( d_tmproot + "_" + std::to_string( m_dir_id++ ) );
        if ( !fs::create_directory( tmpdir_t ) ) {
          throw std::runtime_error( "TemporaryDirectoryFixture::make_tmp_directory::Error directory not created: " + tmpdir_t.u8string() );
        }

        return ( tmpdir_t );
      }

  };

#endif
