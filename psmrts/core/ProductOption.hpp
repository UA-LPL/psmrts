#pragma once

#include <string>
#include <vector>
#include <variant>
#include <tuple>
#include <initializer_list>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsJson.hpp>

namespace psmrts {

  // Overload helper type for the ProductOption visitor. See the size()
  // method for how this can be used.
  template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
  template<typename...Func> overload(Func...) -> overload<Func...>;

  /**
   * @brief Manage configuration keywords with limited data type support
   * 
   * The keys are required to be lower case. This is enforced in the
   * get/add methods. A series of configuration methods are provided
   * as static methods to be used for formatting needs PSMRTS-wide.
   *
   * @author 2025-07-04 Kris J. Becker, UA Original Version
   */
  class ProductOption {
    public:
      /*** Default is micrometer precision */
      inline static const size_t DigitsPrecision = 9;
      inline static const double DoubleTolerance = 1.0e-9;

      using DataTypes = std::variant< bool,
                                      int, 
                                      size_t, 
                                      double,
                                      std::string, 
                                      std::vector<int>,
                                      std::vector<size_t>,
                                      std::vector<double>,
                                      std::vector<std::string>,
                                      ordered_json >;
      using DataEnums = enum {
                              PsmrtsBoolean,
                              PsmrtsInteger,
                              PsmrtsSizeT,
                              PsmrtsDouble,
                              PsmrtsString,
                              PsmrtsIntegerArray,
                              PsmrtsSizeTArray,
                              PsmrtsDoubleArray,
                              PsmrtsStringArray,
                              PsmrtsJsonObject
                            };

      ProductOption() : m_name( "false" ), m_data( false ), m_enum( PsmrtsBoolean ) { }
      explicit ProductOption( const std::string &name, const bool b_data ) : 
                              m_name( psmrts_tolower(name)), 
                              m_data( b_data ), 
                              m_enum( PsmrtsBoolean ) { }
      explicit ProductOption( const std::string &name, const int i_data ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( i_data ), 
                              m_enum( PsmrtsInteger ) { }
      explicit ProductOption( const std::string &name, const size_t i_data_t ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( i_data_t ), 
                              m_enum( PsmrtsSizeT ) { }                              
      explicit ProductOption( const std::string &name, const double d_data ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( d_data ), 
                              m_enum( PsmrtsDouble ) { }
      explicit ProductOption( const std::string &name, const char *s_text ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( std::string(s_text) ), 
                              m_enum( PsmrtsString ) { }                                  
      explicit ProductOption( const std::string &name, const std::string &s_data ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( s_data ), 
                              m_enum( PsmrtsString ) { }
      explicit ProductOption( const std::string &name, const std::initializer_list<int> &i_array ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( std::vector<int> (i_array.begin(), i_array.end())), 
                              m_enum( PsmrtsIntegerArray ) { }                                
      explicit ProductOption( const std::string &name, const std::vector<int> &i_array ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( std::vector<int> (i_array)),
                              m_enum( PsmrtsIntegerArray ) { }
      explicit ProductOption( const std::string &name, const std::initializer_list<size_t> &i_array ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( std::vector<size_t> (i_array.begin(), i_array.end())), 
                              m_enum( PsmrtsSizeTArray ) { }                                
      explicit ProductOption( const std::string &name, const std::vector<size_t> &i_array ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( std::vector<size_t> (i_array)),
                              m_enum( PsmrtsSizeTArray ) { }                              
      explicit ProductOption( const std::string &name, const std::initializer_list<double> &d_array ) : 
                              m_name( psmrts_tolower(name) ),
                              m_data( std::vector<double> (d_array.begin(), d_array.end()) ),
                              m_enum( PsmrtsDoubleArray ) { }                                
      explicit ProductOption( const std::string &name, const std::vector<double> &d_array ) : 
                              m_name( psmrts_tolower(name) ),
                              m_data( std::vector<double> (d_array) ),
                              m_enum( PsmrtsDoubleArray ) { }
      explicit ProductOption( const std::string &name, const std::initializer_list<std::string> &s_array ) : 
                              m_name( psmrts_tolower(name) ),
                              m_data( std::vector<std::string> (s_array.begin(), s_array.end()) ),
                              m_enum( PsmrtsStringArray ) { }                                
      explicit ProductOption( const std::string &name, const std::vector<std::string> &s_array ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( std::vector<std::string> (s_array) ),
                              m_enum( PsmrtsStringArray ) { }
      explicit ProductOption( const std::string &name, const Eigen::Vector3d &d_v ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( std::vector<double> (d_v.data(), d_v.data()+3) ), 
                              m_enum( PsmrtsDoubleArray ) { }
      explicit ProductOption( const std::string &name, const Eigen::Vector3i &i_v ) : 
                              m_name( psmrts_tolower(name) ), 
                              m_data( std::vector<int> (i_v.data(), i_v.data()+3) ), 
                              m_enum( PsmrtsIntegerArray ) { }                                                                
      explicit ProductOption( const std::string &name, const ordered_json &j_data ) : 
                              m_name(psmrts_tolower(name) ),
                              m_data( j_data ),
                              m_enum( PsmrtsJsonObject ) {
      }
      virtual ~ProductOption() { }


      /** Returns the name of the option */
      inline const std::string &name() const {
        return ( m_name );
      }

      /** This is good for a UID but is a string */
      inline const std::string &uid() const {
        return ( m_name );
      }

      /** Returns the enumerated type as stored in the variant */
      inline DataEnums type() const {
        return ( m_enum );
      }

      /** 
       * @brief veturns size of the data -  1 for scalars, DataTypes::size() otherwise
       * 
       * This method returns the sise of the data element contained within this instance
       * of the class. All interisic types report asize of 1, where as all other vectors
       * return the size of the actual values contained in the data.
       * 
       * The overload{  } construct allows you to seclect the correct type stored in variant
       * and reprot the actual size of the data.
       */
      inline size_t size() const {
        // Run an overloaded visitor with default behavior. This overload
        // requires a lambda that successfully accepts each variant
        // type and takes appropriate action. This particular overload will
        // simply return the appropriate number of elements/type. Scalar
        // integral types will always return 1, executed by the std:visit()
        // below. 
        const auto visitor = overload{            
                  [](const std::string &s) { return ( s.size() ); },            
                  [](const std::vector<int> &i_array) { return (i_array.size() ); },
                  [](const std::vector<size_t> &i_t_array) { return (i_t_array.size() ); },
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
       * methods for each of the types contained in the PsmrtsOption data
       * set. This is required - to provide an operator for every type in
       * the ProductOption::DataTypes variant. Unless you use the overload()
       * method as shown above.
       * 
       * See the DoubleVisitor, IntegerVisitor and StringVisitor visitor
       * functors below.
       * 
       * @tparam T       A visitor functor or overload lambda function set 
       *                   to PsmrtsParametet
       * @param visitor One of DoubleVisitor, IntegerVisitor, StringVisitor
       *                  or a compatible functor object/overload lambdas
       */
      template <typename T>
        inline void get_to( T &visitor ) const {
          std::visit( visitor, m_data );
        }

      /** Convert the value to a string using JSON rules */

      /**
       * @brief Returns the string representation of the content of the product.
       * 
       * Each variably type may have particulare specifics involve in performing a string
       * conversion. This overload{} construct ovecomes any particlars using specialization
       * of ach type.
       * 
       * @return std::string 
       */
      inline std::string to_string() const {

        // This overload structure handles conversions of each variant
        // type by calling the appropriate lambda method and converting
        // the stored variant variable type to a string, executed by the
        // std::visit() below.
        const auto visitor = overload {
#if defined(WIN32) || defined(_MSC_VER) || defined(__CYGWIN__)
#pragma warning ( push )
#pragma warning ( disable : 4573 )
#endif
                  // Handle intrisics
                  [](const bool &b) { return ( std::string( ( b ? "true" : "false" ) ) ); },            
                  [](const int &i ) { return ( std::to_string( i ) ); },            
                  [](const size_t &t ) { return ( std::to_string( t ) ); },            
                  [](const double &d ) { return ( ProductOption::to_string( d ) ); },                  
                  [](const std::string &s) { return ( s ); },   

                  // Vector types shoudl look similar to JSON here.
                  [](const std::vector<int> &i_array) { return ( to_string( i_array ) ); },
                  [](const std::vector<size_t> &i_t_array) { return ( to_string( i_t_array ) ); },
                  [](const std::vector<double> &d_array) { return (  to_string( d_array ) ); },
                  [](const std::vector<std::string> &s_array) { return ( to_string( s_array ) ); },

                  [](const ordered_json &j) { return ( j.dump() );  }
#if defined(WIN32) || defined(_MSC_VER) || defined(__CYGWIN__)
#pragma warning ( pop )
#endif                  
              };
         
         return ( std::visit(visitor, m_data ) );
      }

      /* Convert the keyword and value to a JSON object */
      inline ordered_json to_json() const {
        return ( std::visit( [&] ( auto &&datum ) -> ordered_json {
          ordered_json json_t;
          json_t[this->name()] = datum;
          return ( json_t );
        }, m_data ) );
      }


      // Static API to use for consistent covnversions. See visitor
      // functors below.

      /** Convert a boolean data type to a "true" or "false" string */
      inline static std::string to_string( const bool b_data ) {
         return( ( b_data ? "true" : "false" ) );
      }

      /** Convert integer data to a string using std::to_string() */
      inline static std::string to_string( const int i_data ) {
        return ( std::to_string( i_data ) );
      }

      /** Convert size_t data to a string using std::to_string() */
      inline static std::string to_string( const size_t i_data ) {
        return ( std::to_string( i_data ) );
      } 
           
      /** Convert double data to a string with fixed digit representation */
      inline static std::string to_string( const double d_data, 
                                           const size_t ndigits = ProductOption::DigitsPrecision )  {

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
          s_array += ( psmrts_concate( comma, ProductOption::to_string( i ) ) );
          comma = ",";
        }
        return ( psmrts_concate( s_array, std::get<1>( enclosures ) ) );        
      } 

      /** Convert a integer vector to string */
      inline static std::string to_string( const std::vector<size_t> i_array,
                                           const std::tuple<std::string,std::string> &enclosures= { "[", "]" } ) {

        std::string s_array = std::get<0>( enclosures );

        std::string comma = "";
        for ( const auto i : i_array ) {
          s_array += ( psmrts_concate( comma, ProductOption::to_string( i ) ) );
          comma = ",";
        }
        return ( psmrts_concate( s_array, std::get<1>( enclosures ) ) );        
      }      
      
      /** Convert double array to string array with optional array enclousures and precision */
      inline static std::string to_string( const std::vector<double> d_array,
                                           const std::tuple<std::string,std::string> &enclosures= { "[", "]" }, 
                                           const size_t ndigits = DigitsPrecision ) {

        std::string s_array =  std::get<0>( enclosures );

        std::string comma = "";
        for ( const auto d : d_array ) {
          s_array += ( psmrts_concate( comma, ProductOption::to_string( d, ndigits ) ) );
          comma = ",";
        }
        return ( psmrts_concate( s_array,  std::get<1>( enclosures ) ) );        
      } 

      /** Convert string array to string form with optional array enclosures */
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

      /** Return a JSON object formatted using JSON dump() */
      inline static std::string to_string( const ordered_json &j_data ) {
        return ( j_data.dump() );        
      } 

      /** Determine if two products contain the same data */
      inline bool equals( const ProductOption &opt ) const {
        if ( this->name() != opt.name() ) return ( false );
        if ( this->type() != opt.type() ) return ( false );
        if ( this->to_string() != opt.to_string() ) return ( false );
        
        // All good.
        return ( true );
      }

    private:
      std::string m_name;
      DataTypes   m_data;
      DataEnums   m_enum;
  };      

  

  /**
   * @brief DoubleVisitor is a class that extracts double values from a ProductOption
   * 
   * This functor object will extract, converting if necessary, any of the
   * stored intrinsic types. This must be maintained alongside any changes
   * made to ProductOption, paticularly when any new types are added or removed.
   * 
   * @author 2025-08-21 Kris J Becker
   */
  class DoubleVisitor {
    public:
      DoubleVisitor() : m_name("double"), m_doubles{}, m_default( psmrts::null()  ) { }
      DoubleVisitor(const double default_value ) : 
                     m_name("double"), 
                     m_doubles{},
                     m_default( default_value ) { }
      DoubleVisitor(const std::string &name, const double default_value = psmrts::null() ) : 
                     m_name(name), m_doubles{}, m_default( default_value ) { }
      DoubleVisitor( const ProductOption &parameter,
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

      inline void operator()( const size_t i )  {
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

      inline void operator()( const std::vector<size_t> &i_array ) {
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

      inline ProductOption::DataEnums type() const {
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
      ProductOption::DataEnums m_type;
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

  
  /**
   * @brief IntegerVisitor is a class that extracts integers from ProductOption
   * 
   * This functor object will extract, converting if necessary, any of the
   * stored intrinsic types. This must be maintained alongside any changes
   * made to ProductOption, paticular any new types added or removed.
   * 
   * @author 2025-08-21 Kris J Becker
   */
  class IntegerVisitor {
    public:
      IntegerVisitor() : m_name("integer"), m_integers{}, m_default( 0 ) { }
      IntegerVisitor(const int default_value ) : 
                     m_name("integer"), 
                     m_integers{},
                     m_default( default_value ) { }
      IntegerVisitor(const std::string &name, const int default_value = 0 ) : 
                     m_name(name), m_integers{}, m_default( default_value ) { }
      IntegerVisitor( const ProductOption &parameter,
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

      inline void operator()( const size_t i )  {
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

      inline void operator()( const std::vector<size_t> i_array ) {
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

      inline ProductOption::DataEnums type() const {
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
      ProductOption::DataEnums m_type;
      std::vector<int>           m_integers;
      int                        m_default;
  };  

  /**
   * @brief StringVisitor is a class that extracts strings from ProductOption
   * 
   * This functor object will extract, converting if necessary, any of the
   * stored intrinsic types. This must be maintained alongside any changes
   * made to ProductOption, paticular any new types added or removed.
   * 
   * @author 2025-08-21 Kris J Becker
   */
  class StringVisitor {
    public:
      inline static const size_t DigitsPrecision = ProductOption::DigitsPrecision;

      StringVisitor() : m_name("string"), m_strings{}, m_default( "" ) { }
      StringVisitor(const std::string &name, const std::string default_value = "" ) : 
                     m_name(name), m_strings{}, m_default( default_value ) { }
      StringVisitor( const ProductOption &parameter,
                     const std::string default_value = "" ) : 
                     m_name( parameter.name() ), 
                     m_strings{}, 
                     m_default( default_value ) {
        m_type = parameter.type();
        parameter.get_to( *this );
      }
      
      virtual ~StringVisitor() { }

      inline void operator()(const bool b ) {
        m_strings.push_back( ProductOption::to_string( b) );
      }

      inline void operator()( const int i )  {
        m_strings.push_back( ProductOption::to_string( i ) );
      }

      inline void operator()( const size_t i )  {
        m_strings.push_back( ProductOption::to_string( i ) );
      }          

      inline void operator()( const double d ) {
        m_strings.push_back( ProductOption::to_string( d ) );
      }

      inline void operator()( const std::string &s ) {
        m_strings.push_back( s );
      }    

      inline void operator()( const std::vector<int> i_array ) {
        for (const auto &i_value : i_array ) {
          m_strings.push_back( ProductOption::to_string( i_value ) );
        }
      }

      inline void operator()( const std::vector<size_t> i_array ) {
        for (const auto &i_value : i_array ) {
          m_strings.push_back( ProductOption::to_string( i_value ) );
        }
      }

      inline void operator()( const std::vector<double> &d_array ) {
        for ( auto const &d_value : d_array ) {
          m_strings.push_back( ProductOption::to_string( d_value ) );
        }
      }
      
      inline void operator()( const std::vector<std::string> &s_array ) {
        m_strings.insert( m_strings.end(), s_array.begin(), s_array.end() );
      }      
      
      inline void operator()( const ordered_json &j_data ) {
        m_strings.push_back( j_data.dump() );
      }
      

      inline const std::string name() const {
        return ( m_name);
      }

      inline ProductOption::DataEnums type() const {
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
      ProductOption::DataEnums   m_type;
      std::vector<std::string>   m_strings;
      std::string                m_default;
  };  


} // namespace psmrts
