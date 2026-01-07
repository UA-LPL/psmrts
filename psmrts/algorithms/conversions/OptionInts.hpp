#pragma once

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <limits>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>

namespace psmrts::algorithms::conversions {

/**
   * @brief Option conversion extracts integer values from ProductOptions
   * 
   * This functor object will extract, converting if necessary, any of the
   * stored intrinsic types. This must be maintained alongside any changes
   * made to ProductOption, paticular any new types added or removed.
   * 
   */
  class OptionInts {
    public:
      static inline int OptionIntsDefault = std::numeric_limits<int>::max();
      OptionInts() : m_option(), 
                     m_traits( ConversionTraits() ), 
                     m_default( OptionIntsDefault ) { }
      OptionInts( const ProductOption &option ) {
        m_option  = option;
        m_traits  = ConversionTraits();
        m_default = psmrts::null();
      }      
      OptionInts( const ProductOption &option,
                  const ConversionTraits &traits,
                  const int default_v = OptionIntsDefault  ) {
        m_option  = option;
        m_traits  = traits;
        m_default = default_v;
      }
      OptionInts( const ProductOption &option,
                  const int default_v,
                  const ConversionTraits &traits = ConversionTraits() ) {
        m_option  = option;
        m_traits  = traits;
        m_default = default_v;
      }        

      virtual ~OptionInts() = default;

      inline size_t size() const {
        return ( m_option.size() );
      }
            
      inline ProductOption::DataEnums type() const {
        return ( m_option.type() );
      }

      inline const int &default_value( ) const {
        return ( m_default );
      }

      inline std::string name() const {
        return ( m_option.name() );
      }

      inline int get( const size_t index = 0 ) const {
        ConversionParameters p( index, 1, m_traits );
        std::vector<int> one;
        one.reserve( 1 );

        OptionVisitor visitor( one,  this->default_value(), p );
        m_option.visit( visitor );

        return ( one.front() );
      }

      inline const std::vector<int> &get_all( std::vector<int> &d,
                                                 const size_t index = 0,
                                                 const size_t nvals = 0 ) const {

        size_t nth = this->size() - 1;
        if ( index > nth ) return ( d );  // Edge case where starting index exceeds size
        size_t n = ( nvals == 0 ) ? nth - index + 1 : nvals;

        ConversionParameters p( index, n, m_traits );
        OptionVisitor visitor( d, this->default_value(), p );
        m_option.visit( visitor );
        return ( d );
      }

      inline std::vector<int> get_all( const size_t index = 0,
                                          const size_t nvals = 0 ) const {
        std::vector<int> d;
        return ( get_all( d, index,  nvals) );
      }

      /**
       * @brief Compare two products with differing defaults
       * 
       * This function computes the difference of two integer options where
       * differing default values detect extended arrays resulting in a failing
       * comparison. 
       * 
       * Options that do not have all the same number of values will fail by 
       * definition.
       * 
       * @param option1   First option to convert to integers for comparison
       * @param option2   Second option to convert to integers for comparison
       * @param default1  Default for first option for failures and missing data
       * @param default2  Default for second option for failures and missing data
       * @param parms     Optional access parameters and traits for comparisons
       *                    Default is to extract all values from each option
       * @return true     If all corresponding values compare 
       * @return false    If option sizes differ or are not within tolerance
       */
      static inline bool compare( const ProductOption &option1, 
                                  const ProductOption &option2,
                                  const int default1,
                                  const int default2,
                                  const ConversionParameters &parms = ConversionParameters::get_all_values( )
                                )  {                                    

        OptionInts option1_s( option1, parms.traits(), default1 );
        OptionInts option2_s( option2, parms.traits(), default2 );
        std::vector<int> opt1_v, opt2_v;
        
        opt1_v.reserve( option1_s.size() );
        opt2_v.reserve( option2_s.size() );

        option1_s.get_all( opt1_v );
        option2_s.get_all( opt2_v );

        if ( opt1_v.size() != opt2_v.size() ) {
          return ( false );
        }

        size_t n = std::min( opt1_v.size(), opt2_v.size() );
        for ( size_t i = 0  ; i < n ; i++ ) {
          if ( opt1_v[i] != opt2_v[i] ) {
            return ( false );
          }
        }
        return ( true );
      }

      /**
       * @brief Compare integer conversions of two products with same parameters
       * 
       * This comparison is convenient for options that contain the same number
       * of values, since the same default is used. 
       * 
       * @param option1 
       * @param option2 
       * @param default_v 
       * @param traits 
       * @return true 
       * @return false 
       */
      static inline bool compare( const ProductOption &option1, 
                                  const ProductOption &option2,
                                  const int default_v = OptionIntsDefault ,
                                  const ConversionParameters &parms = ConversionParameters::get_all_values( )
                                  )  {
        return ( OptionInts::compare( option1, option2, default_v, default_v, parms ) );                                    
      }

    protected:
      
      class OptionVisitor { 
        public:
          OptionVisitor( std::vector<int> &data,
                         const int &default_v,
                         const ConversionParameters parms = ConversionParameters() ) : 
                         m_datum( data ),m_default( default_v),
                         m_parameters( parms ) { }
          OptionVisitor( std::vector<int> &data,
                         const ConversionParameters parms = ConversionParameters() ) : 
                         m_datum( data ),m_default( OptionIntsDefault  ),
                         m_parameters( parms ) { }                        
          virtual ~OptionVisitor() = default;

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
                if ( i >= OptionIntsDefault ) {
                    m_datum.push_back( m_default);
                } else {
                    m_datum.push_back( static_cast<int>( i ) );
                }
            }
            else {
              m_datum.push_back ( default_value() );
            }    
          }          

          inline void operator()( const double d ) {
            if ( add_it( 0, 1 ) ) { 
            // May need to revisit - floor/ceil 
            // static will truncate, but need to evaluate what to do 
            // with values > int max 
                if (d >= OptionIntsDefault ) {
                    m_datum.push_back( m_default );
                }
                else if (d <= std::numeric_limits<int>::min() ) {
                    m_datum.push_back( std::numeric_limits<int>::min() );
                } 
                else {
                    m_datum.push_back( static_cast<int>( d ) );
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
                if ( i_array[i] >= std::numeric_limits<int>::min() ) {
                    m_datum.push_back( m_default );
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
                if (d_array[i] >= std::numeric_limits<int>::min() ) {
                    m_datum.push_back( m_default );
                }
                else if (d_array[i] <= std::numeric_limits<int>::min() ) {
                    m_datum.push_back( std::numeric_limits<int>::min() );
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
            m_datum.push_back( static_cast<int>( value ) );
            return;
          }


          inline const int &default_value() const {
            return ( m_default );
          }

        private:
          /** Reference to vector that collects doubles */
          std::vector<int>     &m_datum;
          ConversionParameters m_parameters;
          int                  m_default; 
        
          /** Convert strings to ints with error checking */
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

          /** Return conversion parameters */
          inline const ConversionParameters &parameters( ) const {
            return  ( m_parameters );
          }
        
          /** Determine if the index is valid given traits and array size */
          inline bool add_it(const size_t index, const size_t max_size ) const {
            if ( parameters().all() && 
               ( index >= parameters().index() ) &&
               ( index < max_size )  ) {
              return ( true );
            }
            else if ( ( index == parameters().index() ) &&
                     ( index < max_size ) ) {
              return ( true );
            }
            return ( false );
          }
      };


      private:
        ProductOption    m_option;
        ConversionTraits m_traits;
        int              m_default;
  };  

}    // namespace psmrts::algoriths::conversions