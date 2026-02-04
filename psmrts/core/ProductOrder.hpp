/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductOrder_hpp
#define ProductOrder_hpp

#include <string>
#include <functional>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductConfiguration.hpp>

namespace psmrts {


  /** 
   * @brief PSMRTS product order 
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-01-31 Kris J. Becker  Original Version
   */
  class ProductOrder : public PsmrtsRequest {
    public:
      ProductOrder( ) : PsmrtsRequest( "ProductOrder" ),
                        m_config( "ProductOrder" ),
                        m_residual( "ProductOrder" ) { }
      ProductOrder( const std::string &name  ) : 
                    PsmrtsRequest( name ),
                    m_config( name ),
                    m_residual( name ) { }
      ProductOrder( const ProductConfiguration &config ) : 
                    PsmrtsRequest( config.name() ),
                    m_config( config ),
                    m_residual( config.name() ) {}
      ProductOrder( const ProductConfiguration &config,
                    const ProductConfiguration &residual ) : 
                    PsmrtsRequest( config.name() ),
                    m_config( config ),
                    m_residual( residual ) {   }

      virtual ~ProductOrder() { }

      inline bool isvalid() const {
        return ( ( m_config.size() != 0 ) && ( m_residual.size() == 0 ) );
      }

      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      inline const ProductConfiguration &residual() const {
        return ( m_residual );
      }

      inline void add_option( const ProductOption &option ) {
        m_config.add( option );
      }

      inline void add_metadata( const ProductOption &option ) {
        m_config.add_metadata( option );
      }

      inline void add_residual( const ProductOption &option ) {
        m_residual.add( option );
      }      
      
      inline void set_residual( const ProductConfiguration &residual ) {
        m_residual = residual;
      }

    private:
      ProductConfiguration m_config;
      ProductConfiguration m_residual;
  };

      
} // namespace psmrts

#endif
