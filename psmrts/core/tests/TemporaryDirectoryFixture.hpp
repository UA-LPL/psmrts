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
#include <mutex>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

/**
 * @brief Provide a temporary directory system for Catch2 testing
 * 
 * This class creates and maintains a directory in the system temp directory for
 * isolated file creation and usage. The destructor will delete the directory
 * from temp directory.
 * 
 * The default constructor will simply return the system temp directory. The
 * user is assumed to manage its own files in that directory. 
 * 
 * @author Kris J. Becker, University of Arizona
 * @history 2026-03-22 Kris J. Becker  Original Version
 */
class TemporaryDirectoryFixture {
  public:
    TemporaryDirectoryFixture( ) : m_dirname( ),
                                   m_keep( false ) {
      m_dirname = fs::temp_directory_path();
      m_keep = true;
    }

    /**
     * @brief Construct a new temporary unique directory in systen temp
     * 
     * Create a unique temporary directory in the system temporary directory.
     * This directory will be removed by the constructor and any files
     * contained in the directory. 
     * 
     * A local static counter is appended to the dirbase as a string and
     * incremented for the next unique directory name.
     * 
     * @param dirbase Base name of the directory to create
     */
    TemporaryDirectoryFixture( const std::string &dirbase ) :
                               m_dirname( ),
                               m_keep( false ) {
      m_dirname = make_tmp_directory( dirbase );
    }

    /** Destructor will remove the temporary directory created by constructor */
    virtual ~TemporaryDirectoryFixture() {
      teardown();
    }

    /** Return the system temporary directory */
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
      static inline unsigned long m_dir_id = 0;  // Unique directory id
      static inline const size_t MaxDirectories = 20;  // Maximum number tries
      static inline std::mutex  m_mutex{};  // Lock directory creation 
      fs::path m_dirname;
      bool     m_keep;

      /** Remove the directory as needed */
      inline void teardown() {
        // Only remove if it exists
        if ( fs::exists( m_dirname ) ) {

          // Only remove if not being expressly being saved. We also do not want
          // to remove the system temp directory.
          if ( !m_keep  && ( m_dirname != fs::temp_directory_path() ) ) {
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
      inline fs::path make_tmp_directory( const std::string &d_tmproot  = "psmrts_tmpdir" ) {
        fs::path sys_dir_t = fs::temp_directory_path();
        std::scoped_lock mylocker( m_mutex );

        for ( size_t ndirs = 0 ; ndirs < MaxDirectories ; ndirs++ ) {
          fs::path tmpdir_t = sys_dir_t / ( d_tmproot + "_" + std::to_string( m_dir_id++ ) );
          if ( true == fs::create_directory( tmpdir_t ) ) {
            m_keep = false;
            return ( tmpdir_t );
          }
        }

        // Could not create a unique directory, return system tmp.
        m_keep = true;
        return ( fs::temp_directory_path() );
      }

  };

#endif
