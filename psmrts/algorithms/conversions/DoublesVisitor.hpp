#pragma once

#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>

namespace psmrts::algorithms::conversions {


/**
   * @brief Option conversion extracts double values from ProductOptions
   * 
   * This functor object will extract, converting if necessary, any of the
   * stored intrinsic types. This must be maintained alongside any changes
   * made to ProductOption, paticular any new types added or removed.
   * 
   * @author 2026-01-06 Kris J Becker
   */
 
  class DoublesVisitor { 
    public:
      using Type        = double;
      using TypeVector  = std::vector<Type>;
      static inline const Type TypeDefault = psmrts::null();

      DoublesVisitor( TypeVector &data,
                      const Type &default_v,
                      const ConversionParameters &parms = ConversionParameters() ) : 
                      m_datum( data ),m_default( default_v ),
                      m_parameters( parms ) { }
      DoublesVisitor( TypeVector &data,
                      const ConversionParameters &parms = ConversionParameters() ) : 
                      m_datum( data ),m_default( TypeDefault ),
                      m_parameters( parms ) { }    
      virtual ~DoublesVisitor() = default;


      /** Check if a value is valid and not the default type */
      inline bool isvalid( const Type &t ) const {
        if ( psmrts::isnull( default_value() ) || psmrts::isnull( t ) ) return ( false );
        return ( !psmrts::isApprox( t, default_value(), traits().tolerance() ) );
      }

      /** Check if a value is valid and not the default type */
      inline bool isequal( const Type &t1, const Type &t2 ) const {
        if ( !( isvalid( t1 ) && isvalid( t2) ) ) return ( false );
        return ( !psmrts::isApprox( t1, t2, traits().tolerance() ) );
      }
      
      inline void operator()( const bool b ) {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( ( b ? 1.0 : 0.0 ) );
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
            m_datum.push_back( static_cast<Type>( i ) );
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
            m_datum.push_back( static_cast<Type>( i ) );
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
            m_datum.push_back( d );
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
            m_datum.push_back( string_to_double( s ) );
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
            m_datum.push_back( static_cast<Type>( i_array[index] ) ); 
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
            m_datum.push_back( static_cast<Type>( i_array[index] ) ); 
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
             m_datum.push_back( d_array[index] );                

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
            m_datum.push_back( string_to_double( s_array[index] ) );                
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( s_array.size(), process );
      }      
      
      inline void operator()( const ordered_json &j_data ) {

        // Set default and initial processing connditions
        Type value = default_value();  
        auto it_j = j_data.begin();

        /** This lambda processes a scalar value  */
        auto process_scalar = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            if ( it_j->is_number() ) {
              // std::cout << "DoublesVisitor::Json::number..." << std::endl;
              // Try direct assignement
              value = *it_j;
            }
            else if ( it_j->is_string() ) {
              // std::cout << "DoublesVisitor::Json::string..." << std::endl;
              std::string temp_s = *it_j;
              //  std::cout << "DoublesVisitor::Json::string: " << temp_s << std::endl;
              value = string_to_double( temp_s );
            }
          }
        };

        /** This lambda processes a JSON array at the index */
        auto process_array = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            // Got an array, these values must be a number of string
            // std::cout << "DoublesVisitor::Json::array..." << std::endl;
            if ( it_j->at(index).is_number( ) ) {
              // std::cout << "DoublesVisitor::Json::number..." << std::endl;
              value = it_j->at(index);
            }
            else if ( it_j->at(index).is_string() ) {
              // std::cout << "DoublesVisitor::Json::string..." << std::endl;
              std::string temp_s = it_j->at(index);
              // std::cout << "DoublesVisitor::Json::string: " << temp_s << std::endl;
              value = string_to_double( temp_s );
            }
          }
        }; 

        // Preliminary processing of the JSON structure to determine its nature
        size_t level = 0;
        try { 

          // Find the first primitive or array
          while ( it_j->is_structured() && ( it_j != j_data.end() )) {
            if ( it_j->is_array() )     break;
            if ( it_j->is_primitive() ) break;
            level++;
            ++it_j;
          }
          // std::cout << "DoublesVisitor::Json::level: " << level << std::endl;
          // std::cout << "DoublesVisitor::Json::value: " << *it_j << std::endl;

          // Now check if we actually have primitives or arrays
          if ( it_j->is_primitive() ) {
            parameters().extractor( 1, process_scalar );        
          }
          else if ( it_j->is_array() ) {
            parameters().extractor( it_j->size(), process_array );        
          }
        }
        catch ( json::exception & j ) {
          // All errors just result in default value
          // std::cout << "DoublesVisitor::Json::exception!" << std::endl;
          value = default_value();
        }

        // It is what it is...
        m_datum.push_back( value );
      }

      inline const Type &default_value() const {
        return ( m_default );
      }

      /** Return conversion parameters */
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
      /** Reference to vector that collects doubles */
      TypeVector           &m_datum;
      ConversionParameters m_parameters;
      Type                 m_default; 
    
      /** Convert strings to double precision with error checking */
      inline Type string_to_double( const std::string &s) const {
        size_t bad_char_index;
        try {
          double d = std::stod( s, &bad_char_index );
          if ( bad_char_index != s.length( ) ) return ( default_value() );
          return ( d );
        }
        catch ( std::exception &e) {
          return ( default_value() );
        }

        return ( default_value() );
      }
  };

}    // namespace psmrts::algoriths::conversions
