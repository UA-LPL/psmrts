#pragma once
#ifndef ProductSpecification_hpp
#define ProductSpecification_hpp

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <exception>

#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/ProductFeature.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>


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
      using ProductFeatureList = std::vector<ProductFeature>;

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

      inline ProductFeature specs( ) const {
        return ( ProductFeature( m_specs ) );
      }

      inline const ordered_json &json_specs() const {
        return ( m_specs );
      }

      inline size_t size() const {
        return ( m_parameters.size() );
      }

      inline const ProductFeatureList &parameters() const {
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

      inline const ProductFeature &get_parameter( const std::string &name ) const {
        for ( auto &parm_t : this->parameters() ) {
          if ( parm_t.name() == name ) return ( parm_t );
        }

        // Gotta toss an exception
        throw std::runtime_error( "*** ProductSpecification::get_parameter(" + name + ") - named parameter does not exist!" );
      }
     
      inline ProductFeature driver() const {
        if ( this->specs().contains( "driver" ) ) {
          return ( ProductFeature( this->specs().value<ordered_json>( "driver" ) ) );
        }
        return ( ProductFeature( "driver" ) );
      }
      
      inline std::vector<std::string> required() const {
        std::vector<std::string> result;
        ProductFeatureList param_specs = this->parameters();
        if (param_specs.size() > 0 ) {
          for ( const auto &param : param_specs ) {
            if ( param.is_required() ) {
                auto req = param.name();
                if (std::find(result.begin(), result.end(), req) == result.end()) {
                  result.push_back(req);
                }
            }
          }
        }
        return result;
      }
      
      inline std::vector<std::string> optional() const {
        std::vector<std::string> result;
        ProductFeatureList param_specs = this->parameters();
        if (param_specs.size() > 0 ) {
          for ( const auto &param : param_specs ) {
            if ( !param.is_required() ) {
                auto opt = param.name();
                if (std::find(result.begin(), result.end(), opt) == result.end()) {
                  result.push_back(opt);
                }
            }
          }
        }
        return result;
      }

      /** Checks if a user/dev request can be satisfies by this product spec 
      inline bool satisfies( const ProductConfiguration &config ) const {

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
           ProductFeature other_parm_t = this->get_parameter( parm_t.name() );
            if ( !parm_t.validate( other_parm_t ) ) {
              spec_errors_t.add_error( std::runtime_error( spec_errors_t.name() + " - parameter " + 
                                                            parm_t.name() + " does not exist in other" ) );
            }
          }
          catch ( const std::exception &se ) {
            spec_errors_t.add_error( se );
          }
        }

#if 0        
        // Check for parameter size equivalency? Not if its a config. FIX THIS!
        if ( this->size() != other.size() ) {
              spec_errors_t.add_error( std::runtime_error( spec_errors_t.name() + " - number parameters differ in " +
                                                            this->name() + "/" + other.name() + " product specs" ) );          
        }
#endif        
        // Now check for any errors and return a match if none
        if ( ( spec_errors_t.error_count() > 0 ) && ( throwException == true ) ) {
          spec_errors_t.throw_errors();
        }  
        
        // Return match status...WE REALLY SHOULD RETURN spec_errors_t.
        return ( spec_errors_t.error_count() == 0 );
      }


    private:
      std::string          m_name;
      std::string          m_product;
      std::string          m_type;
      ordered_json         m_specs;
      ProductFeatureList m_parameters;


      /** Parse/internalize the contents of the JSON struct */
      inline ProductFeatureList get_parameter_set() const {
        ProductFeatureList parms;

        if ( m_specs.contains( "features" ) ) {
          ordered_json parms_t = m_specs["features"];
          if ( ( parms_t.is_array() && ( parms_t.size() > 0 ) ) ) {
            for ( auto const & [key, j_parm_t ] : parms_t.items() ) {
              parms.push_back( ProductFeature( j_parm_t ) );
            }
          }
        }

        return ( parms );
      }
      
      /** Initialize the product specfication with expected values */
      inline void initialize( const std::string &name, 
                              const std::string &type,
                              const std::string &product,
                              const ordered_json &options = json_utils::json_null() ) {
        
        
        auto get_json_value = [] ( ordered_json &json_t,
                                   const std::string &key, 
                                   const std::string &value_t = "" ) -> std::string {
          
          std::string o_value = value_t;
          if ( o_value.size() == 0 ) {
            if ( !json_t.is_null() ) {
              // See if it exits and return it
              if ( json_t.contains( key ) ) {
                o_value = json_t[key];
              }
            }
          }
          else {
            if ( !json_t.is_null() ) {
              // See if it exists in the JSON object and return it
              if ( !json_t.contains( key ) ) {
                json_t[key] = o_value;
              }
            }
          }
          return ( o_value );
        };

        // Set up specs for the product          
        m_specs = ( options.is_null() ) ? ordered_json::object() : options;
        m_name = get_json_value( m_specs, "name", name );
        m_type = get_json_value( m_specs, "type", type );
        m_product = get_json_value( m_specs, "product", product );

        // Now ensure the required types are defined
        try {
          // m_name = m_specs["name"];
         //  m_type = m_specs["type"];
        }
        catch ( const json::exception &je ) {
          std::string mess = std::string("*** ProductSpecification name/type error - ").append( je.what() );
          throw std::runtime_error( mess );
        }

        // Create the parameters list
        m_parameters = this->get_parameter_set( );
      }

  };


} // namespace psmrts

#endif
