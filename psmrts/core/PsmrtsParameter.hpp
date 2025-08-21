#pragma once

#include <type_traits>
#include <iterator>
#include <string>
#include <vector>
#include <fstream>
#include <variant>
#include <tuple>
#include <initializer_list>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsJson.hpp>

namespace psmrts {

// Overload helper type for the PsmrtsParameter visitor. See the size()
// method for how this can be used.
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<typename...Func> overload(Func...) -> overload<Func...>;

  /**
   * @brief Manage configuration keywords with limited data type support
   * 
   * The JSON keys are required to be lower case. This is enforced in the
   * get/add methods. 
   *
   * @author 2024-07-04 Kris J. Becker, UA Original Version
   */
  class PsmrtsParameter {
    public:
      /*** Default is micrometer precision */
      inline static const size_t DigitsPrecision = 9;
      inline static const double DoubleTolerance = 1.0e-9;
      using DataTypes = std::variant< bool,
                                      int, 
                                      double,
                                      std::string, 
                                      std::vector<int>,
                                      std::vector<double>,
                                      std::vector<std::string>,
                                      ordered_json >;
      using DataEnums = enum {
                              PsmrtsBoolean,
                              PsmrtsInteger,
                              PsmrtsDouble,
                              PsmrtsString,
                              PsmrtsIntegerArray,
                              PsmrtsDoubleArray,
                              PsmrtsStringArray,
                              PsmrtsJsonObject
                            };

      PsmrtsParameter() : m_name( "false" ), m_data( false ), m_enum( PsmrtsBoolean ) { }
      explicit PsmrtsParameter( const std::string &name, const bool b_data ) : 
                                m_name( psmrts_tolower(name)), m_data( b_data ), m_enum( PsmrtsBoolean ) { }
      explicit PsmrtsParameter( const std::string &name, const int i_data ) : 
                                m_name( psmrts_tolower(name) ), m_data( i_data ), m_enum( PsmrtsInteger ) { }
      explicit PsmrtsParameter( const std::string &name, const double d_data ) : 
                                m_name( psmrts_tolower(name) ), m_data( d_data ), m_enum( PsmrtsDouble ) { }
      explicit PsmrtsParameter( const std::string &name, const char *s_text ) : 
                                m_name( psmrts_tolower(name) ), m_data( std::string(s_text) ), m_enum( PsmrtsString ) { }                                  
      explicit PsmrtsParameter( const std::string &name, const std::string &s_data ) : 
                                m_name( psmrts_tolower(name) ), m_data( s_data ), m_enum( PsmrtsString ) { }
      explicit PsmrtsParameter( const std::string &name, const std::initializer_list<int> &i_array ) : 
                                m_name( psmrts_tolower(name) ), m_data( std::vector<int> (i_array.begin(), i_array.end())), m_enum( PsmrtsIntegerArray ) { }                                
      explicit PsmrtsParameter( const std::string &name, const std::vector<int> &i_array ) : 
                                m_name( psmrts_tolower(name) ), m_data( std::vector<int> (i_array)), m_enum( PsmrtsIntegerArray ) { }
      explicit PsmrtsParameter( const std::string &name, const std::initializer_list<double> &d_array ) : 
                                m_name( psmrts_tolower(name) ), m_data( std::vector<double> (d_array.begin(), d_array.end()) ), m_enum( PsmrtsDoubleArray ) { }                                
      explicit PsmrtsParameter( const std::string &name, const std::vector<double> &d_array ) : 
                                m_name( psmrts_tolower(name) ), m_data( std::vector<double> (d_array) ), m_enum( PsmrtsDoubleArray ) { }
      explicit PsmrtsParameter( const std::string &name, const std::initializer_list<std::string> &s_array ) : 
                                m_name( psmrts_tolower(name) ), m_data( std::vector<std::string> (s_array.begin(), s_array.end()) ), m_enum( PsmrtsStringArray ) { }                                
      explicit PsmrtsParameter( const std::string &name, const std::vector<std::string> &s_array ) : 
                                m_name( psmrts_tolower(name) ), m_data( std::vector<std::string> (s_array) ), m_enum( PsmrtsStringArray ) { }
      explicit PsmrtsParameter( const std::string &name, const Eigen::Vector3d &d_v ) : 
                                m_name( psmrts_tolower(name) ), 
                                m_data( std::vector<double> (d_v.data(), d_v.data()+3) ), 
                                m_enum( PsmrtsDoubleArray ) { }
      explicit PsmrtsParameter( const std::string &name, const Eigen::Vector3i &i_v ) : 
                                m_name( psmrts_tolower(name) ), 
                                m_data( std::vector<int> (i_v.data(), i_v.data()+3) ), 
                                m_enum( PsmrtsIntegerArray ) { }                                                                
      explicit PsmrtsParameter( const std::string &name, const ordered_json &j_data ) : 
                                m_name(psmrts_tolower(name) ), m_data( j_data ), m_enum( PsmrtsJsonObject ) {
        std::cout << "Constructing DataTypes::json." << std::endl;
      }
      virtual ~PsmrtsParameter() { }


      /** Returns the name of the parameter */
      inline const std::string &name() const {
        return ( m_name );
      }

      /** Returns the enumerated type as stored in the variant */
      inline DataEnums type() const {
        return ( m_enum );
      }

      /** Returns size of the Parameters */
      inline size_t size() const {
        // Run an overloaded visitor with default behavior
        const auto visitor = overload{
                  [](const std::string &s) { return ( s.size() ); },            
                  [](const std::vector<int> &i_array) { return (i_array.size() ); },
                  [](const std::vector<double> &d_array) { return (d_array.size() ); },
                  [](const std::vector<std::string> &s_array) { return (s_array.size() ); },
                  [](const ordered_json &j) { return ( j.size() );  },
                  [](auto &&args) { return ( size_t(1) ); } // Default lambda for all other types
              };

          return ( std::visit(visitor, m_data) );
        }

      /**
       * @brief Variant Visitor interface
       * 
       * Developers can call this with their own containers and conversions
       * methods for each of the types contained in the PsmrtsParamter data
       * set. This is required - to provide an operator for every type in
       * the PsmrtsParameter::DataTypes variant. 
       * 
       * See the DoubleVisitor, IntegerVisitor and StringVisitor visitor
       * functors below.
       * 
       * @tparam T       A visitor functor or overload lambda function set 
       *                   to PsmrtsParametet
       * @param visitor 
       */
      template <typename T>
        inline void get_to( T &visitor ) const {
          std::visit( visitor, m_data );
        }


      inline std::string to_string() const {
        return ( std::visit( [&] ( auto &&datum ) -> std::string { 
          json j = datum;
          return ( j.dump() ); 
        }, m_data ) );
      }

      /* Convert the keyword and value to JSON */
      inline ordered_json to_json() const {
        return ( std::visit( [&] ( auto &&datum ) -> ordered_json {
          ordered_json json_t;
          json_t[this->name()] = datum;
          return ( json_t );
        }, m_data ) );
      }


      // Static API to use for consistent covnversions. See visitor
      // functors below.

      inline static std::string to_string( const bool b_data ) {
         return( ( b_data ? "true" : "false" ) );
      }

      inline static std::string to_string( const int i_data ) {
        return ( std::to_string( i_data ) );
      }

      inline static std::string to_string( const double d_data, 
                                           const size_t ndigits = PsmrtsParameter::DigitsPrecision )  {

        if ( isnull( d_data ) ) return ( "null" );

        // For all other cases
        std::ostringstream out;
        out << std::fixed << std::setprecision(ndigits) << d_data;
        return ( out.str() );        
      }
      
      /** Convert a integer vector to string */
      inline static std::string to_string( const std::vector<int> i_array,
                                           const std::tuple<std::string,std::string> &enclosures= { "[", "]" } ) {

        std::string s_array = std::get<0>( enclosures );

        std::string comma = "";
        for ( const auto i : i_array ) {
          s_array += ( psmrts_concate( comma, PsmrtsParameter::to_string( i ) ) );
          comma = ",";
        }
        return ( psmrts_concate( s_array, std::get<1>( enclosures ) ) );        
      } 
      
      inline static std::string to_string( const std::vector<double> d_array,
                                           const std::tuple<std::string,std::string> &enclosures= { "[", "]" }, 
                                           const size_t ndigits = DigitsPrecision ) {

        std::string s_array =  std::get<0>( enclosures );

        std::string comma = "";
        for ( const auto d : d_array ) {
          s_array += ( psmrts_concate( comma, PsmrtsParameter::to_string( d, ndigits ) ) );
          comma = ",";
        }
        return ( psmrts_concate( s_array,  std::get<1>( enclosures ) ) );        
      } 

      inline static std::string to_string( const std::vector<std::string> s_array,
                                           const std::tuple<std::string,std::string> &enclosures= { "[", "]" } ) {

        std::string s_out = std::get<0>( enclosures );

        std::string comma = "";
        for ( const auto s : s_array ) {
          s_out += ( psmrts_concate( comma, "\"" + s + "\"") );
          comma = ",";
        }
        return ( psmrts_concate( s_out, std::get<1>( enclosures ) ) );        
      } 

      inline static std::string to_string( const ordered_json &j_data ) {
        return ( j_data.dump() );        
      } 
      
      /** Convert the rest of the elements to string */
      template <typename T>
        std::string to_string( const T &data ) const {
          return ( std::to_string ( data ) );
        }


    private:
      std::string m_name;
      DataTypes   m_data;
      DataEnums   m_enum;
  };      

  

/** Defines a double value visitor with common conversions */
  class DoubleVisitor {
    public:
      DoubleVisitor() : m_name("double"), m_doubles{}, m_default( psmrts::null()  ) { }
      DoubleVisitor(const double default_value ) : 
                     m_name("double"), 
                     m_doubles{},
                     m_default( default_value ) { }
      DoubleVisitor(const std::string &name, const double default_value = psmrts::null() ) : 
                     m_name(name), m_doubles{}, m_default( default_value ) { }
      DoubleVisitor( const PsmrtsParameter &parameter,
                     const double default_value = psmrts::null()) : 
                     m_name( parameter.name() ), 
                     m_doubles{}, 
                     m_default( default_value ) {
        m_type = parameter.type();
        parameter.get_to( *this );
      }                     
      virtual ~DoubleVisitor() { }


      inline void operator()( const bool b ) {
        m_doubles.push_back( ( b ? 1.0 : 0.0 ) );
      }

      inline void operator()( const int i )  {
        m_doubles.push_back( i );
      }    

      inline void operator()( const double d ) {
        m_doubles.push_back( d );
      }

      inline void operator()( const std::string &s ) {
        if ( !this->convert_and_add( s )  ) {
          // Do something!
        }
      }    

      inline void operator()( const std::vector<int> &i_array ) {
        m_doubles.insert(m_doubles.end(), i_array.begin(), i_array.end() );
      }


      inline void operator()( const std::vector<double> &d_array ) {
        m_doubles.insert(m_doubles.end(), d_array.begin(), d_array.end() );
      }
      
      inline void operator()( const std::vector<std::string> &s_array ) {
        for ( auto const &s : s_array ) { 
          if ( !this->convert_and_add( s )  ) {
            // Do something!
          }
        }
      }      

      inline void operator()( const ordered_json &j_data ) {
        return;
      }
      

      inline const std::string name() const {
        return ( m_name);
      }

      inline PsmrtsParameter::DataEnums type() const {
        return ( m_type );
      }


      inline size_t size() const {
        return ( m_doubles.size() );
      }

      inline double get( const int nth = 0 ) const {
        if ( this->size() <= nth ) return ( m_default );
        return ( m_doubles[nth] );
      }

      inline const std::vector<double> &array( ) const {
        return ( m_doubles );
      }

      inline const double *data( ) const {
        if ( this->size() == 0 ) return ( nullptr );
        return ( m_doubles.data() );
      }      

      inline void set_name( const std::string &name) {
        m_name = name;
      }

      std::string                m_name;
      PsmrtsParameter::DataEnums m_type;
      std::vector<double>        m_doubles;
      double                     m_default;

      inline bool convert_and_add( const std::string &s ) {
        try {
          double d = std::stod( s );
          m_doubles.push_back( d );
          return ( true );
        }
        catch ( ... ) {
          // noop
        }
        return ( false );
      }
  };  

  
/** Defines an integer value visitor with common conversions */
  class IntegerVisitor {
    public:
      IntegerVisitor() : m_name("integer"), m_integers{}, m_default( 0 ) { }
      IntegerVisitor(const int default_value ) : 
                     m_name("integer"), 
                     m_integers{},
                     m_default( default_value ) { }
      IntegerVisitor(const std::string &name, const int default_value = 0 ) : 
                     m_name(name), m_integers{}, m_default( default_value ) { }
      IntegerVisitor( const PsmrtsParameter &parameter,
                      const int default_value = 0 ) : 
                      m_name( parameter.name() ), 
                      m_integers{}, 
                      m_default( default_value ) {
        m_type = parameter.type();
        parameter.get_to( *this );
      }                     
      virtual ~IntegerVisitor() { }


      inline void operator()( const bool b ) {
        m_integers.push_back( ( b ? 1 : 0 ) );
      }

      inline void operator()( const int i )  {
        m_integers.push_back( i );
      }          

      inline void operator()( const double d ) {
        m_integers.push_back( d );
      }

      inline void operator()( const std::string &s ) {
      }    

      inline void operator()( const std::vector<double> d_array ) {
        m_integers.insert(m_integers.end(), d_array.begin(), d_array.end() );
      }

      inline void operator()( const std::vector<int> i_array ) {
        m_integers.insert(m_integers.end(), i_array.begin(), i_array.end() );
      }

      inline void operator()( const std::vector<std::string> &s_array ) {
        return;
      }      

      inline void operator()( const ordered_json &j_data ) {
        return;
      }
      
      
      inline const std::string name() const {
        return ( m_name);
      }

      inline PsmrtsParameter::DataEnums type() const {
        return ( m_type );
      }

      inline size_t size() const {
        return ( m_integers.size() );
      }

      inline int get( const int nth = 0 ) const {
        if ( this->size() <= nth ) return ( m_default );
        return ( m_integers[nth] );
      }

      inline const std::vector<int> &array( ) const {
        return ( m_integers );
      }

      inline const int *data( ) const {
        if ( this->size() == 0 ) return ( nullptr );
        return ( m_integers.data() );
      }      

      inline void set_name( const std::string &name) {
        m_name = name;
      }


      std::string                m_name;
      PsmrtsParameter::DataEnums m_type;
      std::vector<int>           m_integers;
      int                        m_default;
  };  

/** Defines an integer value visitor with common conversions */
  class StringVisitor {
    public:
      inline static const size_t DigitsPrecision = PsmrtsParameter::DigitsPrecision;

      StringVisitor() : m_name("string"), m_strings{}, m_default( "" ) { }
      StringVisitor(const std::string &name, const std::string default_value = "" ) : 
                     m_name(name), m_strings{}, m_default( default_value ) { }
      StringVisitor( const PsmrtsParameter &parameter,
                     const std::string default_value = "" ) : 
                     m_name( parameter.name() ), 
                     m_strings{}, 
                     m_default( default_value ) {
        m_type = parameter.type();
        parameter.get_to( *this );
      }
      
      virtual ~StringVisitor() { }

      inline void operator()(const bool b ) {
        m_strings.push_back( PsmrtsParameter::to_string( b) );
      }

      inline void operator()( const int i )  {
        m_strings.push_back( PsmrtsParameter::to_string( i ) );
      }    

      inline void operator()( const double d ) {
        m_strings.push_back( PsmrtsParameter::to_string( d ) );
      }

      inline void operator()( const std::string &s ) {
        m_strings.push_back( s );
      }    

      inline void operator()( const std::vector<int> i_array ) {
        for (const auto &i_value : i_array ) {
          m_strings.push_back( PsmrtsParameter::to_string( i_value ) );
        }
      }

      inline void operator()( const std::vector<double> &d_array ) {
        for ( auto const &d_value : d_array ) {
          m_strings.push_back( PsmrtsParameter::to_string( d_value ) );
        }
      }
      
      inline void operator()( const std::vector<std::string> &s_array ) {
        m_strings.insert( m_strings.end(), s_array.begin(), s_array.end() );
      }      
      
      inline void operator()( const ordered_json &j_data ) {
        std::cout << "Running DataTypes::json-> string" << std::endl;
        m_strings.push_back( j_data.dump() );
      }
      

      inline const std::string name() const {
        return ( m_name);
      }

      inline PsmrtsParameter::DataEnums type() const {
        return ( m_type );
      }

      inline size_t size() const {
        return ( m_strings.size() );
      }

      inline const std::string &get( const int nth = 0 ) const {
        if ( this->size() <= nth ) return ( m_default );
        return ( m_strings[nth] );
      }

      inline const std::vector<std::string> &array( ) const {
        return ( m_strings );
      }

      inline const std::string *data( ) const {
        if ( this->size() == 0 ) return ( nullptr );
        return ( m_strings.data() );
      }      

      inline void set_name( const std::string &name) {
        m_name = name;
      }

      std::string                m_name;
      PsmrtsParameter::DataEnums m_type;
      std::vector<std::string>   m_strings;
      std::string                m_default;
  };  


} // namespace psmrts
