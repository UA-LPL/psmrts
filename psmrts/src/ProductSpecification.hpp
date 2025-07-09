#pragma once
#ifndef ProductSpecification_hpp
#define ProductSpecification_hpp

#include <string>
#include <memory>
#include <deque>
#include <vector>
#include <variant>
#include <exception>

#include <PsmrtsParameters.hpp>
#include <ProductParameter.hpp>
#include <ProductRequest.hpp>
#include <PsmrtsRequest.hpp>


namespace psmrts { 


  /**
   * @brief Process/maintain product specfication data
   * 
   * This class maintains the specifications for PMSRTS product.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-05-19 Kris J. Becker - Original Version
   */
  class ProductSpecification {
    public:
      using ProductParameterList = std::deque<ProductParameter>;

      ProductSpecification( )  {
        initialize( "null", "", "" );
      }

      ProductSpecification( const std::string &name, const std::string &type,
                             const ordered_json &options = json_utils::json_null() ) { 
        initialize( name , type, "", options );
      }

      ProductSpecification( const std::string &name,
                            const std::string &type,
                            const std::string &product,
                            const ordered_json &options = json_utils::json_null() ) { 
        initialize( name , type, product, options );
      }


      ProductSpecification( const ordered_json &specs ) { 
        initialize( "", "", "",specs );
      }

      virtual ~ProductSpecification() = default;


      inline const std::string &name() const {
        return ( m_name );
      }

      inline const std::string &product() const {
        return ( m_product );
      }

      inline const std::string &type() const {
        return ( m_type);
      }      

      inline ProductParameter specs( ) const {
        return ( ProductParameter( m_specs ) );
      }

      inline const ordered_json &json_specs() const {
        return ( m_specs );
      }

      inline size_t size() const {
        return ( m_parameters.size() );
      }

      inline const ProductParameterList &parameters() const {
        return ( m_parameters );
      }

      inline std::vector<std::string> get_parameter_names() const {
        std::vector<std::string> pnames_t;
        for ( auto const &parm_t : this->parameters() ) {
          pnames_t.push_back( parm_t.name() );
        }
        return ( pnames_t );
      }

      inline bool has_parameter( const std::string &name ) const {
        for ( auto const &parm_t : this->parameters() ) {
          if ( parm_t.name() == name ) return ( true );
        }
        return ( false );
      }

      inline const ProductParameter &get_parameter( const std::string &name ) const {
        for ( auto &parm_t : this->parameters() ) {
          if ( parm_t.name() == name ) return ( parm_t );
        }

        // Gotta toss an exception
        throw std::runtime_error( "*** ProductSpecification::get_parameter(" + name + ") - named parameter does not exist!" );
      }
     
      inline ProductParameter driver() const {
        if ( this->has_parameter( "driver" ) ) {
          return ( this->get_parameter( "driver" ) );
        }
        return ( ProductParameter( "nodriver" ) );
      }
      
      /** Checks if a user/dev request can be satisfies by this product spec 
      inline bool satisfies( const ProductRequest &request ) const {

      }
      */

      /** Compares two specfications for a match */
      inline bool matches( const ProductSpecification &other, const bool throwException = false ) const {

        bool it_matches = false;
        if ( this->name() != other.name() )       return ( false ); 
        if ( this->type() != other.type() )       return ( false );
        if ( this->product() != other.product() ) return ( false );
        if ( this->size() != other.size() )       return ( false );

        // Now check required
        PsmrtsRequest spec_errors_t( "PsmrtsSpecification::matches()");

        for ( auto const &parm_t : this->parameters() ) {
          try {
           ProductParameter other_parm_t = this->get_parameter( parm_t.name() );
            if ( !parm_t.validate( other_parm_t ) ) {
              spec_errors_t.add_error( std::runtime_error( spec_errors_t.name() + " - parameter " + 
                                                            parm_t.name() + " does not exist in other" ) );
            }
          }
          catch ( const std::runtime_error &rte ) {
            spec_errors_t.add_error( rte );
          }
        }
        
        // Now check for any errors and return a match if none
        if ( ( spec_errors_t.error_count() > 0 ) && ( throwException == true ) ) {
          spec_errors_t.throw_errors();
        }  
        
        // Return match status...
        return ( spec_errors_t.error_count() == 0 );
      }


    private:
      std::string          m_name;
      std::string          m_product;
      std::string          m_type;
      ordered_json         m_specs;
      ProductParameterList m_parameters;


      /** Parse/internalize the contents of the JSON struct */
      inline ProductParameterList get_parameter_set() const {
        ProductParameterList parms;

        if ( m_specs.contains( "parameters" ) ) {
          ordered_json parms_t = m_specs["parameters"];
          if ( ( parms_t.is_array() && ( parms_t.size() > 0 ) ) ) {
            for ( auto const & [key, j_parm_t ] : parms_t.items() ) {
              parms.push_back( ProductParameter( j_parm_t ) );
            }
          }
        }

        return ( parms );
      }
      
      /** Initialize the product specfication with expected values */
      inline void initialize( const std::string &name, const std::string &type,
                              const std::string &product,
                              const ordered_json &options = json_utils::json_null() ) {
        
        // Set up specs for the product
                          
        m_specs = ordered_json();
        if ( name.length() > 0 ) {
          m_specs["name"] = name;
          m_name = name;
        }
        else {
          m_name = "";
        }

        if ( type.length() > 0 ) {
          m_specs["type"] = type;
          m_type = type;
        }
        else {
          m_type = "";
        }

        if ( product.length() > 0 ) {
          m_specs["product"] = product;
          m_product = product;
        }
        else {
          m_product = "";
        }


        // Now ensure the required types are defined
        try {
          m_name = m_specs["name"];
         //  m_type = m_specs["type"];
        }
        catch ( const json::exception &je ) {
          std::string mess = std::string("*** ProductSpecification name/type error - ").append( je.what() );
          throw std::runtime_error( mess );
        }

        // Now update with the options. Note that any existing keywords will
        // be preserved in the merge.
        const bool MergeObjectsTrue = true;
        m_specs.update( options, MergeObjectsTrue );

        // Create the parameters list
        m_parameters = this->get_parameter_set( );
      }

  };


} // namespace psmrts

#endif
