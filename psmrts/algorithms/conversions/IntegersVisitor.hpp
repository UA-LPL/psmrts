#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef IntegersVisitor_hpp
#define IntegersVisitor_hpp

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>

namespace psmrts::algorithms::conversions {
    
  class IntegersVisitor {
    public:
      using Type = int;
      using TypeVector = std::vector<Type>;
      static inline const Type TypeDefault = std::numeric_limits<int>::min();

      IntegersVisitor( TypeVector &data,
                       const Type &default_v,
                       const ConversionParameters &params = ConversionParameters() ) :
                       m_datum( data ), 
                       m_parameters( params ),
                       m_default( default_v ) { }
      IntegersVisitor( TypeVector &data,
                       const ConversionParameters &params = ConversionParameters() ) : 
                       m_datum( data ),
                       m_parameters( params ),
                       m_default( TypeDefault ) { }
      virtual ~IntegersVisitor() = default;

      inline bool isvalid( const Type &t ) const {
        if ( t == default_value() ) return ( false );
        return true;
      }

      inline bool isequal( const Type &t1, const Type &t2 ) const {
        if ( !( isvalid( t1 ) && isvalid( t2 ) ) ) return ( false );
        return ( t1 == t2 );
      }

      inline void operator()( const bool b ) {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( ( b ? 1 : 0 ) );
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
            m_datum.push_back( i );
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
            if ( i >= std::numeric_limits<Type>::max() ) {
              m_datum.push_back( default_value());
            } else {
              m_datum.push_back( static_cast<Type>( i ) );
            }
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
            if (d >= std::numeric_limits<Type>::max() ) {
                m_datum.push_back( default_value());
            }
            else if (d <= std::numeric_limits<Type>::min() ) {
                m_datum.push_back( default_value() );
            } 
            else {
                m_datum.push_back( static_cast<Type>( d ) );
            }
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
            m_datum.push_back( string_to_int( s ) );
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
            m_datum.push_back(  i_array[index]  ); 
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
            if ( i_array[index] >= std::numeric_limits<Type>::max() ) {
                m_datum.push_back( default_value() );
            } else {
                m_datum.push_back( static_cast<int>( i_array[index] ) );
            } 
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
            if (d_array[index] >= std::numeric_limits<Type>::max() ) {
                m_datum.push_back( default_value() );
            }
            else if (d_array[index] <= std::numeric_limits<Type>::min() ) {
                m_datum.push_back( default_value() );
            } 
            else {
                m_datum.push_back( static_cast<int>( d_array[index] ) );
            }
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
            m_datum.push_back( string_to_int( s_array[index] ) );                
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

        /** This lambda processes a scalar value  */
        auto process_scalar = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            if ( j_data.is_boolean() ) {
              // Try direct assignement
              value = ( j_data == true ) ? 1 : 0;
            } 
            else if ( j_data.is_number() ) {
              // Try direct assignement
              value = static_cast<int>( j_data );
            }
            else if ( j_data.is_string() ) {
              std::string temp_s = j_data;
              value = string_to_int( temp_s );
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
            if ( j_data.at(index).is_boolean() ) {
              // Try direct assignement
              value = ( j_data.at(index)  == true ) ? 1 : 0;
            }   
            // Got an array, these values must be a number of string
            else if ( j_data.at(index).is_number( ) ) {
              value = static_cast<int>( j_data.at(index) );
            }
            else if ( j_data.at(index).is_string() ) {
              std::string temp_s = j_data.at(index);
              value = string_to_int( temp_s );
            }
          }
          else {
            value = default_value();
          }
          m_datum.push_back( value );             
        }; 

        // Preliminary processing of the JSON structure to determine its nature
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

      /** Return conversion parameters */
      inline const ConversionParameters &parameters() const {
        return ( m_parameters );
      }

      /** Return conversion parameter traits */
      inline const ConversionTraits &traits() const {
        return ( this->parameters().traits() );
      }

      /** Return reference to data */
      inline TypeVector &data() const {
        return ( m_datum );
      }

    private:
        TypeVector           &m_datum;
        ConversionParameters m_parameters;
        Type                 m_default;

        inline int string_to_int( const std::string &s ) const {
          try {
            size_t len;
            double val = std::stod( s, &len );
            if (len != s.length() ) return ( default_value() );

            if ( val >= std::numeric_limits<Type>::max() ) {
                return ( default_value() );
            }
            else if (val <= std::numeric_limits<Type>::min() ) {
                return ( default_value() );
            } 
            else {
                return static_cast<Type>( val );
            }
          }
          catch( ... ) {
            return ( default_value() );
          }
          return ( default_value() );
        }
  };

}   // namespace psmrts::algorithms::conversions

#endif
