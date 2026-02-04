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
      
    private:
      std::string m_name;
      std::string m_type;
      ProductConfiguration m_config;
      ProductSpecification m_specs;
  };

} // namespace psmrts

#endif
