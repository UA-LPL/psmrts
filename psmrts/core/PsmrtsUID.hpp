#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsUID_hpp
#define PsmrtsUID_hpp   
  
#include <atomic>
#include <cstdlib>
#include <errno.h>
#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>

namespace psmrts {
  
   /**
     * @brief Provide a system-wide unique identifer 
     * 
     * This class provides a PSRMTS-wide (and beyond) procedure
     * for acquiring a unique integer based identifier. All
     * PSMRTS products constructed should have one of these
     * for caching purposes.
     * 
     * Use of PsmrtsUID::UID_Reserved is for products
     * that are not intended to be cached. However, this is not
     * directly enforced. If they are cached, then
     * they will only ever occupy one space in the map 
     * and promptly be replaced.
     * 
     * Inherent use of std::atomic makes this class thread-safe.
     * 
     */
    class PsmrtsUID {
      public:
        using UIDType = unsigned long long;

        inline static const  UIDType &null_uid() {
          return ( UID_Reserved );
        }

        /** Return a unique ID which should never assumed to be negative */
        inline static UIDType get_uid() {
          return ( ++m_uid );  // This reserves ID <= UID_Reserved!
        }

        /** Checks for a valid ID */
        inline static bool is_valid_uid( const UIDType uid ) {
          return ( uid > null_uid() );
        }

        /** Convert a UID to a string */
        inline static std::string to_string( const UIDType uid ) {
          return ( std::to_string( uid ) );
        }

        /**
         * @brief Convert string to UID with robust error checking
         * 
         * This method will convert a string containing a UID to the binary form
         * of a PsmrtsUI. The string cannot contain any character other than
         * digits or an error occurs. If errors do occur an null_uid() is
         * returned. 
         * 
         * @param uid_s    String containing a PsmrtsUID
         * @return UIDType null_uid() if an error is detected, otherwise the
         *                   converted UID is returned.
         */
        inline static UIDType from_string( const std::string &uid_s = "null" ) {

          for ( size_t i = 0 ; i < uid_s.length() ; i++ ) {
            if ( !std::isdigit( uid_s[i] ) ) return ( null_uid() );
          }

          // Convert it 
          char *end = nullptr;
          return ( std::strtoull( uid_s.c_str(), &end, 10 ) );
        }

      private:
        PsmrtsUID()  = default;
        ~PsmrtsUID() = default;

        inline static const UIDType UID_Reserved{0};
        inline static std::atomic<UIDType> m_uid{UID_Reserved};
    };

  } // namespace psmrts

    #endif
