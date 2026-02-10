/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductSpecification_hpp
#define ProductSpecification_hpp

#include <string>
#include <vector>
#include <exception>
#include <optional>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/ProductFeature.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductOrder.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/AllOptionConversions.hpp>
// #include <psmrts/core/PRQProduct.hpp>


namespace psmrts { 


  /**
   * @brief Process/maintain product specification data
   * 
   * This class maintains the specifications for PMSRTS product. It contains
   * several required sections which include "info" and "features". 
   * 
   * The "info" section contains, at a minimum, the "name" of the product,
   * "type", which is the type of product described such as "shape", "tracer" or
   * "prioritytracer" and a "description" entry which provides a short
   * description of the product.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-05-19 Kris J. Becker - Original Version
   */
  class ProductSpecification {
    public:
      using ProductInfo      = PsmrtsContainer<ProductOption>;
      using ProductFeatures  = PsmrtsContainer<ProductFeature>;
      using Creator = std::function<void(const ProductConfiguration &config)>;


      ProductSpecification( ) : m_name( "" ), m_product( "" ),
                                m_info( "info" ), m_features( "features" ),
                                m_creator( std::nullopt ) { }
      ProductSpecification( const std::string &name,
                            const std::string &product = "product" ) :
                            m_name( name ), m_product( product ),
                            m_info( "info" ), m_features( "features" ),
                            m_creator( std::nullopt ) {
        m_info.add( ProductOption( "name", name) );
        m_info.add( ProductOption( "product", product) );
      }
      explicit ProductSpecification( const ProductInfo &info, 
                                     const std::initializer_list<ProductFeature> &features ) :
                                     m_name( info.find( "name" ).to_string() ),
                                     m_product( info.find( "product" ).to_string() ),
                                     m_info( "info", info.data() ),
                                     m_features( "features", features ),
                                     m_creator( std::nullopt ) { }                                
      explicit ProductSpecification( const ProductInfo &info,
                                     const std::vector<ProductFeature> &features ) : 
                                     m_name( info.find( "name" ).to_string() ),
                                     m_product( info.find( "product" ).to_string() ),                                     
                                     m_info( "info", info.data() ),
                                     m_features( "features", features ),
                                     m_creator( std::nullopt ) { }         
      virtual ~ProductSpecification() = default;


      inline std::string name() const {
        return ( m_name );
      }

      inline std::string product() const {
        return ( m_product );
      }

      inline size_t size() const {
        return ( m_features.size() );
      }

      inline const ProductInfo &info() const {
        return ( m_info );
      }

      inline const ProductFeatures &features() const {
        return ( m_features );
      }

      inline std::vector<std::string> info_keys() const {
        return ( m_info.keys() );
      }

      inline std::vector<std::string> feature_names() const {
        return ( m_features.keys() );
      }

      inline void add_info( const ProductOption &option ) {
        m_info.replace( option );
      }

      inline void add_feature( const ProductFeature &feature ) {
        m_features.replace( feature );
      }

      inline bool contains( const std::string &name ) const {
        return ( m_features.contains( name ) );
      }

      inline const ProductFeature &find( const std::string &name ) const {
        return ( m_features.find( name ) );
      }      

      inline std::vector<std::string> required() const {
        std::vector<std::string> keys;
        for ( const auto &f : this->features() ) {
          if ( f.is_required() ) keys.push_back( f.name() );
        }
        return ( keys );
      }

      inline std::vector<std::string> optional() const {
        std::vector<std::string> keys;
        for ( const auto &f : this->features() ) {
          if ( !f.is_required() ) keys.push_back( f.name() );
        }
        return ( keys );
      }

      /**
       * @brief Get feature name that contains an alias key of name
       * 
       * This method will return the name of a feature if any of the features in
       * the specification contains an alias name. If it does not exist, an
       * empty string is returned.
       * 
       * If a non-blank string is returned, find() can safely be called to
       * return a reference to the associated ProductFeature associated with the
       * alias name.
       * 
       * @param name         name of potential alias key
       * @return std::string If an alias name exits, the real name of the
       *                       feature is returned. If no alias exists, an empty
       *                       string is returned.
       */
      inline std::string get_alias_feature_name( const std::string &name ) const {
        for ( const auto &f : this->features() ) {
          if ( f.isa_alias( name ) ) return ( f.name() );
        }
        return ( "" );        
      }

      /**
       * @brief Process/compare a product configuration with a feature specification
       * 
       * This method is used to process a user specification comparing the
       * option to a feature of the specification. Product features contain a
       * set of keyword/value metadata specifying conditions that must be met by
       * the config option.
       * 
       * @param config       Product configuration related to a product
       *                      specification that will be compared/verified
       *                      against a feature of the same name/type.
       * @param translations Environment/parameter keyword/value pairs that will
       *                      replace occurances of path elements that begin
       *                      with a "$".
       * @return ProductOrder Product order "form" that contains the results of
       *                       the evalaution of option to an existing feature
       *                       specification.
       */
      inline ProductOrder process_order( const ProductConfiguration &config,
                                         const PsmrtsTranslations &translations )
                                         const {
        
        // Check for invalid configuration
        if ( config.size() == 0 ) return ( ProductOrder() );

        ProductOrder order( config.name() );
        std::vector<std::string> required_list;

        for ( const auto &option : config.options() ) {

          std::string f_name = this->get_alias_feature_name( option.name() );
          if ( this->contains( option.name() ) || this->contains( f_name ) ) {
            // std::cout << "SpecOption: " << option.name() << ", Alias: " << f_name << std::endl;
            if ( f_name == "" ) f_name = option.name();

            // Get the real name of the option
            ProductOption option_t( f_name, option );

            const ProductFeature &feature = this->find( f_name );
            if ( feature.is_required() ) required_list.push_back( f_name );

            // Process based upon the feature type
            if ( ( feature.type() == "file" ) || ( feature.type() == "directory" )) {
              process_file( option_t, feature, translations, order );
            }
            else if ( feature.type() == "double" ) {
              process_doubles( option_t, feature, order );
            }
            else if ( feature.type() == "int" ) {
              process_integers( option_t, feature, order );
            }
            else if ( feature.type() == "size_t" ) {
              process_size_t( option_t, feature, order );
            }            
            else if ( feature.type() == "bool" ) {
              process_booleans( option_t, feature, order );
            }           
            else { // treat the rest as strings
              process_strings( option_t, feature, order );
            }
          }
          else {
            // This may or may not be an error so callers must check conditions
            order.add_residual( option );
          }
        }

        // Now check to see if all required keywords are present
        for ( const auto &key_r : this->required() ) {
          if ( !this->contains( key_r, required_list ) ) {
            std::string mess = "*** ProductSpecification::process_order() - "
                              "Required feature key " + key_r  + 
                              " is not present in " + this->name() + " specification";
            order.add_error( std::runtime_error( mess ) );
          }
        }

        return ( order );
      }

      /** Return the JSON specification */
      inline ordered_json to_json() const {

        ordered_json j_info = ordered_json::array();
        for ( const auto &j_data : info().data()  ) {
          j_info.push_back( j_data.to_json() );
        }

        ordered_json j_features = ordered_json::array();
        for ( const auto &j_feature : features().data()  ) {
          j_features.push_back( j_feature.to_json() );
        } 
        
        ordered_json j = ordered_json::object();
        if ( j_info.size() > 0 ) j["info"]      = j_info;
        if ( j_info.size() > 0 ) j["features"]  = j_features;
        return ( j );
      }

      inline void add_creator( const Creator &creator ) {
        m_creator = creator;
      }


      /**
       * @brief Check for a string in a vector of strings
       * 
       * This method will search for the string "s" in the vector "v".
       * The strings must match exactly as the string comparison is case
       * sensitive.
       * 
       * @param s  String to search for in "v"
       * @param v  Vector containing a list of strings
       * @return true If "v" contains the string "s"
       * @return false If "s" is not in "v"
       */
      inline bool contains( const std::string &s,
                            const std::vector<std::string> &v ) const {
        for ( const auto &test_s : v ) {
          if ( test_s == s ) return ( true );
        }
        return ( false );
      }

        /**
         * @brief Process a file FeatureOption config option
         * 
         * This method compares a product configuration option with a product
         * specification related to a file. It will extract and compare the file
         * extension, translate any environment/paramterization variables and
         * add to the product order the results. 
         * 
         * Errors encountered are recorded in the product order object and
         * indicates a failure of config option compatability with the feature
         * spec.
         * 
         * @param option   Configuration option that ultimately originates from
         *                  the user.
         * @param feature  Feature specification that it compares to the user
         *                  config.
         * @param translations Environment/parameters used to translation
         *                      occurances of path elements that begin with a "$".
         * @param order    Product order that accumulates validation of the
         *                  options with the feature specs.
         */
      inline void process_file( const ProductOption &option, 
                                const ProductFeature &feature,
                                const PsmrtsTranslations &translations,
                                ProductOrder &order ) const {

        if ( feature.type() == "directory") {
          // std::cout << "DirectoryOption File: " << option.to_string() << std::endl;
          order.add_option( option );
          std::string expanded_d = translations.translate_path( option.to_string() );
          if ( expanded_d != option.to_string() ) {
              order.add_metadata( ProductOption( option.name() +"_expanded", expanded_d ) );
          }
        }
        else if ( feature.validate_file_suffix( option.to_string() ) ) {
          // Its a file.
          // std::cout << "FileOption File: " << option.to_string() << std::endl;

          order.add_option( option );
          std::string expanded_f = translations.translate_path( option.to_string() );
          if ( option.to_string() != expanded_f ) {
            order.add_metadata( ProductOption( option.name()+"_expanded", expanded_f) );
          }
        }
        else {
          // Its not compatible with this one
          std::string mess = "*** ProductSpecification::process_order() - "
                              "Invalid filename/extension in option(" 
                              + option.name() + ") = " + option.to_string();
          order.add_error( std::runtime_error( mess ) );
          order.add_residual( option );
        } 
        return;                                   
      }
      

      /**
       * @brief Process a double feature specification with a config option
       * 
       * This method processes compares a product config option with a feature
       * specification. Option values are extracted and compared with feature
       * specs for validity. Errors that occur are recorded in the order.
       * 
       * @param option  Configuration option that ultimately originates from
       *                  the user.
       * @param feature  Feature specification that it compares to the user
       *                  config.
       * @param order    Product order that accumulates validation of the
       *                   options with the feature specs.
       */
      inline void process_doubles( const ProductOption &option, 
                                  const ProductFeature &feature,
                                  ProductOrder &order ) const {
        std::vector<double> d_values;
        psmrts::optvis::DoublesVisitor visitor_d = OptionDoublesExtractor( option ).create_visitor( d_values, option );
        option.visit( visitor_d );
        bool is_all_valid = true;

        for ( size_t ndx = 0 ; ndx < d_values.size() ; ndx++ ) {
          if ( !visitor_d.isvalid( d_values[ndx] ) ) {
            std::string mess = "*** ProductSpecification::process_order() - "
                              "Double value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            order.add_error( std::runtime_error( mess ) );
          }
        }
        
        // Check for valid values if present in feature
        if ( feature.contains( "valid" ) ) {
          std::vector<double> valids_d = OptionDoublesExtractor( option, visitor_d.traits() ).get_all();
          for ( size_t opt_nth = 0  ; opt_nth < d_values.size() ; opt_nth++  ) {
            bool is_valid = false;
            for ( size_t vld_nth = 0 ; vld_nth < valids_d.size() ; vld_nth++ ) {
              if ( visitor_d.isequal( d_values[opt_nth], valids_d[vld_nth]) ) {
                is_valid = true;
                break;
              }
            }
            // Check for a valid value
            if ( is_valid == false ) {
              is_all_valid = false;
              std::string mess = "*** ProductSpecification::process_order() - "
                                "Double value " + option.to_string( opt_nth )  + 
                                " at index = " + std::to_string( opt_nth ) +
                                " in option(" + option.name() + ") is not valid!";
              order.add_error( std::runtime_error( mess ) );                    
            }
          }
        }

        if ( is_all_valid == true ) {
          order.add_option( option );
        }
        else {
          order.add_residual( option );
        }
      
        return;
      }

      /**
       * @brief Process an integer feature specification with a config option
       * 
       * This method processes compares a product config option with a feature
       * specification. Option values are extracted and compared with feature
       * specs for validity. Errors that occur are recorded in the order.
       * 
       * @param option  Configuration option that ultimately originates from
       *                  the user.
       * @param feature  Feature specification that it compares to the user
       *                  config.
       * @param order    Product order that accumulates validation of the
       *                   options with the feature specs.
       */
      inline void process_integers( const ProductOption &option, 
                                    const ProductFeature &feature,
                                    ProductOrder &order ) const {     
        std::vector<int> i_values;
        psmrts::optvis::IntegersVisitor visitor_i = OptionIntegersExtractor( option ).create_visitor( i_values, option );
        option.visit( visitor_i );
        bool is_all_valid = true;
      
        for ( size_t ndx = 0 ; ndx < i_values.size() ; ndx++ ) {
          if ( !visitor_i.isvalid( i_values[ndx] ) ) {
            std::string mess = "*** ProductSpecification::process_order() - "
                              "Integer value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            order.add_error( std::runtime_error( mess ) );
            is_all_valid = false;
          }
        }

        // Check for valid values if present in feature
        if ( feature.contains( "valid" ) ) {
          std::vector<int> valids_i = OptionIntegersExtractor( option, visitor_i.traits() ).get_all();
          for ( size_t opt_nth = 0  ; opt_nth < i_values.size() ; opt_nth++  ) {
            bool is_valid = false;
            for ( size_t vld_nth = 0 ; vld_nth < valids_i.size() ; vld_nth++ ) {
              if ( visitor_i.isequal( i_values[opt_nth], valids_i[vld_nth]) ) {
                is_valid = true;
                break;
              }
            }
            // Check for a valid value
            if ( is_valid == false ) {
              is_all_valid = false;
              std::string mess = "*** ProductSpecification::process_order() - "
                                "Integer value " + option.to_string( opt_nth )  + 
                                " at index = " + std::to_string( opt_nth ) +
                                " in option(" + option.name() + ") is not valid!";
              order.add_error( std::runtime_error( mess ) );                    
            }
          }
        }

        if ( is_all_valid == true ) {
          order.add_option( option );
        }
        else {
          order.add_residual( option );
        }        
      }

      /**
       * @brief Process an size_t feature specification with a config option
       * 
       * This method processes compares a product config option with a feature
       * specification. Option values are extracted and compared with feature
       * specs for validity. Errors that occur are recorded in the order.
       * 
       * @param option  Configuration option that ultimately originates from
       *                  the user.
       * @param feature  Feature specification that it compares to the user
       *                  config.
       * @param order    Product order that accumulates validation of the
       *                   options with the feature specs.
       */
      inline void process_size_t( const ProductOption &option, 
                                    const ProductFeature &feature,
                                    ProductOrder &order ) const {     
        std::vector<size_t> st_values;
        psmrts::optvis::SizetsVisitor visitor_st = OptionSizetsExtractor( option ).create_visitor( st_values, option );
        option.visit( visitor_st );
        bool is_all_valid = true;
      
        for ( size_t ndx = 0 ; ndx < st_values.size() ; ndx++ ) {
          if ( !visitor_st.isvalid( st_values[ndx] ) ) {
            std::string mess = "*** ProductSpecification::process_order() - "
                              "Size_t value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            order.add_error( std::runtime_error( mess ) );
            is_all_valid = false;
          }
        }

        // Check for valid values if present in feature
        if ( feature.contains( "valid" ) ) {
          std::vector<size_t> valids_st = OptionSizetsExtractor( option, visitor_st.traits() ).get_all();
          for ( size_t opt_nth = 0  ; opt_nth < st_values.size() ; opt_nth++  ) {
            bool is_valid = false;
            for ( size_t vld_nth = 0 ; vld_nth < valids_st.size() ; vld_nth++ ) {
              if ( visitor_st.isequal( st_values[opt_nth], valids_st[vld_nth]) ) {
                is_valid = true;
                break;
              }
            }
            // Check for a valid value
            if ( is_valid == false ) {
              is_all_valid = false;
              std::string mess = "*** ProductSpecification::process_order() - "
                                "Size_t value " + option.to_string( opt_nth )  + 
                                " at index = " + std::to_string( opt_nth ) +
                                " in option(" + option.name() + ") is not valid!";
              order.add_error( std::runtime_error( mess ) );                    
            }
          }
        }

        if ( is_all_valid == true ) {
          order.add_option( option );
        }
        else {
          order.add_residual( option );
        }        
      }

      /**
      * @brief Process a boolean feature specification with a config option
       * 
       * This method processes compares a product config option with a feature
       * specification. Option values are extracted and compared with feature
       * specs for validity. Errors that occur are recorded in the order.
       * 
       * @param option  Configuration option that ultimately originates from
       *                  the user.
       * @param feature  Feature specification that it compares to the user
       *                  config.
       * @param order    Product order that accumulates validation of the
       *                   options with the feature specs.
       */
      inline void process_booleans( const ProductOption &option, 
                                    const ProductFeature &feature,
                                    ProductOrder &order ) const {   
        // Process these as strings for better error detection
        std::vector<std::string> b_values;
        psmrts::optvis::StringsVisitor visitor_b = OptionStringsExtractor( option ).create_visitor( b_values, option );
        option.visit( visitor_b );
        bool is_valid = true;

        for ( size_t ndx = 0 ; ndx < b_values.size() ; ndx++ ) {
          if ( !visitor_b.isvalid( b_values[ndx] ) ) {
            std::string mess = "*** ProductSpecification::process_order() - "
                              "Boolean value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            order.add_error( std::runtime_error( mess ) );
            is_valid = false;
          }
        } 
        
        if ( is_valid == true ) {
          order.add_option( option );
        }
        else {
          order.add_residual( option );
        }
        return;
      }

      /**
       * @brief Process a feature specification with a config option using strings
       * 
       * This method processes compares a product config option with a feature
       * specification using string value comparisons. This method should work
       * best with most all options if the feature specs are robust and correct.
       * 
       * Option values are extracted as strings and compared with feature
       * specs for validity. Errors that occur are recorded in the order.
       * 
       * @param option  Configuration option that ultimately originates from
       *                  the user.
       * @param feature  Feature specification that it compares to the user
       *                  config.
       * @param order    Product order that accumulates validation of the
       *                   options with the feature specs.
       */
      inline void process_strings( const ProductOption &option, 
                                   const ProductFeature &feature,
                                   ProductOrder &order ) const { 
        std::vector<std::string> s_values;
        psmrts::optvis::StringsVisitor visitor_s = OptionStringsExtractor( option ).create_visitor( s_values, option );
        option.visit( visitor_s );
        bool is_all_valid = true;

        for ( size_t ndx = 0 ; ndx < s_values.size() ; ndx++ ) {
          if ( !visitor_s.isvalid( s_values[ndx] ) ) {
            std::string mess = "*** ProductSpecification::process_order() - "
                              "String value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            order.add_error( std::runtime_error( mess ) );
            is_all_valid = false;
          }
        }
        
        // Check for valid values if present in feature
        if ( feature.contains( "valid" ) ) {
          std::vector<std::string> valids_s = OptionStringsExtractor( option, visitor_s.traits() ).get_all();
          for ( size_t opt_nth = 0  ; opt_nth < s_values.size() ; opt_nth++  ) {
            bool is_valid = false;
            for ( size_t vld_nth = 0 ; vld_nth < valids_s.size() ; vld_nth++ ) {
              if ( visitor_s.isequal( s_values[opt_nth], valids_s[vld_nth]) ) {
                is_valid = true;
                break;
              }
            }
            // Check for a valid value
            if ( is_valid == false ) {
              is_all_valid = false;
              std::string mess = "*** ProductSpecification::process_order() - "
                                "String value " + option.to_string( opt_nth )  + 
                                " at index = " + std::to_string( opt_nth ) +
                                " in option(" + option.name() + ") is not valid!";
              order.add_error( std::runtime_error( mess ) );                    
            }
          }
        }        

        if ( is_all_valid == true ) {
          order.add_option( option );
        }
        else {
          order.add_residual( option );
        }          
      }
      
    private:
      std::string     m_name;
      std::string     m_product;
      ProductInfo     m_info;
      ProductFeatures m_features;
      std::optional<Creator> m_creator;
      
  };

} // namespace psmrts

#endif
