/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductProcessing_hpp
#define ProductProcessing_hpp

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
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/ProductMaker.hpp>
#include <psmrts/core/AllOptionConversions.hpp>


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
   * @history 2026-02-15 Kris J. Becker - Original Version
   */
  class ProductProcessing {
    public:
      using ProductInfo      = PsmrtsContainer<ProductOption>;
      using ProductFeatures  = PsmrtsContainer<ProductFeature>;


      ProductProcessing( ) : m_translator( ) { }
      ProductProcessing( const PsmrtsTranslations &trans ) : m_translator( trans ) { }       
      virtual ~ProductProcessing() = default;


      inline const PsmrtsTranslations &translator() const {
        return ( m_translator );
      }

      inline void set_translator( const PsmrtsTranslations &trans ) {
         m_translator  = trans;
      }


           /**
       * @brief Process a configuration returning a product order with status
       * 
       * This method function takes a compound configuration and returns
       * results that are intended to create 
       * 
       * @param config 
       * @param translations 
       * @return ProductOrder 
       */
      inline ProductOrder process_configuration( const ProductConfiguration &config ) { 
        ProductOrder order_t( config.name() );                              
        if ( config.size() == 0 ) return ( order_t );

        // Process the till the first occurance of valid or no errors occurs
        auto tracer_specs_v = ProductMaker<PsmrtsTracer>().get_product_specs();
        for ( const auto &tracer_s : tracer_specs_v ) {
          order_t = this->process_cart( ProductCart( tracer_s, config) );
          order_t.add_dependency( tracer_s.name() );

          // If this parse is successful, we are done and its a standalone tracer.
          if ( order_t.isvalid() ) {
            return ( order_t );
          }

          // Check for errors. If none break for shape processing
          if ( order_t.error_count() == 0 ) {
            break;
          }
        }
        
        // If we have errors, then no tracer is detected/valid for this config
        // and we only have a shape to consider. Pass the orginal config for
        // shape processing. 
        //
        // If we have no errors but its not valid, assume a shape is required
        // and copy the residual config and process shape.
        ProductConfiguration config_t( config.name()  );
        ProductOrder order_s( config.name() );
        if ( ( order_t.error_count() > 0 ) || ( order_t.size() == 0 )) {
          // Process as shape only, start over
          config_t = config;
        }
        else {
          // order_t content contains processed tracer, lets see if we have
          // shape to consume the remaining residual/dependencies
          config_t = order_t.residual_config();
        }

        auto shape_specs_v  = ProductMaker<PsmrtsShape>().get_product_specs();
        for ( const auto &shape_s : shape_specs_v ) {
          order_s = this->process_cart( ProductCart( shape_s, config_t ) );
          if ( order_s.isvalid() ) break;
        }

        // Make it all make sense. Retain errors and status in the composite
        // version.
        order_t = order_s.make_composite( order_t );

        return ( order_t );
      }

      /**
       * @brief Process/compare a product configuration with a feature specification
       * 
       * This method is used to process a user specification comparing the
       * option to a feature of the specification. Product features contain a
       * set of keyword/value metadata specifying conditions that must be met by
       * the config option.
       * 
       * Note that special cases may exist when processing comparing
       * configuraitons with specifications. Specs can can contain dependency
       * keywords that are reqiured to exist, but are passed on unprocessed as a
       * residual option. This is most prevelant in some tracers, such as
       * "bullet" that requires a mesh shape. This case 
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
      inline ProductOrder process_cart( const ProductCart &cart ) const {
        
        // Check for invalid configuration
        ProductOrder order( cart, this->translator() );
        if ( cart.has_valid_content() == false ) {
          std::string mess = "ProductProcssing::process_cart(" + cart.name() + ") does not contain a valid product configuration";
          order.add_error( std::runtime_error( mess ) );
          return ( order );
        }

        std::vector<std::string> required_list;
        const ProductSpecification &specs_t = cart.specification();
        for ( const auto &option : cart.options() ) {

          std::string f_name = specs_t.get_alias_feature_name( option.name() );
          if ( specs_t.contains( option.name() ) || specs_t.contains( f_name ) ) {
            if ( f_name == "" ) f_name = option.name();

            const ProductFeature &feature = specs_t.find( f_name );
            if ( feature.is_required() ) required_list.push_back( f_name );

            if ( feature.is_dependency() ) {
              // Dependency keys are pushed as is into residual options for
              // additional processing. This occurs, for example, for some
              // tracers that require a shape. Not all do. But they are require
              // to exist.
              order.add_dependency( option, f_name );
            }
            else { 

              // Get the real name of the option
              ProductOption option_t( f_name, option );
              specs_t.valid_option_with_feature( option_t, feature, order );          

              // Process based upon the feature type
              if ( ( feature.type() == "file" ) || ( feature.type() == "directory" )) {
                process_file( option_t, feature, order );
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
          }
          else {
            // This may or may not be an error so callers must check conditions
            // std::cout << "SpecResidual: " << option.name() << std::endl;
            order.add_residual( option );
          }
        }

        // Now check to see if all required keywords are present
        for ( const auto &key_r : specs_t.required() ) {
          if ( !this->contains( key_r, required_list ) ) {
            std::string mess = "*** ProductProcessing::process_cart(" + cart.name() + ")"
                              "- required feature key " + key_r  + 
                              " is not present in " + specs_t.name() + " specification";
            order.add_error( std::runtime_error( mess ) );
          }
        }

        return ( order );
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
                                ProductOrder &order ) const {

        if ( feature.type() == "directory") {
          // std::cout << "DirectoryOption File: " << option.to_string() << std::endl;
          order.add_option( option );
          std::string expanded_d = order.translate_path( option.to_string() );
          if ( expanded_d != option.to_string() ) {
              order.add_metadata( ProductOption( option.name() +"_expanded", expanded_d ) );
          }
        }
        else if ( feature.validate_file_suffix( option.to_string() ) ) {
          // Its a file.
          //  << "FileOption File: " << option.to_string() << std::endl;

          order.add_option( option );
          std::string expanded_f = order.translate_path( option.to_string() );
          if ( option.to_string() != expanded_f ) {
            // std::cout << "FileOption FileExpanded: " << expanded_f << std::endl;
            order.add_metadata( ProductOption( option.name()+"_expanded", expanded_f) );
          }
        }
        else {
          // Its not compatible with this one
          // std::cout << "FileOption File Invalid: " << option.name() << std::endl;
          std::string mess = "*** ProductProcessing::process_order() - "
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
            std::string mess = "*** ProductProcessing::process_order() - "
                              "Double value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            order.add_error( std::runtime_error( mess ) );
          }
        }
        
        // Check for valid values if present in feature
        if ( feature.contains( "valid" ) ) {
          std::vector<double> valids_d = OptionDoublesExtractor(feature.find("valid"), visitor_d.traits() ).get_all();
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
              std::string mess = "*** ProductProcessing::process_order() - "
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
            std::string mess = "*** ProductProcessing::process_order() - "
                              "Integer value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            order.add_error( std::runtime_error( mess ) );
            is_all_valid = false;
          }
        }

        // Check for valid values if present in feature
        if ( feature.contains( "valid" ) ) {
          std::vector<int> valids_i = OptionIntegersExtractor( feature.find("valid"), visitor_i.traits() ).get_all();
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
              std::string mess = "*** ProductProcessing::process_order() - "
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
            std::string mess = "*** ProductProcessing::process_order() - "
                              "Size_t value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            order.add_error( std::runtime_error( mess ) );
            is_all_valid = false;
          }
        }

        // Check for valid values if present in feature
        if ( feature.contains( "valid" ) ) {
          std::vector<size_t> valids_st = OptionSizetsExtractor( feature.find("valid"), visitor_st.traits() ).get_all();
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
              std::string mess = "*** ProductProcessing::process_order() - "
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
            std::string mess = "*** ProductProcessing::process_order() - "
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
            std::string mess = "*** ProductProcessing::process_order() - "
                              "String value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            order.add_error( std::runtime_error( mess ) );
            is_all_valid = false;
          }
        }
        
        // Check for valid values if present in feature
        if ( feature.contains( "valid" ) ) {
          std::vector<std::string> valids_s = OptionStringsExtractor( feature.find("valid"), visitor_s.traits() ).get_all();
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
              std::string mess = "*** ProductProcessing::process_order() - "
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
      PsmrtsTranslations m_translator;

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
        if ( std::find( v.begin(), v.end(), s) != v.end() ) return ( true );
        return ( false );
      }
      
  };

} // namespace psmrts

#endif
