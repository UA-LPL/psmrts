#ifndef PsmrtsParameter_hpp
#define PsmrtsParameter_hpp

#include <type_traits>
#include <iterator>
#include <string>
#include <vector>
#include <fstream>
#include <variant>
#include <initializer_list>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
namespace psmrts {

  // Some traits for determining vectors.
  template <typename C> struct is_vector : std::false_type {};    
  template <typename T,typename A> struct is_vector< std::vector<T,A> > : std::true_type {};    
  template <typename C> inline constexpr bool is_vector_v = is_vector<C>::value;

// helper type for the visitor
template<class... Ts>
struct overload : Ts... { using Ts::operator()...; };
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
      inline static const double Precision_d = 9;
      using DataTypes = std::variant<
                                      bool,
                                      int, 
                                      double,
                                      std::string, 
                                      std::vector<int>,
                                      std::vector<double>,
                                      std::vector<std::string>,
                                      ordered_json>;
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
      explicit PsmrtsParameter( const std::string &name, const std::string &s_data ) : 
                                m_name( psmrts_tolower(name) ), m_data( s_data ), m_enum( PsmrtsString ) { }  
      PsmrtsParameter( const std::string &name, const std::vector<int> &i_array ) : 
                       m_name( psmrts_tolower(name) ), m_data( i_array ), m_enum( PsmrtsIntegerArray ) { }
      PsmrtsParameter( const std::string &name, const std::vector<double> &d_array ) : 
                       m_name( psmrts_tolower(name) ), m_data( d_array ), m_enum( PsmrtsIntegerArray ) { }
      PsmrtsParameter( const std::string &name, const std::vector<std::string> &s_array ) : 
                       m_name( psmrts_tolower(name) ), m_data( s_array ), m_enum( PsmrtsStringArray ) { }
      explicit PsmrtsParameter( const std::string &name, const ordered_json &j_data ) : 
                                m_name(psmrts_tolower(name) ), m_data( j_data ), m_enum( PsmrtsJsonObject ) { }
      virtual ~PsmrtsParameter() { }


      /** Returns the name of the parameter */
      inline const std::string &name() const {
        return ( m_name );
      }

      /** Returns size of the Parameters */
      inline size_t size() const {
        const auto visitor = overload{
                  [](const std::string &s) { return ( s.size() ); },            
                  [](const std::vector<int> &i_array) { return (i_array.size() ); },
                  [](const std::vector<double> &d_array) { return (d_array.size() ); },
                  [](const std::vector<std::string> &s_array) { return (s_array.size() ); },
                  [](const ordered_json &j) { return ( j.size() );  },
                  [](auto &&args) { return ( size_t(1) );  }
              };

          return ( std::visit(visitor, m_data) );
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


    protected:
      inline std::string to_string( const int i_data ) const {
        return ( std::to_string( i_data ) );
      }

      inline std::string to_string( const double d_data, 
                                    const size_t ndigits = Precision_d ) const {

        if ( isnull( d_data ) ) return ( "null" );

        // For all other cases
        std::ostringstream out;
        out.precision(ndigits);
        out << std::fixed << d_data;
        return ( out.str() );        
      }
      
      /** Convert a integer vector to string */
      inline std::string to_string( const std::vector<int> i_array ) const {

        std::string s_array = "[";

        std::string comma = "";
        for ( const auto i : i_array ) {
          s_array += ( psmrts_concate( comma, this->to_string( i ) ) );
          comma = ",";
        }
        return ( psmrts_concate( s_array, "]" ) );        
      } 
      
      inline std::string to_string( const std::vector<double> d_array, 
                                    const size_t ndigits = Precision_d ) const {

        std::string s_array = "[";

        std::string comma = "";
        for ( const auto d : d_array ) {
          s_array += ( psmrts_concate( comma, this->to_string( d, ndigits ) ) );
          comma = ",";
        }
        return ( psmrts_concate( s_array, "]" ) );        
      } 

      inline std::string to_string( const std::vector<std::string> s_array ) const {

        std::string s_out = "[";

        std::string comma = "";
        for ( const auto s : s_array ) {
          s_out += ( psmrts_concate( comma, "\"" + s + "\"") );
          comma = ",";
        }
        return ( psmrts_concate( s_out, "]" ) );        
      } 

      inline std::string to_string( const ordered_json &j_data ) const {
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

} // namespace psmrts
#endif
