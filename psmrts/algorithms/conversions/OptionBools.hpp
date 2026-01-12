#ifndef OptionBools_hpp
#define OptionBools_hpp

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>

namespace psmrts::algorithms::conversions {

/**
 * @brief Option bool conversion extracts bools from ProductOptions
 * 
 * This functor object will extract, converting if necessary, ay of the 
 * stored intrinsic types. This must be maintained alongside any changes
 * made to ProductOption, particularly any new types added or removed.
 * 
 */

 class OptionBools {
    public:
      static inline bool OptionBoolsDefault = false;
     OptionBools() : m_option(), m_default(OptionBoolsDefault) {}
     OptionBools( const ProductOption &option ) {
        m_option = option;
        m_traits = ConversionTraits();
        m_default = OptionBoolsDefault;
     }
     OptionBools( const ProductOption &option,
                  const ConversionTraits &traits,
                  const bool default_v = OptionBoolsDefault ) {
        m_option = option;
        m_traits = traits;
        m_default = default_v;
     }
     OptionBools( const ProductOption &option,
                  const bool default_v ) {
          m_option = option;
          m_traits = ConversionTraits();
          m_default = default_v;
     }

     virtual ~OptionBools() = default;

     inline size_t size() const {
        return ( m_option.size() );
     }
     
     inline ProductOption::DataEnums type() const {
        return ( m_option.type() );
     }

     inline const bool &default_value() const {
        return ( m_default );
     }

     inline std::string name() const {
        return ( m_option.name() );
     }
    
     inline bool get( const size_t index = 0 ) const {
        ConversionParameters p( index, 1, m_traits );
        std::vector<bool> one;
        one.reserve( 1 );

        OptionVisitor visitor( one,this->default_value(), p );
        m_option.visit( visitor );

        return ( one.front() );
     }

      inline const std::vector<bool> &get_all( std::vector<bool> &d,
                                                 const size_t index = 0,
                                                 const size_t nvals = 0 ) const {

        size_t nth = ( this->size() == 0 ) ? 0 : this->size() - 1;
        if ( index > nth ) return ( d );  // Edge case where starting index exceeds size
        size_t n = ( nvals == 0 ) ? nth - index + 1 : nvals;

        ConversionParameters p( index, n, m_traits );
        OptionVisitor visitor( d, this->default_value(), p );
        m_option.visit( visitor );
        return ( d );
      }

      inline std::vector<bool> get_all( const size_t index = 0,
                                        const size_t nvals = 0 ) const {
        std::vector<bool> d;
        return ( get_all( d, index,  nvals) );
      }

      /**
       * @brief Compare two products with differing defaults
       * 
       * This function computes the difference of two bool options where
       * differing default values detect extended arrays resulting in a failing
       * comparison. 
       * 
       * Options that do not have all the same number of values will fail by 
       * definition.
       * 
       * @param option1   First option to convert to bools for comparison
       * @param option2   Second option to convert to bools for comparison
       * @param default1  Default for first option for failures and missing data
       * @param default2  Default for second option for failures and missing data
       * @param parms     Optional access parameters and traits for comparisons
       *                    Default is to extract all values from each option
       * @return true     If all corresponding values compare within tolerance
       * @return false    If option sizes differ or are not within tolerance
       */
      static inline bool compare( const ProductOption &option1, 
                                  const ProductOption &option2,
                                  const double default1,
                                  const double default2,
                                  const ConversionParameters &parms = ConversionParameters::get_all_values( )
                                )  {                                    

        OptionBools option1_s( option1, parms.traits(), default1 );
        OptionBools option2_s( option2, parms.traits(), default2 );
        std::vector<bool> opt1_v, opt2_v;
        
        opt1_v.reserve( option1_s.size() );
        opt2_v.reserve( option2_s.size() );

        option1_s.get_all( opt1_v );
        option2_s.get_all( opt2_v );

        if ( opt1_v.size() != opt2_v.size() ) {
          return ( false );
        }

        size_t n = std::min( opt1_v.size(), opt2_v.size() );
        for ( size_t i = 0  ; i < n ; i++ ) {
          if ( opt1_v[i] != opt2_v[i] ) return ( false );
        }
        return ( true );
      }

          /**
       * @brief Compare bools conversions of two products with same parameters
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
                                  const double default_v = OptionBoolsDefault,
                                  const ConversionParameters &parms = ConversionParameters::get_all_values( )
                                  )  {
        return ( OptionBools::compare( option1, option2, default_v, default_v, parms ) );                                    
      }


    protected: 
     class OptionVisitor { 
        public:
          OptionVisitor( std::vector<bool> &data,
                         const bool &default_v,
                         const ConversionParameters parms = ConversionParameters() ) : 
                         m_datum( data ),m_default( default_v ),
                         m_parameters( parms ) { }
          OptionVisitor( std::vector<bool> &data,
                         const ConversionParameters parms = ConversionParameters() ) : 
                         m_datum( data ),m_default( OptionBoolsDefault ),
                         m_parameters( parms ) { }                        
          virtual ~OptionVisitor() = default;

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

          inline const bool &default_value() const {
            return ( m_default );
          }

        private:
          /** Reference to vector that collects strings */
          std::vector<bool>        &m_datum;
          ConversionParameters     m_parameters;
          bool                     m_default; 
          
          inline const ConversionParameters &parameters( ) const {
            return  ( m_parameters );
          }
        
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

    private:
        ProductOption    m_option;
        ConversionTraits m_traits;
        bool             m_default;
 };

} // namespace psrmts::algorithms::conversions

#endif
