#pragma once
#ifndef ProductConfiguration_hpp
#define ProductConfiguration_hpp

#include <memory>
#include <string>
#include <vector>
#include <exception>

#include <psmrts/core/ProductFeature.hpp>

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
  class ProductConfiguration {
    public:
      using ProductFeatureList = std::vector<ProductFeature>;

      ProductConfiguration( ) : m_request_j(), m_parameters{} { }

      /** 
      ProductConfiguration( const ordered_json &request ) {
        m_request_j = request;
        (void) parse_parameters( request, m_parameters );
      }
      */

      ProductConfiguration( const std::vector<ProductFeature> &parameters ) :
                      m_request_j(), m_parameters{} { 
        // Unbind them all to the JSON struct...

      }

      virtual ~ProductConfiguration() = default;


      inline size_t size() const {
        return ( m_parameters.size() );
      }

      /**
      inline void add_parameter( const ProductFeature &parm ) { 

      }

      inline ProductFeature &get_parameter( const std::string &parm ) {       
      
      }

      inline int parse_parameters( const ordered_json, ProductFeatureList &plist ) {

      }
      */
    protected:
      ordered_json         m_request_j;
      ProductFeatureList   m_parameters;

  };


} // namespace psmrts

#endif