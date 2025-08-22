#pragma once
#ifndef ProductRequest_hpp
#define ProductRequest_hpp

#include <memory>
#include <string>
#include <vector>
#include <exception>

#include <psmrts/core/ProductParameter.hpp>

namespace psmrts { 


  /**
   * @brief Process/maintain user/dev product requests 
   * 
   * This class maintains user and developer configuration requests that are
   * submitted to PSMRTS.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-05-29 Kris J. Becker - Original Version
   */
  class ProductRequest {
    public:
      using ProductParameterList = std::vector<ProductParameter>;

      ProductRequest( ) : m_request_j(), m_parameters{} { }

      /** 
      ProductRequest( const ordered_json &request ) {
        m_request_j = request;
        (void) parse_parameters( request, m_parameters );
      }
      */

      ProductRequest( const std::vector<ProductParameter> &parameters ) :
                      m_request_j(), m_parameters{} { 
        // Unbind them all to the JSON struct...

      }

      virtual ~ProductRequest() = default;


      inline size_t size() const {
        return ( m_parameters.size() );
      }

      /**
      inline void add_parameter( const ProductParameter &parm ) { 

      }

      inline ProductParameter &get_parameter( const std::string &parm ) {       
      
      }

      inline int parse_parameters( const ordered_json, ProductParameterList &plist ) {

      }
      */
    protected:
      ordered_json         m_request_j;
      ProductParameterList m_parameters;

  };


} // namespace psmrts

#endif