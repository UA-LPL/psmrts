/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef BoolsVisitor_hpp
#define BoolsVisitor_hpp

#include <stdexcept>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>

namespace psmrts::algorithms::conversions {

  /**
   * @brief Option conversion extracts boolean values from ProductOptions
   * 
   * This functor object will extract, converting if necessary, any of the
   * stored intrinsic types. This must be maintained alongside ay changes
   * made to ProductOption, particularly any new types added or removed.
   * 
   */
  class BoolsVisitor {
    public:
        using Type       = bool;
        using TypeVector = std::vector<Type>;
        static inline const Type TypeDefault = false;

        BoolsVisitor( TypeVector &data,
                      const Type &default_v,
                      const ConversionParameters &params = ConversionParameters() ) :
                      m_datum( data ), m_default( default_v ),
                      m_parameters( params ) { }
        BoolsVisitor( TypeVector &data,
                      const ConversionParameters &params = ConversionParameters() ) :
                      m_datum( data ), m_default( TypeDefault ),
                      m_parameters( params ) { }
        virtual ~BoolsVisitor() = default;

        /** Check if a value is valid and not the default type */
        inline bool isvalid( const Type &t ) const {
        // If value is successfully converted to a bool, it will always be valid 
        // ( ignores comparison to default )
            return ( true );
        }
        
        inline bool isequal( const Type &t1, const Type &t2 ) const {
            return ( t1 == t2 );
        }

        inline void operator()( const bool b ) {
          auto process = [&]( const bool addit, const size_t index ) {
            if ( addit ) {
              m_datum.push_back( b );
            } else {
              m_datum.push_back( default_value() );
            }
          };
          parameters().extractor( 1 , process );
        }

        inline void operator()( const int i )  {
          auto process = [&]( const bool addit, const size_t index ) {
            if ( addit ) {
              m_datum.push_back( ( ( i != 0 ) ? true : false ) );
            } else {
              m_datum.push_back ( default_value() );
            }
          };
          parameters().extractor( 1, process ); 
        }

        inline void operator()( const size_t i )  {
          auto process = [&]( const bool addit, const size_t index ) {
            if ( addit ) {
              m_datum.push_back( ( ( i != 0 ) ? true : false ));
            } else {
              m_datum.push_back ( default_value() );
            }
          };
          parameters().extractor( 1, process );  
        }          

        inline void operator()( const double d ) {
          auto process = [&]( const bool addit, const size_t index ) {
            if ( addit ) {
              m_datum.push_back( ( ( d != 0.0 ) ? true : false ) );
            } else {
              m_datum.push_back ( default_value() );
            }
          };
          parameters().extractor( 1, process );              
        }

        inline void operator()( const std::string &s ) {
          auto process = [&]( const bool addit, const size_t index ) {
            if ( addit ) {
              m_datum.push_back( string_to_bool(s, m_default) );
            } else {
              m_datum.push_back ( default_value() );
            }
          };
          parameters().extractor( 1, process );            
        }    

        inline void operator()( const std::vector<int> i_array ) {
          auto process = [&]( const bool addit, const size_t index ) {
            if ( addit ) {
              m_datum.push_back( ( ( i_array[index] != 0 ) ? true : false ) );
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
              m_datum.push_back( ( ( i_array[index] != 0 ) ? true : false ) ); 
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
              m_datum.push_back( ( ( d_array[index] != 0.0 ) ? true : false ) );                
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
              m_datum.push_back( ( string_to_bool(s_array[index], m_default) ) );                
            }
            else {
              m_datum.push_back ( default_value() );
            }
          };
          parameters().extractor( s_array.size(), process );             
        }

        inline void operator()( const ordered_json &j_data ) {
          // Set default and initial processing conditions
          Type value = default_value();  

          /** This lambda processes a scalar value  */
          auto process_scalar = [&]( const bool addit, const size_t index ) {
            if ( addit ) {
              if ( j_data.is_boolean() ) {
                // Try direct assignement
                value = j_data;
              } 
              else if ( j_data.is_number() ) {
                // Try direct assignement
                value = ( ( j_data != 0.0 ) ? true : false );
              }
              else if ( j_data.is_string() ) {
                std::string temp_s = j_data;
                value = string_to_bool( temp_s, m_default );
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
                value = j_data.at(index);
              }  
              // Got an array, these values must be a number of string
              else if ( j_data.at(index).is_number( ) ) {
                value = ( ( j_data.at(index) != 0.0 ) ? true : false );
              }
              else if ( j_data.at(index).is_string() ) {
                std::string temp_s = j_data.at(index);
                value = string_to_bool( temp_s, m_default );
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

        inline const ConversionParameters &parameters() const {
            return ( m_parameters );
        }

        inline const ConversionTraits &traits() const {
            return ( this->parameters().traits() );
        }

        inline TypeVector &data() const {
            return ( m_datum );
        }

    private:
        TypeVector           &m_datum;
        ConversionParameters m_parameters;
        Type                 m_default;

        /**
         * @brief Converts a string to the associable boolean equivalent
         * 
         * This fuction anticipates the following string values:
         * 
         *   { true, 1, yes, on, false, 0, no, off }
         * 
         * String inputs that contain numerical types will need to be converted to those 
         * actual types for conversion. 
         * 
         * @param s          String input to be converted
         * @param default_v  Default bool value
         * @return true      
         * @return false 
         */
        static bool string_to_bool(const std::string &s, const bool default_v) {
            if (s.empty()) return default_v;
            
            std::string v = psmrts::psmrts_tolower(s);
            if (v == "true" || v == "1" || v == "yes" || v == "on") {
              return true;
            }
          
            if (v == "false" || v == "0" || v == "no" || v == "off") {
              return false;
            }

            return default_v;
        }
  };
}   // namespace psmrts::algorithms::conversions

#endif
