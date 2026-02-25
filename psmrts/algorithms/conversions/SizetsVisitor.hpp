/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef SizetsVisitor_hpp
#define SizetsVisitor_hpp

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>

namespace psmrts::algorithms::conversions {

/**
   * @brief Option conversion extracts size_t values from ProductOptions
   * 
   * This functor object will extract, converting if necessary, any of the
   * stored intrinsic types. This must be maintained alongside any changes
   * made to ProductOption, particularly any new types added or removed.
   * 
   */
  class SizetsVisitor {
    public:
      using Type        = size_t;
      using TypeVector  = std::vector<Type>;
      static inline const Type TypeDefault = std::numeric_limits<size_t>::max();

      //! SizetsVisitor constructor using a given default value, default_v
      SizetsVisitor( std::vector<size_t> &data,
                      const int &default_v,
                      const ConversionParameters parms = ConversionParameters() ) : 
                      m_datum( data ),
                      m_parameters( parms ),
                      m_default( default_v) { }
      //! SizetsVisitor constructor using TypeDefault as the default value
      SizetsVisitor( TypeVector &data,
                      const ConversionParameters parms = ConversionParameters() ) : 
                      m_datum( data ),
                      m_parameters( parms ),
                      m_default( TypeDefault ) { }                        
      virtual ~SizetsVisitor() = default;

      //! Check if a value is valid and not the default type
      inline bool isvalid( const Type &t ) const {
        return ( default_value() != t );
      }

      //! Determine if two values are equal. Each value is first
      //! validated and confirmed to not be the default type 
      inline bool isequal( const Type &t1, const Type &t2 ) const {
        if ( !( isvalid( t1 ) && isvalid( t2) ) ) return ( false );
        return ( t1 == t2 );
      }
      
      //! operator to add a bool value converted to size_t
      //! adds either the given value or the default value
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

      //! operator to add an integer value converted to size_t
      //! adds either the given value or the default value
      inline void operator()( const int i )  {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            if ( i >= 0 ) {
              m_datum.push_back( static_cast<Type>( i ) );
            }
            else {
              m_datum.push_back ( default_value() );
            }
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( 1, process );        
      }

      //! operator to add a size_t value
      //! adds either the given value or the default value
      inline void operator()( const size_t i )  {
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

      //! operator to add a double value converted to size_t
      //! adds either the given value or the default value
      inline void operator()( const double d ) {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) { 
            if (d >= static_cast<double>( std::numeric_limits<size_t>::min() ) &&
                d <= static_cast<double>( std::numeric_limits<size_t>::max() ) ) {
              m_datum.push_back( static_cast<Type>( d ) );
            }
            else {
              m_datum.push_back( default_value() );
            }
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( 1, process );
      }

      //! operator to add a string value converted to size_t
      //! adds either the given value or the default value
      inline void operator()( const std::string &s ) {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( string_to_sizet( s ) );
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( 1, process );          
      }    

      //! operator to add an vector of integers converted to size_ts
      inline void operator()( const std::vector<int> i_array ) {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            if ( i_array[index] < 0 ||
                 i_array[index] > std::numeric_limits<size_t>::max() ) {
                m_datum.push_back( default_value() );
            }
            else {
                m_datum.push_back( static_cast<Type>( i_array[index] ) );
            } 
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( i_array.size(), process );            
      }

      //! operator to add an vector of size_t values
      inline void operator()( const std::vector<size_t> i_array ) {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( i_array[index] ); 
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( i_array.size(), process ); 
      }

      //! operator to add a vector of doubles converted to size_ts
      inline void operator()( const std::vector<double> &d_array ) {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            if ( ( d_array[index] < 0.0 ) || 
                 ( d_array[index] > static_cast<double>( std::numeric_limits<Type>::max() ) ) ) {
              m_datum.push_back( default_value() );
            }
            else {
              m_datum.push_back( static_cast<Type>( d_array[index] ) );
            }
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( d_array.size(), process );           
      }
     
      //! operator to add a vector of strings converted to size_ts
      inline void operator()( const std::vector<std::string> &s_array ) {
        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( string_to_sizet( s_array[index] ) );                
          }
          else {
            m_datum.push_back( default_value() );
          }
        };
        parameters().extractor( s_array.size(), process );           
      }      
      
      //! operator to add ordered json data
      inline void operator()( const ordered_json &j_data ) {
        // Set default and initial processing conditions
        Type value = default_value();  

        // This lambda processes a scalar value
        auto process_scalar = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            if ( j_data.is_boolean() ) {
              // Try direct assignement
              value = ( j_data == true ) ? static_cast<size_t>(1) : 0;
            }  
            else if ( j_data.is_number() ) {
              // Try direct assignment
              value = j_data;
              // confirm value is within valid range
              if ( value < 0.0 || value > std::numeric_limits<Type>::max() ) {
                value = default_value();
              }
              else {
                value = j_data;
              }
            }
            else if ( j_data.is_string() ) {
              std::string temp_s = j_data;
              value = string_to_sizet( temp_s );
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
              value = ( j_data.at(index)  == true ) ? static_cast<size_t>(1) : 0;
            }   
            // Got an array, these values must be a number or a string
            else if ( j_data.at(index).is_number( ) ) {
              value = j_data.at(index);
            }
            else if ( j_data.at(index).is_string() ) {
              std::string temp_s = j_data.at(index);
              value = string_to_sizet( temp_s );
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

      //! Return default size_t value. This is operating system dependent.
      inline const Type &default_value() const {
        return ( m_default );
      }

      //! Return conversion parameters
      inline const ConversionParameters &parameters() const {
        return ( m_parameters );
      }

      //! Return conversion parameter traits
      inline const ConversionTraits &traits() const {
        return ( this->parameters().traits() );
      }

      //! Return reference to data
      inline TypeVector &data() const {
        return ( m_datum );
      }

    private:
      std::vector<size_t>  &m_datum;     //! Reference to vector that collects size_t values
      ConversionParameters m_parameters; //! Parameters for converting to size_t
      size_t               m_default;    //! default size_t value
    
      //! Convert strings to size_ts with error checking
      //! std::stoul throws std::out_of_range if necessary
      inline size_t string_to_sizet( const std::string &s ) const {
        // checking for negative sign in string
        if ( s[0] == '-' ) {                
          return ( default_value() );
        }

        try {
          size_t len;
          double val = std::stod( s, &len );
          if (len != s.length() ) return ( default_value() );

          if (val >= std::numeric_limits<size_t>::min() &&
              val <= std::numeric_limits<size_t>::max() ) {
            return static_cast<Type>( val );
          }
          else {
            return default_value();
          }
        }
        catch ( ... ) {
          return default_value();
        }
        return default_value();
      }
  };
}    // namespace psmrts::algorithms::conversions

#endif
