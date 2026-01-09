#pragma once

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
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( ( b ? 1 : 0 ) );
        }
        else {
          m_datum.push_back ( default_value());
        }
      }

      inline void operator()( const int i )  {
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( i );
        }
        else {
          m_datum.push_back ( default_value() );
        }            
      }

      inline void operator()( const size_t i )  {
        if ( add_it( 0, 1 ) ) {
          if ( i >= std::numeric_limits<Type>::max() ) {
            m_datum.push_back( std::numeric_limits<Type>::max() );
          } else {
            m_datum.push_back( static_cast<Type>( i ) );
          }
        }
        else {
          m_datum.push_back ( default_value() );
        }    
      }          

      inline void operator()( const double d ) {
        if ( add_it( 0, 1 ) ) { 
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
          m_datum.push_back ( default_value());
        }                
      }

      inline void operator()( const std::string &s ) {
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( string_to_int( s ) );
        }
        else {
          m_datum.push_back ( default_value() );
        }               
      }    

      inline void operator()( const std::vector<int> i_array ) {
        size_t ith = parameters().index();
        size_t nth = parameters().count() + ith;

        for ( size_t i = ith ; i < nth ; i++ ) {
          if ( add_it( i, i_array.size() ) ) {
            m_datum.push_back( i_array[i]  ); 
          }
          else {
            m_datum.push_back ( default_value() );
          }             
        }
      }

      inline void operator()( const std::vector<size_t> i_array ) {
        size_t ith = parameters().index();
        size_t nth = parameters().count() + ith;

        for ( size_t i = ith ; i < nth  ; i++ ) {
          if ( add_it( i, i_array.size() ) ) {
            if ( i_array[i] >= std::numeric_limits<Type>::max() ) {
                m_datum.push_back( std::numeric_limits<Type>::max() );
            } else {
                m_datum.push_back( static_cast<int>( i_array[i] ) );
            }
          }
          else {
            m_datum.push_back ( default_value());
          }             
        }            
      }

      inline void operator()( const std::vector<double> &d_array ) {
        size_t ith = parameters().index();
        size_t nth = parameters().count() + ith;

        for ( size_t i = ith ; i < nth  ; i++ ) {
          if ( add_it( i, d_array.size() ) ) {
            if (d_array[i] >= std::numeric_limits<Type>::max() ) {
                m_datum.push_back( std::numeric_limits<Type>::max() );
            }
            else if (d_array[i] <= std::numeric_limits<Type>::min() ) {
                m_datum.push_back( std::numeric_limits<Type>::min() );
            } 
            else {
                m_datum.push_back( static_cast<int>( d_array[i] ) );
            }              
          }
          else {
            m_datum.push_back ( default_value() );
          }             
        }             
      }
      
      inline void operator()( const std::vector<std::string> &s_array ) {
        size_t ith = parameters().index();
        size_t nth = parameters().count() + ith;

        for ( size_t i = ith ; i < nth  ; i++ ) {
          if ( add_it( i, s_array.size() ) ) {
            m_datum.push_back( string_to_int( s_array[i] ) );                
          }
          else {
            m_datum.push_back ( default_value() );
          }             
        }              
      }      
      
      inline void operator()( const ordered_json &j_data ) {
        // Not simple but look for the easiest way to process this
        auto it_j = j_data.begin();

        int value = default_value();  // Set default return condition
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

            // Check if its ok to get a scaler
            if ( add_it( 0, it_j->size() ) ) {  
              if ( it_j->is_number() ) {
                // Try direct assignement
                value = *it_j;
              }
              else if ( it_j->is_string() ) {
                std::string temp_s = *it_j;
                value = string_to_int( temp_s );
              }
            }            
          }
          else if ( it_j->is_array() ) {
            // Got an array, these values must be a number of string
            size_t ith = parameters().index();
            size_t nth = parameters().count() + ith;
            if ( add_it( ith, it_j->size() ) ) {
              if ( it_j->at(ith).is_number( ) ) {
                value = it_j->at(ith);
              }
              else if ( it_j->at(ith).is_string() ) {
                std::string temp_s = it_j->at(ith);
                value = string_to_int( temp_s );
              }
            }
          }
        }
        catch ( json::exception & j ) {
          // All errors just result in default value
          value = default_value();
        }

        // It is what it is...
        m_datum.push_back( static_cast<Type>( value ) );
        return;
      }

      inline const Type &default_value() const {
        return ( m_default );
      }

      /** Return conversion parameters */
      inline const ConversionParameters &parameters() const {
        return ( m_parameters );
      }

      /** Return conversion parameter traits */
      inline const ConversionParameters &traits() const {
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

        inline double string_to_int( const std::string &s ) const {
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
        }

        inline bool add_it(const size_t index, const size_t max_valid_size ) const {
            if ( ( index >= parameters().index() ) && ( index < max_valid_size ) ) {
              return ( true );
            }
            return ( false );
        }
  };

}   // namespace psmrts::algorithms::conversions