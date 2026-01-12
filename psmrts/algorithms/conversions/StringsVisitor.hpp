#ifndef StringsVisitor_hpp
#define StringsVisitor_hpp

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
                      const Type &default_v,
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

        auto process = [&]( const bool addit, const size_t index ) {
          if ( addit ) {
            m_datum.push_back( ( b ? "true" : "false" ) );
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
            m_datum.push_back( std::to_string( i ) );
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
            m_datum.push_back( std::to_string( i ) );
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
            std::ostringstream out;
            out << std::fixed << std::setprecision( parameters().traits().digits() ) << d;               
            m_datum.push_back( out.str() );
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
            m_datum.push_back( s );
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
            m_datum.push_back( std::to_string( i_array[index] ) ); 
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
            m_datum.push_back( std::to_string( i_array[index] ) ); 
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
            std::ostringstream out;
            out << std::fixed << std::setprecision( parameters().traits().digits() ) << d_array[index];               
            m_datum.push_back( out.str() );  
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
            m_datum.push_back( s_array[index] ); 
          }
          else {
            m_datum.push_back ( default_value() );
          }
        };
        parameters().extractor( s_array.size(), process ); 
      }      
      
      inline void operator()( const ordered_json &j_data ) {

       auto process = [&]( const bool addit, const size_t index ) {
         if ( addit ) {
           if ( j_data.is_primitive() ) {
             Type str_t = j_data;
             m_datum.push_back( str_t );
           }
           else {        
              m_datum.push_back( j_data.dump( parameters().traits().spaces() ) );
           }
         }
         else {
           m_datum.push_back( default_value() );
         }              
        };

        parameters().extractor( 1, process );
      }

      inline const Type &default_value() const {
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
      TypeVector           &m_datum;
      ConversionParameters  m_parameters;
      Type                  m_default; 
  };


}    // namespace psmrts::algoriths::conversions

#endif
