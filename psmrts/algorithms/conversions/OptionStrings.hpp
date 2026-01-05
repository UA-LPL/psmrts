#pragma once

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
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
  class OptionStrings {
    public:

      OptionStrings() : m_option(), m_default("") { }
      OptionStrings( const ProductOption &option ) {
        m_option  = option;
        m_traits  = ConversionTraits();
        m_default = "";
      }      
      OptionStrings( const ProductOption &option,
                     const ConversionTraits &traits,
                     const std::string default_v = "" ) {
        m_option  = option;
        m_traits  = traits;
        m_default = default_v;
      }
      OptionStrings( const ProductOption &option,
                     const std::string default_v ) {
        m_option  = option;
        m_traits  = ConversionTraits();
        m_default = default_v;
      }        

      virtual ~OptionStrings() = default;

      inline size_t size() const {
        return ( m_option.size() );
      }
            
      inline ProductOption::DataEnums type() const {
        return ( m_option.type() );
      }

      inline const std::string &default_value( ) const {
        return ( m_default );
      }

      inline std::string name() const {
        return ( m_option.name() );
      }

      inline std::string get( const size_t index = 0 ) const {
        ConversionParameters p( index, m_traits );
        std::vector<std::string> one;
        one.reserve( 1 );

        OptionVisitor visitor( one, p );
        m_option.get_to( visitor );

        return ( one.front() );
      }

      inline const std::vector<std::string> &get_all( std::vector<std::string> &s ) const {
        ConversionParameters p = ConversionParameters::get_all_values( m_traits );

        OptionVisitor visitor( s, p );
        m_option.get_to( visitor );

        return ( s );
      }

      /**
       * @brief Compare string conversions of two products
       * 
       * @param option1 
       * @param option2 
       * @param traits 
       * @param default_v 
       * @return true 
       * @return false 
       */
      static inline bool compare( const ProductOption &option1, 
                                  const ProductOption &option2,
                                  const ConversionParameters &parms = ConversionParameters::get_all_values( ),
                                  const std::string default_v = "" )  {

        OptionStrings option1_s( option1, parms.traits(), default_v );
        OptionStrings option2_s( option2, parms.traits(), default_v );
        std::vector<std::string> opt1_v, opt2_v;
        
        opt1_v.reserve( option1_s.size() );
        opt2_v.reserve( option2_s.size() );

        option1_s.get_all( opt1_v );
        option2_s.get_all( opt2_v );

        if ( opt1_v.size(), opt2_v.size() ) {
          return ( false );
        }

        size_t n = std::max( opt1_v.size(), opt2_v.size() );
        for ( size_t i = 0  ; i < n ; i++ ) {
          if ( opt1_v[i] != opt2_v[i] ) return ( false );
        }
        return ( true );
      }

    protected:
      
      class OptionVisitor { 
        public:
          OptionVisitor( std::vector<std::string> &data,
                        const std::string &default_v,
                        const ConversionParameters parms = ConversionParameters() ) : 
                        m_datum( data ),m_default( default_v),
                        m_parameters( parms ) { }
          OptionVisitor( std::vector<std::string> &data,
                        const ConversionParameters parms = ConversionParameters() ) : 
                        m_datum( data ),m_default( ""),
                        m_parameters( parms ) { }                        
          virtual ~OptionVisitor() = default;

          inline void operator()( const bool b ) {
            if ( add_it( 0 ) ) {
              m_datum.push_back( ( b ? "true" : "false" ) );
            }
            else {
              m_datum.push_back ( default_value());
            }
          }

          inline void operator()( const int i )  {
            if ( add_it( 0 ) ) {
              m_datum.push_back( std::to_string( i ));
            }
            else {
              m_datum.push_back ( default_value());
            }            
          }

          inline void operator()( const size_t i )  {
            if ( add_it( 0 ) ) {
              m_datum.push_back( std::to_string( i ));
            }
            else {
              m_datum.push_back ( default_value() );
            }    
          }          

          inline void operator()( const double d ) {
            if ( add_it( 0 ) ) {
              std::ostringstream out;
              out << std::fixed << std::setprecision( parameters().traits().digits() ) << d;               
              m_datum.push_back( out.str() );
            }
            else {
              m_datum.push_back ( default_value());
            }                
          }

          inline void operator()( const std::string &s ) {
            if ( add_it( 0 ) ) {
              m_datum.push_back( s );
            }
            else {
              m_datum.push_back ( default_value() );
            }               
          }    

          inline void operator()( const std::vector<int> i_array ) {
            size_t ith = parameters().index();
            size_t nth = parameters().count() + ith;
            size_t max_n = i_array.size();

            for (size_t i = ith ; i < nth ; nth++ ) {
              if ( add_it( i ) && ( ith < max_n) ) {
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
            size_t max_n = i_array.size();

            for (size_t i = ith ; i < nth  ; nth++ ) {
              if ( add_it( i ) && ( ith < max_n) ) {
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
            size_t max_n = d_array.size();

            for (size_t i = ith ; i < nth  ; nth++ ) {
              if ( add_it( i ) && ( ith < max_n) ) {
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
            size_t max_n = s_array.size();

            for (size_t i = ith ; i < nth  ; nth++ ) {
              if ( add_it( i ) && ( ith < max_n) ) {
                m_datum.push_back( s_array[i] );                
              }
              else {
                m_datum.push_back ( default_value() );
              }             
            }              
          }      
          
          inline void operator()( const ordered_json &j_data ) {
            if ( add_it( 0 ) ) {            
              m_datum.push_back( j_data.dump( parameters().traits().spaces() ) );
            }
            else {
              m_datum.push_back( default_value() );
            }
          }

          inline const std::string &default_value() const {
            return ( m_default );
          }

        private:
          /** Reference to vector that collects strings */
          std::vector<std::string> &m_datum;
          ConversionParameters     m_parameters;
          std::string              m_default; 
          
          inline const ConversionParameters &parameters( ) const {
            return  ( m_parameters );
          }

          inline bool add_it(const size_t index) const {
            if ( parameters().all() && ( index >= parameters().index()) ) {
              return ( true );
            }
            else if ( index == parameters().index() ) {
              return ( true );
            }
            return ( false );
          }
      };


      private:
        ProductOption    m_option;
        ConversionTraits m_traits;
        std::string      m_default;
  };  

}    // namespace psmrts::algoriths::conversions