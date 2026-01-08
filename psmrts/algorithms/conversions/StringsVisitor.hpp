#pragma once

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
                      const std::string &default_v,
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
        return ( default_value() == t );
      }

      /** Check if a value is valid and not the default type */
      inline bool isequal( const Type &t1, const Type &t2 ) const {
        if ( !( isvalid( t1 ) && isvalid( t2) ) ) return ( false );
        return ( t1 == t2 );
      }
      
      
      inline void operator()( const bool b ) {
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( ( b ? "true" : "false" ) );
        }
        else {
          m_datum.push_back ( default_value());
        }
      }

      inline void operator()( const int i )  {
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( std::to_string( i ));
        }
        else {
          m_datum.push_back ( default_value());
        }            
      }

      inline void operator()( const size_t i )  {
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( std::to_string( i ));
        }
        else {
          m_datum.push_back ( default_value() );
        }    
      }          

      inline void operator()( const double d ) {
        if ( add_it( 0, 1 ) ) {
          std::ostringstream out;
          out << std::fixed << std::setprecision( parameters().traits().digits() ) << d;               
          m_datum.push_back( out.str() );
        }
        else {
          m_datum.push_back ( default_value());
        }                
      }

      inline void operator()( const std::string &s ) {
        if ( add_it( 0, 1 ) ) {
          m_datum.push_back( s );
        }
        else {
          m_datum.push_back ( default_value() );
        }               
      }    

      inline void operator()( const std::vector<int> i_array ) {
        size_t ith = parameters().index();
        size_t nth = parameters().count() + ith;

        for (size_t i = ith ; i < nth ; i++ ) {
          if ( add_it( i, i_array.size() ) ) {
            m_datum.push_back( std::to_string( i_array[i] ) ); 
          }
          else {
            m_datum.push_back ( default_value() );
          }             
        }
      }

      inline void operator()( const std::vector<size_t> i_array ) {
          size_t ith = parameters().index();
        size_t nth = parameters().count() + ith;

        for (size_t i = ith ; i < nth  ; i++ ) {
          if ( add_it( i, i_array.size() ) ) {
            m_datum.push_back( std::to_string( i_array[i] ) ); 
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
            std::ostringstream out;
            out << std::fixed << std::setprecision( parameters().traits().digits() ) << d_array[i];               
            m_datum.push_back( out.str() );                
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
            m_datum.push_back( s_array[i] );                
          }
          else {
            m_datum.push_back ( default_value() );
          }             
        }              
      }      
      
      inline void operator()( const ordered_json &j_data ) {
        if ( add_it( 0, 1 ) ) {
          if ( j_data.is_primitive() ) {
            std::string str_t = j_data;
            m_datum.push_back( str_t );
          }
          else {        
            m_datum.push_back( j_data.dump( parameters().traits().spaces() ) );
          }
        }
        else {
          m_datum.push_back( default_value() );
        }
      }

      inline const std::string &default_value() const {
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
      std::vector<std::string> &m_datum;
      ConversionParameters     m_parameters;
      std::string              m_default; 
      
      /** Determine if the index is valid given traits and array size */
      inline bool add_it(const size_t index, const size_t max_valid_size ) const {
        if ( ( index >= parameters().index() ) && ( index < max_valid_size ) ) {
          return ( true );
        }
        return ( false );
      }
  };


}    // namespace psmrts::algoriths::conversions
