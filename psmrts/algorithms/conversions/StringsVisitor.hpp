/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef StringsVisitor_hpp
#define StringsVisitor_hpp

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>

namespace psmrts::algorithms::conversions {


/**
   * @brief Option string conversion extracts strings from ProductOptions
   * 
   * This functor object will extract, converting if necessary, any of the
   * stored intrinsic types. This must be maintained alongside any changes
   * made to ProductOption, paticular any new types added or removed.
   * 
   * @author 2025-08-21 Kris J Becker
   */
  class StringsVisitor { 
    public:
      using Type        = std::string;
      using TypeVector  = std::vector<Type>;
      static inline const Type TypeDefault = "";  
       
      StringsVisitor( TypeVector &data,
                      const Type &default_v,
                      const ConversionParameters parms = ConversionParameters() ) : 
                      m_datum( data ),m_default( default_v),
                      m_parameters( parms ) { }
      StringsVisitor( TypeVector &data,
                      const ConversionParameters parms = ConversionParameters() ) : 
                      m_datum( data ),m_default( TypeDefault ),
                      m_parameters( parms ) { }                        
      virtual ~StringsVisitor() = default;


      /** Check if a value is valid and not the default type */
      inline bool isvalid( const Type &t ) const {
        return ( default_value() != t );
      }

      /** Check if a value is valid and not the default type */
      inline bool isequal( const Type &t1, const Type &t2 ) const {
        if ( !( isvalid( t1 ) && isvalid( t2) ) ) return ( false );
        return ( t1 == t2 );
      }
      
      
      inline void operator()( const bool b ) {

        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( ( b ? "true" : "false" ) );
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };

        parameters().extractor( 1, process );
      }

      inline void operator()( const int i )  {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( std::to_string( i ) );
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( 1, process );
      }

      inline void operator()( const size_t i )  {

        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( std::to_string( i ) );
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( 1, process );        
      }          

      inline void operator()( const double d ) {

        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            std::ostringstream out;
            out << std::fixed << std::setprecision( parameters().traits().digits() ) << d;               
            m_datum.push_back( out.str() );
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( 1, process );         
      }

      inline void operator()( const std::string &s ) {

       auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( s );
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( 1, process );         
      }    

      inline void operator()( const std::vector<int> i_array ) {

       auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( std::to_string( i_array[index] ) ); 
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( i_array.size(), process ); 
      }

      inline void operator()( const std::vector<size_t> i_array ) {

       auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( std::to_string( i_array[index] ) ); 
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( i_array.size(), process ); 
      }

      inline void operator()( const std::vector<double> &d_array ) {
       auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            std::ostringstream out;
            out << std::fixed << std::setprecision( parameters().traits().digits() ) << d_array[index];               
            m_datum.push_back( out.str() );  
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( d_array.size(), process ); 
      }
      
      inline void operator()( const std::vector<std::string> &s_array ) {

       auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( s_array[index] ); 
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( s_array.size(), process ); 
      }      
      
      inline void operator()( const ordered_json &j_data ) {

        Type value = default_value();

        /** This lambda processes a scalar value  */
        auto process_scalar = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            if ( j_data.is_string() ) {
              value = j_data;
            }
            else {
              value = to_string( j_data );
            }
          }
          else {
            value = default_value();
          }
          m_datum.push_back( value );       
        };

        /** This lambda processes a JSON array at the index */
        auto process_array = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            // Got an array, these values must be a number of string
            if ( j_data.at(index).is_string() ) {
              value = j_data.at(index);
            }
            else {            
              value = to_string( j_data.at(index) );
            }
          }
          else {
            value = default_value();
          }
          m_datum.push_back( value );                 
        }; 

        // Preliminary processing of the JSON structure to determine its nature
        size_t level = 0;
        try { 
          // Now check if we actually have primitives or arrays
          if ( j_data.is_primitive() ) {
            parameters().extractor( 1, process_scalar );        
          }
          else if ( j_data.is_array() ) {
            parameters().extractor( j_data.size(), process_array );        
          }
        }
        catch ( json::exception & j ) {
          // All errors just result in default value
          value = default_value();
          m_datum.push_back( value );        
        }
      }

      inline const Type &default_value() const {
        return ( m_default );
      }

      inline const ConversionParameters &parameters( ) const {
        return  ( m_parameters );
      }

      /** Return conversion parameters */
      inline const ConversionTraits &traits( ) const {
        return  ( this->parameters().traits() );
      }
      
      /** Return the refenerence to the data */
      inline TypeVector &data() const {
        return ( m_datum );
      }    

    private:
      /** Reference to vector that collects strings */
      TypeVector           &m_datum;
      ConversionParameters  m_parameters;
      Type                  m_default; 
  };


}    // namespace psmrts::algoriths::conversions

#endif
