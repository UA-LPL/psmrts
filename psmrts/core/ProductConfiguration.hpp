#pragma once
#ifndef ProductConfiguration_hpp
#define ProductConfiguration_hpp

#include <string>
#include <exception>
#include <memory>
#include <vector>
#include <initializer_list>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>

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
      using ProductOptionList = std::vector<ProductOption>;

      ProductConfiguration( ) : m_name("none"), m_options{} { }
      ProductConfiguration(const std::string &name ) : m_name(), m_options{} { }
      explicit ProductConfiguration( const std::string &name, 
                                     const std::initializer_list<ProductOption> &options ) : 
                                     m_name( psmrts_tolower(name) ),
                                     m_options( options.begin(), options.end() ) { }                                
      explicit ProductConfiguration( const std::string &name, 
                                     const std::vector<ProductOption> &options ) : 
                                     m_name( psmrts_tolower(name) ),
                                     m_options( options ) { }      

      virtual ~ProductConfiguration() = default;


      inline const std::string &name() const {
        return ( m_name );
      }

      inline size_t size() const {
        return ( m_options.size() );
      }

      inline bool contains( const std::string &name ) const {
        std::string name_l = psmrts_tolower( name );
        for ( auto const &option : m_options ) {
          if ( option.name() == name_l ) {
            return ( true );
          }
        }
        return ( false );
      }

      inline void add( const ProductOption &option ) {
        m_options.push_back( option );
        return;
      }

      inline const ProductOption &find( const std::string &name ) {       
        std::string name_l = psmrts_tolower( name );
        for ( auto const &option : m_options ) {
          if ( option.name() == name_l ) {
            return ( option );
          }
        }  
        throw std::runtime_error( "*** ProductOption::get(" +  name +  " ) error - option not found" );
      }

      /** Remove the specifed option */
      inline bool remove( const std::string &name ) {   
        std::string name_l = psmrts_tolower( name );
        ProductOptionList::iterator opt_t = m_options.begin();

        while ( opt_t != m_options.end() ) {
          if ( opt_t->name() == name_l ) {
            m_options.erase( opt_t );
            return ( true );
          }
          ++opt_t;
        }  
        return ( false );
      }

      inline void clear() {
        m_options.clear();
      }

    protected:
      std::string       m_name;
      ProductOptionList m_options;
  };


} // namespace psmrts

#endif