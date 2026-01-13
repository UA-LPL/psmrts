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
            return ( t != m_default ); // value is not the set default
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
          auto it_j = j_data.begin();

          /** This lambda processes a scalar value  */
          auto process_scalar = [&]( const bool addit, const size_t index ) {
            if ( addit ) {
              if ( it_j->is_number() ) {
                // Try direct assignement
                //value = *it_j;
                value = ( ( *it_j != 0.0 ) ? true : false );
              }
              else if ( it_j->is_string() ) {
                std::string temp_s = *it_j;
                value = string_to_bool( temp_s, m_default );
              }
            }
          };
          /** This lambda processes a JSON array at the index */
          auto process_array = [&]( const bool addit, const size_t index ) {
            if ( addit ) {
              // Got an array, these values must be a number of string
              if ( it_j->at(index).is_number( ) ) {
                //value = it_j->at(index);
                value = ( ( it_j->at(index) != 0.0 ) ? true : false );
              }
              else if ( it_j->at(index).is_string() ) {
                std::string temp_s = it_j->at(index);
                value = string_to_bool( temp_s, m_default );
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
            value = default_value();
          }

          // It is what it is...
          m_datum.push_back( value );
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
        /** -- May be unnecessary with lambda implementation / remove when directed --
        inline bool add_it(const size_t index, const size_t max_valid_size ) const {
            if ( ( index >= parameters().index() ) && ( index < max_valid_size ) ) {
                return ( true );
            }
            return ( false );
        }
        */
  };
}   // namespace psmrts::algorithms::conversions

#endif
