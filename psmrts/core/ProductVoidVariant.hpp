/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductVoidVariant_hpp
#define ProductVoidVariant_hpp

#include <vector>
#include <deque>
#include <string>
#include <exception>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>

namespace psmrts { 

  /**
   * @brief A specialized void type variant placeholder for monostate
   * 
   * 
   * @see PsmrtsTracer.hpp for an example of this technique.
   */
  class ProductVoidVariant : public MissingProcessRequestHandler {
    public:
      ProductVoidVariant() : MissingProcessRequestHandler( "void" ),
                             m_config( "void" ),
                             m_specs( "void", "variant" ) { }
      ProductVoidVariant( const std::string &name ):
                          MissingProcessRequestHandler( name ),
                          m_config( name ),
                          m_specs( name, "variant" ) { }
      ProductVoidVariant( const ProductConfiguration &config ) :
                          MissingProcessRequestHandler( config.name() ),
                          m_config( config ),
                          m_specs( config.name(), "variant" ) { }
      virtual ~ProductVoidVariant() = default;
      
      inline ProductSpecification product_specifications() const {
        return ( m_specs );
      }

      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      inline bool matches( const ProductConfiguration &conf ) const {
        return ( false );
      }

      inline void create( const ProductConfiguration &config,
                          const PsmrtsTranslations &trans ) {
        throw std::runtime_error( "PsmrtsVoidVariant is not a valid product!");
      }

      
    private:
      std::string m_name;
      std::string m_type;
      ProductConfiguration m_config;
      ProductSpecification m_specs;
  };

} // namespace psmrts

#endif
