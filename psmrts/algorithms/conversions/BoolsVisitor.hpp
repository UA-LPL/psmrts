#pragma once

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>

namespace psmrts::algorithms::conversions {

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
            return ( t ); // Might need a change
        }
        
        inline bool isequal( const Type &t1, const Type &t2 ) const {
            return ( t1 == t2 );
        }

        inline void operator()( const bool b ) {
            if ( add_it( 0, 1 ) ) {
              m_datum.push_back( b );
            }
            else {
              m_datum.push_back ( default_value());
            }
        }

        inline void operator()( const int i )  {
            if ( add_it( 0, 1 ) ) {
              m_datum.push_back( ( ( i != 0 ) ? true : false ) );
            }
            else {
              m_datum.push_back ( default_value() );
            }            
        }

        inline void operator()( const size_t i )  {
            if ( add_it( 0, 1 ) ) {
              m_datum.push_back( ( ( i != 0 ) ? true : false ));
            }
            else {
              m_datum.push_back ( default_value() );
            }    
        }          

        inline void operator()( const double d ) {
            if ( add_it( 0, 1 ) ) {
              m_datum.push_back( ( ( d != 0.0 ) ? true : false ) );
            }
            else {
              m_datum.push_back ( default_value());
            }                
        }

        inline void operator()( const std::string &s ) {
            if ( add_it( 0, 1 ) ) {
              // m_datum.push_back( s );
              m_datum.push_back( string_to_bool(s, m_default) );
            }
            else {
              m_datum.push_back ( default_value() );
            }               
        }    

        inline void operator()( const std::vector<int> i_array ) {
            size_t ith = parameters().index();
            size_t nth = parameters().count() + ith;

            for (size_t i = ith ; i < nth ; nth++ ) {
              if ( add_it( i, i_array.size() ) ) {
                //m_datum.push_back( std::to_string( i_array[i] ) ); 
                m_datum.push_back( ( ( i_array[i] != 0 ) ? true : false ) );
              }
              else {
                m_datum.push_back ( default_value() );
              }             
            }
        }

        inline void operator()( const std::vector<size_t> i_array ) {
            size_t ith = parameters().index();
            size_t nth = parameters().count() + ith;

            for (size_t i = ith ; i < nth  ; i++ ) { // Should be i++?
              if ( add_it( i, i_array.size() ) ) {
                //m_datum.push_back( std::to_string( i_array[i] ) );
                m_datum.push_back( ( ( i_array[i] != 0 ) ? true : false ) ); 
              }
              else {
                m_datum.push_back ( default_value());
              }             
            }            
        }

        inline void operator()( const std::vector<double> &d_array ) {
            size_t ith = parameters().index();
            size_t nth = parameters().count() + ith;

            for (size_t i = ith ; i < nth  ; i++ ) {
              if ( add_it( i, d_array.size() ) ) {
                //std::ostringstream out;
                m_datum.push_back( ( ( d_array[i] != 0.0 ) ? true : false ) );                
              }
              else {
                m_datum.push_back ( default_value() );
              }             
            }             
        }
          
        inline void operator()( const std::vector<std::string> &s_array ) {
            size_t ith = parameters().index();
            size_t nth = parameters().count() + ith;

            for (size_t i = ith ; i < nth  ; i++ ) {
              if ( add_it( i, s_array.size() ) ) {
                //m_datum.push_back( s_array[i] );
                m_datum.push_back( ( string_to_bool(s_array[i], m_default) ) );                
              }
              else {
                m_datum.push_back ( default_value() );
              }             
            }              
        }

        inline void operator()( const ordered_json &j_data ) {
            // Not simple but look for the easiest way to process this
            auto it_j = j_data.begin();

            bool value = default_value();  // Set default return condition
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
                    value = *it_j;  // THIS IS A PROBLEM! SHOULD be bool!
                  }
                  else if ( it_j->is_string() ) {
                    std::string temp_s = *it_j;
                    value = string_to_bool( temp_s );
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
                    value = string_to_bool( temp_s );
                  }
                }
              }
            }
            catch ( json::exception & j ) {
              // All errors just result in default value
              value = default_value();
            }

            // It is what it is.. 
            m_datum.push_back( ( ( value != 0.0 ) ? true : false ) );
            return;
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

        inline bool add_it(const size_t index, const size_t max_valid_size ) const {
            if ( ( index >= parameters().index() ) && ( index < max_valid_size ) ) {
                return ( true );
            }
            return ( false );
        }
  };
}   // namespace psmrts::algorithms::conversions