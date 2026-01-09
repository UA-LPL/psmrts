#pragma once

#include <string>
#include <vector>
#include <variant>
#include <tuple>
#include <initializer_list>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/algorithms/conversions/AllConversionsVisitors.hpp>

namespace psmrts {

  // Overload helper type for the ProductOption visitor. See the size()
  // method for how this can be used.
  template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
  template<typename...Func> overload(Func...) -> overload<Func...>;

  /**
   * @brief Data container for PSMRTS product options and features
   * 
   * This data container is used to represent data in PSMRTS products that
   * is used to describe products. Product configurations and feature
   * specifications can be compared for reuse purposes. The product names are 
   * required to be lower case. This is enforced in the get/add methods.
   * 
   * This class is implemented as a std::variant container so mainipulations use
   * visit/visitor strategies implemented in the conversion algorithms. See the
   * classes in the pmsrts/algiorithms/conversions for details.
   *
   * @author 2025-07-04 Kris J. Becker, UA Original Version
   */
  class ProductOption {
    public:
      using DoublesVisitor = psmrts::algorithms::conversions::DoublesVisitor;
      using StringsVisitor = psmrts::algorithms::conversions::StringsVisitor;
      // using SizetsVisitor = psmrts::algorithms::conversions::SizetsVisitor;
      // using BoolsVisitor = psmrts::algorithms::conversions::BoolsVisitor;
      // using IntsVisitor = psmrts::algorithms::conversions::IntsVisitor;
      // using JsonVisitor = psmrts::algorithms::conversions::JsonVisitor;
      
      using DoublesExtractor = psmrts::algorithms::conversions::Extractor<ProductOption, DoublesVisitor>;
      using StringsExtractor = psmrts::algorithms::conversions::Extractor<ProductOption, StringsVisitor>;
      // using SizetsExtractor  = psmrts::algorithms::conversions::Extractor<ProductOption, SizetsVisitor>;
      // using IntsExtractor    = psmrts::algorithms::conversions::Extractor<ProductOption, IntsVisitor>;
      // using BoolsExtractor   = psmrts::algorithms::conversions::Extractor<ProductOption, BoolsVisitor>;
      // using JsonExtractor    =  psmrts::algorithms::conversions::Extractor<ProductOption, JsonVisitor>;
      
      using StringsComparator = psmrts::algorithms::conversions::Comparator<ProductOption, StringsVisitor>;

      
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
       * @brief Returns size of the data -  1 for scalars, DataTypes::size() otherwise
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
       * See classes in psmrts/algorithms/conversions.
       * 
       * @tparam T       A visitor functor or overload lambda function set 
       *                   to PsmrtsParameter
       * @param visitor One of OptionStrings, OptionDoubles, etc...,
       *                  or a compatible functor object/overload lambdas
       */
      template <typename T>
        inline void visit( T &visitor ) const {
          std::visit( visitor, m_data );
        }

      /* Convert the keyword and value to a JSON object */
      inline ordered_json to_json() const {
        return ( std::visit( [&] ( auto &&datum ) -> ordered_json {
          ordered_json json_t;
          json_t[this->name()] = datum;
          return ( json_t );
        }, m_data ) );
      }

      /** Return each value as a string */
      inline std::string to_string( const size_t index = 0 ) const {
        return ( StringsExtractor( *this ).get(index) );
      }

      /** Return each value as a double */
      inline double to_double( const size_t index = 0 ) const {
        return ( DoublesExtractor( *this ).get(index) );
      }
      
      /** Compare with another product using strings conversions */
      inline bool operator==( const ProductOption &other ) const {
        // Iffy, but ensures failure unless those strings exist in string types...
        return ( StringsComparator( *this, "++" ).compare( other, "--" ) );
      }

      /** Compare with another product using strings conversions */
      inline bool operator!=( const ProductOption &other ) const {
        // Iffy, but ensures failure unless those strings exist in string types...
        return ( !StringsComparator( *this, "++" ).compare( other, "--" ) );
      }      

    private:
      std::string m_name;
      DataTypes   m_data;
      DataEnums   m_enum;
  };

} // namespace psmrts
