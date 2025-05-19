#pragma once
#ifndef ProductSpecification_hpp
#define ProductSpecification_hpp

#include <string>
#include <memory>
#include <deque>
#include <variant>
#include <exception>

#include <PsmrtsParameters.hpp>

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

      ProductSpecification( )  {
        initialize( );
      }

      ProductSpecification( const std::string &name, const std::string &type,
                             const ordered_json &options = json_utils::json_null() ) { 
        initialize( name , type, options );
      }

      ProductSpecification( const ordered_json &specs ) { 
        initialize( "", "", specs );
      }

      virtual ~ProductSpecification() { }


      inline const std::string &name() const {
        return ( m_name );
      }

      inline const std::string &type() const {
        return ( m_type);
      }      

      inline const PsmrtsParameters &specs( ) const {
        return ( m_parameters );
      }

      inline std::vector<std::string> required( ) const {
        return ( m_parameters.get_parameter<std::vector<std::string>>( "required" ) );
      }

      inline std::vector<std::string> optional( ) const {
        return ( m_parameters.get_parameter<std::vector<std::string>>( "optional" ) );
      }      

      inline bool matches( const ProductSpecification &other, const bool throwException = false ) const {

        bool it_matches = false;
        try {
          if ( this->name() != other.name() ) return ( false ); 
          if ( this->type() != other.type() ) return ( false );

          // Now check required
          std::string all_errors;
          std::string newline("");

          std::vector<std::string> keys = this->required();
          for ( const std::string &key : keys ) {
            if ( other.specs().parameters().contains( key ) ) {
              ordered_json diff_j = json::diff( other.specs().parameters()[key], this->specs().parameters()[key] );
              if ( diff_j.size() != 0 ) {
                all_errors.push_back( newline + "Required product " + key + " does not match product spec value" );                
                newline = "\n";
              }
            }
            else {
              all_errors.push_back( newline + "Required product " + key + " does not exist in other spec" );
              newline = "\n";
            }
          }

          keys = this->optional();
          for ( const std::string &key : keys ) {
            if ( other.specs().parameters().contains( key ) ) {
              ordered_json diff_j = json::diff( other.specs().parameters()[key], this->specs().parameters()[key] );
              if ( diff_j.size() != 0 ) {
                all_errors.push_back( newline + "Optional key " + key + " does not match product spec value" );                
                newline = "\n";
              }
            }
          }
          
          // Now check for any errors and return a match if none
          if ( all_errors.length() == 0 ) it_matches = true;
        }
        catch ( const json::exception &je ) {
          it_matches =  false;
        }
        catch ( const std::runtime_error &re ) {
          it_matches = false;
        }  
        
        // Return match status...
        return ( it_matches );
      }

    private:
      std::string      m_name;
      std::string      m_type;
      PsmrtsParameters m_parameters;

      inline void initialize( const std::string &name = "", const std::string &type = "",
                              const ordered_json &options = json_utils::json_null() ) {
        
        // Set up specs for the product
        ordered_json p_specs = options;
        if ( name.length() > 0 ) {
          p_specs["name"] = name;
          m_name = name;
        }

        if ( type.length() > 0 ) {
          p_specs["type"] = type;
          m_type = type;
        }

        // Now ensure the required types are defined
        try {
          m_name = p_specs["name"];
          m_type = p_specs["type"];
        }
        catch ( const json::exception &je ) {
          std::string mess = std::string("*** ProductSpecification name/type error - ").append( je.what() );
          throw std::runtime_error( mess );
        }

        m_parameters = PsmrtsParameters( p_specs );
      }



  };


} // namespace psmrts

#endif