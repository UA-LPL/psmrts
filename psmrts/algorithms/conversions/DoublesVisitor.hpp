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
                      const double &default_v,
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
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( ( b ? 1.0 : 0.0 ) );
        }
        else {
          m_datum.push_back ( default_value());
        }
      }

      inline void operator()( const int i )  {
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( static_cast<double>( i ) );
        }
        else {
          m_datum.push_back ( default_value() );
        }            
      }

      inline void operator()( const size_t i )  {
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( static_cast<double>( i ) );
        }
        else {
          m_datum.push_back ( default_value() );
        }    
      }          

      inline void operator()( const double d ) {
        if ( add_it( 0, 1 ) ) {               
          m_datum.push_back( d );
        }
        else {
          m_datum.push_back ( default_value());
        }                
      }

      inline void operator()( const std::string &s ) {
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( string_to_double( s ) );
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
            m_datum.push_back( static_cast<double>( i_array[i] ) ); 
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
            m_datum.push_back( static_cast<double>( i_array[i] ) ); 
          }
          else {
            m_datum.push_back ( default_value());
          }             
        }            
      }

      inline void operator()( const TypeVector &d_array ) {
        size_t ith = parameters().index();
        size_t nth = parameters().count() + ith;

        for ( size_t i = ith ; i < nth  ; i++ ) {
          if ( add_it( i, d_array.size() ) ) {
            m_datum.push_back( d_array[i] );                
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
            m_datum.push_back( string_to_double( s_array[i] ) );                
          }
          else {
            m_datum.push_back ( default_value() );
          }             
        }              
      }      
      
      inline void operator()( const ordered_json &j_data ) {
        // Not simple but look for the easiest way to process this
        auto it_j = j_data.begin();

        double value = default_value();  // Set default return condition
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
            // std::cout << "DoublesVisitor::Json::primitive..." << std::endl;

            // Check if its ok to get a scaler
            if ( add_it( 0, it_j->size() ) ) {  
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
          }
          else if ( it_j->is_array() ) {
            // Got an array, these values must be a number of string
            // std::cout << "DoublesVisitor::Json::array..." << std::endl;
            size_t ith = parameters().index();
            size_t nth = parameters().count() + ith;
            if ( add_it( ith, it_j->size() ) ) {
              if ( it_j->at(ith).is_number( ) ) {
                // std::cout << "DoublesVisitor::Json::number..." << std::endl;
                value = it_j->at(ith);
              }
              else if ( it_j->at(ith).is_string() ) {
                // std::cout << "DoublesVisitor::Json::string..." << std::endl;
                std::string temp_s = it_j->at(ith);
                // std::cout << "DoublesVisitor::Json::string: " << temp_s << std::endl;
                value = string_to_double( temp_s );
              }
            }
          }
        }
        catch ( json::exception & j ) {
          // All errors just result in default value
          // std::cout << "DoublesVisitor::Json::exception!" << std::endl;
          value = default_value();
        }

        // It is what it is...
        m_datum.push_back( value );
        return;
      }

      inline const double &default_value() const {
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
      inline double string_to_double( const std::string &s) const {
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
    
      /** Determine if the index is valid given traits and array size */
      inline bool add_it(const size_t index, const size_t max_valid_size ) const {
        if ( ( index >= parameters().index() ) && ( index < max_valid_size ) ) {
          return ( true );
        }
        return ( false );
      }
  };

}    // namespace psmrts::algoriths::conversions
