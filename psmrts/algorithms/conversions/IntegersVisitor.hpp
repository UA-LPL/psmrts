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
                       m_datum( data ), m_default( default_v ),
                       m_parameters( params ) { }
      IntegersVisitor( TypeVector &data,
                       const ConversionParameters &params = ConversionParameters() ) : 
                       m_datum( data ), m_default( TypeDefault ),
                       m_parameters( params ) { }
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
              m_datum.push_back( std::numeric_limits<Type>::max() );
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
                m_datum.push_back( std::numeric_limits<Type>::max() );
            }
            else if (d <= std::numeric_limits<Type>::min() ) {
                m_datum.push_back( std::numeric_limits<Type>::min() );
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
                m_datum.push_back( std::numeric_limits<Type>::max() );
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
            //m_datum.push_back( d_array[index] );                
            if (d_array[index] >= std::numeric_limits<Type>::max() ) {
                m_datum.push_back( std::numeric_limits<Type>::max() );
            }
            else if (d_array[index] <= std::numeric_limits<Type>::min() ) {
                m_datum.push_back( std::numeric_limits<Type>::min() );
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
        auto it_j = j_data.begin();

        /** This lambda processes a scalar value  */
        auto process_scalar = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            if ( it_j->is_number() ) {
              // Try direct assignement
              value = *it_j;
            }
            else if ( it_j->is_string() ) {
              std::string temp_s = *it_j;
              value = string_to_int( temp_s );
            }
          }
        };

        /** This lambda processes a JSON array at the index */
        auto process_array = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            // Got an array, these values must be a number of string
            if ( it_j->at(index).is_number( ) ) {
              value = it_j->at(index);
            }
            else if ( it_j->at(index).is_string() ) {
              std::string temp_s = it_j->at(index);
              value = string_to_int( temp_s );
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
                return std::numeric_limits<Type>::max();
            }
            else if (val <= std::numeric_limits<Type>::min() ) {
                return std::numeric_limits<Type>::min();
            } 
            else {
                return static_cast<Type>( val );
            }
          }
          catch( ... ) {
            return ( default_value() );
          }
          return ( default_value() );
          /**
            size_t bad_char_index;
            try {
              int i = std::stoi( s, &bad_char_index );
              if ( bad_char_index != s.length( ) ) return ( default_value() );
              return ( i );
            }
            catch ( std::exception &e) {
              return ( default_value() );
            }

            return ( default_value() );
            */
        }
        /** -- May be unnecessary with lambda implementation / remove when directed --
        inline bool add_it(const size_t index, const size_t max_valid_size ) const {
            if ( ( index >= parameters().index() ) && ( index < max_valid_size ) ) {
              return ( true );
            }
            return ( false );
        }*/
  };

}   // namespace psmrts::algorithms::conversions

#endif
