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
#include <mutex>
#include <tuple>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductFeature.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
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
      using UIDType          = PsmrtsProduct::UIDType;

      using ShapeInventory   = PsmrtsInventory::ShapeInventory;
      using TracerInventory  = PsmrtsInventory::TracerInventory;      
      using ResidualList     = ProductSpecification::ResidualList;      

      using ShapeCacheMap    = PsmrtsInventory::ShapeCacheMap;
      using TracerCacheMap   = PsmrtsInventory::TracerCacheMap;


      /** Default constructor for product processing */
      ProductProcessing( ) : m_translator( ) { }

      /** Constructor with customized path translator */
      ProductProcessing( const SharedTranslations &trans ) : 
                         m_translator( trans ) { }       
      virtual ~ProductProcessing() = default;


      /** Returns the file path translator */
      inline const SharedTranslations &translator() const {
        return ( m_translator );
      }

      /** Sets a new file path translator replacing the existing instance */
      inline void set_translator( const SharedTranslations &trans ) {
         m_translator  = trans;
      }

      /** Translate a file path using the path translator */
      inline std::string translate_path( const std::string &filepath ) const {
        if ( !m_translator ) return ( filepath );
        return ( m_translator->translate_path( filepath ) );
      }


      /**
       * @brief Search a shape inventory that matches the validate product config
       * 
       * Applies a search algorithm to find an exising shape product in the
       * given shape inventory. The parameter config is expected to be a
       * processed configuration validate with a specific product specification.
       * 
       * @param config    Processed/validated product configuration
       * @param inventory Shape inventory of active shape products
       * @param shape     An optional parameter that will contain a compatible
       *                   shape product if found in the search
       * @return ProductOrder Returns the order that should be checked for
       *                        errors to determine validity of search
       */
      inline SharedShape search_shape_inventory( const ProductCart &cart, 
                                                  const ShapeInventory &inventory ) 
                                                  const {
        // Create the shape search lambda method to run the search directly on
        // the shape cache map using thread-safe techniques
        const ProductSpecification &spec_t   = cart.specification();
        const ProductConfiguration &config_t = cart.configuration();

        PsmrtsErrors errors;
        SharedShape shape_p;
        auto shape_search = [&]( const ShapeCacheMap &map_c ) -> bool {
          for ( const auto &[ uid, p ] : map_c ) {
            ProductCart cart_s( p->specs(), p->config() );
            if ( spec_t.name() == p->specs().name() ) {
              if ( this->compare_product_config( config_t, cart_s, errors ) ) {
                shape_p = p;
                return ( true );
              }
            }
          }
          return ( false );
        };

        // Run the search and return result
        inventory.process( shape_search );
        return ( shape_p );
      }

      /**
       * @brief Search a tracer inventory that matches the validate product config
       * 
       * Applies a search algorithm to find an exising tracer product in the
       * given tracer inventory. The parameter config is expected to be a
       * processed configuration validate with a specific product specification.
       * 
       * Note that this search does not check the shape of a tracer if it exists
       * because if does not have a cart that defines the shape configuration.
       * See search_inventory for that.
       * 
       * @param config       Processed/validated product configuration
       * @param inventory    Tracer inventory of active tracer products
       * @return ShapeTracer Returns a valid shared pointer to the tracer that
       *                      matches the cart config.
       */
      inline SharedTracer search_tracer_inventory( const ProductCart &cart, 
                                                   const TracerInventory &inventory_t )
                                                   const {

         // Create the shape search lambda method to run the search directly on
        // the shape cache map using thread-safe techniques
        const ProductSpecification &spec_t   = cart.specification();
        const ProductConfiguration &config_t = cart.configuration();

        PsmrtsErrors errors;
        SharedTracer tracer_p;
        auto tracer_search = [&]( const TracerCacheMap &map_c ) -> bool {
          for ( const auto &[ uid, p ] : map_c ) {
            ProductCart cart_t( p->specs(), p->config() );
            if ( spec_t.name() == p->specs().name() ) {
              if ( this->compare_product_config( config_t, cart_t, errors ) ) {
                tracer_p = p;
                return ( true );
              }
            }
          }
          return ( false );
        };

        // Run the search and return result
        inventory_t.process( tracer_search );
        return ( tracer_p );
      }

      /**
       * @brief Search inventories for a product that satisifies the configuration
       * 
       * @param set_p      Prodoct set containing configurations and specs
       * @param inventory  PSMRTS inventory to search for products
       * @return true      If the product set was fully resolved in the search
       * @return false     If the search failed
       */

       /**
        * @brief Searches for products specified in the product order
        * 
        * @param order       Order containing tracers and/or shape configurations
        * @param inventory_t Tracer inventory to search
        * @param inventory_p Shape inventory to searh
        * @return std::tuple<bool, SharedTracer, SharedShape> Returns if the search
        *             was successful as specified in the order and then shared
        *             pointers to the products.
        */
      inline std::tuple<bool, SharedTracer, SharedShape> search_inventory( const ProductOrder &order, 
                                                                     const TracerInventory &inventory_t,
                                                                     const ShapeInventory &inventory_s ) 
                                                                     const {

        SharedTracer tracer_p;
        SharedShape  shape_p;
        if ( !order.isvalid() ) return ( std::make_tuple( false, tracer_p, shape_p) );

        SharedCart tracer_c = order.find( "tracer" );
        SharedCart shape_c  = order.find( "shape" );
        bool success = false;  // fail condition is default
        if ( tracer_c ) {
          auto tracer_t = this->search_tracer_inventory( *tracer_c, inventory_t );
          if ( tracer_t ) {
            // Now check if a shape exists and it matches the shape config
            PRQShape shaper_t;
            if ( shape_c && tracer_t->process( shaper_t ) ) {
              auto shape_t = shaper_t.shape();
              ProductCart cart_s( shape_t->specs(), shape_t->config() );
              PsmrtsErrors errors;
              if ( this->compare_product_config( shape_c->configuration(), cart_s, errors ) ) {
                tracer_p = tracer_t;
                shape_p  = shape_t;
                success = true;
              }
            }
            else if ( !shape_c ) {
              tracer_p = tracer_t;
              success = true;
            }
          }
        }
        else if ( shape_c ) {
          shape_p = this->search_shape_inventory( *shape_c, inventory_s );
          if ( shape_p ) success = true;
        }                  
            
        return ( std::make_tuple( success, tracer_p, shape_p ) );
      }


      /**
       * @brief Process/validate a PSMRTS product configuration 
       * 
       * @param config  Product configuration containing all options for tracers
       *                  and shapes
       * @return SharedOrder Contains the processed options into carts
       *                              for searching for and creating new products
       */
      inline SharedOrder process_order( const ProductConfiguration &config ) 
                                               const {

        SharedOrder order_t = make_shared_copy( ProductOrder( config, m_translator ) );
        if ( config.size() == 0 ) {
          order_t->add_error( "process_order() - configuration has no options" ); 
          return ( order_t );
        }

        // Process the till the first occurance of valid or no errors occurs
        PsmrtsErrors all_errors;
        ProductCart cart_t;
        auto tracer_specs_v = ProductMaker<PsmrtsTracer>().get_product_specs();
        for ( const auto &tracer_s : tracer_specs_v ) {
           cart_t = ProductCart( tracer_s );

          this->process_cart( config, cart_t );

          // If this parse is successful, we are done and its a standalone tracer.
          if ( cart_t.has_valid_content() ) {
            order_t->add( cart_t );
            return ( order_t  );
          }

          // Check for errors. If none break for shape processing
          if ( cart_t.error_count() == 0 ) {
            break;
          }
          
          // Accumulate error conditions
          all_errors.append( cart_t );
        }

        // If we have errors, then no tracer is detected/valid for this config
        // and we only have a shape to consider. Pass the original config for
        // shape processing. 
        //
        // If we have no errors but its not valid, assume a shape is required
        // and copy the residual config and process shape.
        ProductConfiguration config_t( config.name()  );
        if ( ( cart_t.error_count() > 0 ) || ( cart_t.size() == 0 )) {
          // Process as shape only, start over
          config_t = config;
          all_errors.clear_errors();
        }
        else {
          // cart_t content contains processed tracer, lets see if we have
          // shape to consume the remaining residual/dependencies
          config_t = cart_t.residual_config();
          cart_t.clear_residuals();
        }

        ProductCart cart_s;
        auto shape_specs_v  = ProductMaker<PsmrtsShape>().get_product_specs();
        for ( const auto &shape_s : shape_specs_v ) {
          cart_s = ProductCart( shape_s );
          this->process_cart( config_t, cart_s );
          if ( cart_s.has_valid_content() ) {
            // Is there a tracer with this shape?
            if ( cart_t.has_valid_content() ) {
              order_t->add( cart_t );
            }
            order_t->add( cart_s );
            return ( order_t );
          }
          // Accumulate error conditions
          all_errors.append( cart_s );          
        }

        order_t->append( all_errors );
        return ( order_t );        

      }

      /**
       * @brief Expedites a search comparison of an existing product with a config
       * 
       * This method is intended to compare a new config with an existing
       * product that has a processed configuration and a specification. This
       * actually performs a check on an inventory search condition. The cart
       * contains the specs and config that has been extracted from an existing
       * tracer or shape product. It is used to compare against the contents of
       * the config parameter to determine if they match. 
       * 
       * If the contents of the config do not match, an error is added to the
       * returned ProductOrder indicating a failed match of the config and the
       * product cart.
       * 
       * If no errors are encountered, the order contains the orginal cart, and
       * the order configuration where its configuration contains the product
       * UID for useage.
       * 
       * @param config Configuration to compare with the product_cart
       * @param cart   Cart containing the spec and config of the product
       *                 to compare.
       * @return true Returns true if the config and cart match
       */
      inline bool compare_product_config( const ProductConfiguration &config,
                                          const ProductCart &cart,
                                          PsmrtsErrors &errors ) const { 

        errors.clear_errors();

        // Prepare the cart for comparisons of the product configuration
        ProductConfiguration config_new( config.name() );

        const ProductSpecification &specs_c  = cart.specification();
        const ProductConfiguration &config_c = cart.configuration();

        for ( const ProductOption &option : config.options() ) {
          std::string name_t = option.name();
          std::string f_name = specs_c.get_alias_feature_name( name_t, name_t );

          if ( specs_c.contains( f_name ) ) {

            const ProductFeature &feature = specs_c.find( f_name );
            if ( feature.is_dependency() ) {
              f_name = name_t;
            }

            ProductOption option_f( f_name, option );
            config_new.add_option( option_f );

            // Compare the cart config option if it exists, otherwise ensure the
            // value is an option default value.
            if ( compare_feature_options( option_f, config_c, feature ) ) {

              if ( feature.is_path_type() ) {
                std::string f_extended = f_name + "_extended";
                if ( config.metadata().contains( name_t+"_extended" ) ) {
                  config_new.add_metadata( ProductOption( f_extended, 
                                                          config.metadata().find( name_t+"_extended" ) ) );
                }
                else {
                  config_new.add_metadata( ProductOption( f_extended, 
                                                          this->translate_path( option.to_string() ) ) );
                }
              }
            }
            else {
              errors.add_error( "\"" + name_t + "\" option is invalid or isn't the default in specs " + specs_c.name() );              
            }
          }
          else {
            errors.add_error( "\"" + name_t + "\" is not found in specs for " + specs_c.name() );
          }
        }

        // Now check for required keywords
        for ( const std::string &key_r : specs_c.required() ) {
          if ( !config_new.contains( key_r ) ) {
            errors.add_error( "Required key \"" + key_r + "\" not found in config " + config.name() );
          }
        }

        // Reverse check of product keys to determine keys exist there that are
        // not in the requested config. If they don't exist, see if they are the
        // default.
        for ( const ProductOption &option_c : config_c.options() ) {
          if ( !config_new.contains( option_c.name() ) ) {
            specs_c.validate_option_default( option_c, errors );
          }
        }

        return ( errors.error_count() == 0  );
      }

      /** Compare two options with its feature type */
      inline bool compare_options( const ProductOption &o1, const ProductOption &o2,
                                   const ProductFeature &feature ) const {

        // Process based upon the feature type
        if ( feature.type() == "double" ) {
         return ( ProductOption::DoublesComparator::compare( o1, o2 ) ) ;
        }
        else if ( feature.type() == "int" ) {
         return ( ProductOption::IntegersComparator::compare( o1, o2 ) ) ;
        }
        else if ( feature.type() == "size_t" ) {
         return ( ProductOption::SizetsComparator::compare( o1, o2 ) ) ;
        }            
        else if ( feature.type() == "bool" ) {
         return ( ProductOption::BoolsComparator::compare( o1, o2 ) ) ;
        }           
        else { // treat the rest as strings
         return ( ProductOption::StringsComparator::compare( o1, o2 ) ) ;
        }

        return ( false );
      }

      /**
       * @brief Compare an option against a product config and feature
       * 
       * This method compares a product option to an existing product
       * configuraiton and its associate feature specification.
       * 
       * @param option  Product option to compare to a product config/feature
       * @param config  Product configuration with potenital option to compare
       *                 for equivalent values. 
       * @param feature Product feature to validate option
       * @return true   If the option compares with an existing config option or
       *                  contains a specified default
       * @return false  If the option does not compare/match product
       *                  config/feature 
       */
      inline bool compare_feature_options( const ProductOption &option, 
                                           const ProductConfiguration &config, 
                                           const ProductFeature &feature ) const {

        if ( feature.is_dependency( ) ) return ( true );

        // Compare options
        if ( config.contains( option.name() ) ) {
          return ( compare_options( option, config.find( option.name() ), feature) );
        }
        else {
          if ( feature.contains( "default") ) {
            return ( compare_options( option, feature.find( "default" ), feature) );
          }
        } 

        // Here indicate the option did not exist in the config and it does not
        // have a default to compare with so its not equal.
        return ( false );
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
       * keywords that are required to exist, but are passed on unprocessed as a
       * residual option. This is most prevelant in some tracers, such as
       * "bullet" that requires a mesh shape. This case 
       * 
       * @param config     Original full tracer/shape configuration to process 
       * @param cart       Product cart configuration will be populated with the
       *                     result of processing. It will contain errors if 
       *                     the config is invalid.
       * @param translator Environment/parameter keyword/value pairs that will
       *                      replace occurances of path elements that begin
       *                      with a "$".
       * @return true      If processing was successful
       * @return false     If the config contained errors during processing
       */
      inline bool process_cart( const ProductConfiguration &config,
                                ProductCart &cart ) const {
       
        cart.clear_errors();
        
        std::vector<std::string> required_list;
        const ProductSpecification &specs_t = cart.specification();
        for ( const auto &option : config.options() ) {
          std::string option_name_t = option.name();
          std::string f_name = specs_t.get_alias_feature_name( option_name_t, option_name_t );
          if ( specs_t.contains( f_name ) ) {

            const ProductFeature &feature = specs_t.find( f_name );
            if ( feature.is_required() ) required_list.push_back( f_name );

            if ( feature.is_dependency() ) {
              // Dependency keys are pushed as is into residual options for
              // additional processing. This occurs, for example, for some
              // tracers that require a shape. Not all do. But they are required
              // to exist.
              cart.add_residual( option );
            }
            else { 

              // Get the real name of the option
              ProductOption option_t( f_name, option );
              specs_t.valid_option_with_feature( option_t, feature, cart );          

              // Process based upon the feature type
              if ( ( feature.type() == "file" ) || ( feature.type() == "directory" )) {
                process_file( option_t, feature, cart );
              }
              else if ( feature.type() == "double" ) {
                process_doubles( option_t, feature, cart );
              }
              else if ( feature.type() == "int" ) {
                process_integers( option_t, feature, cart );
              }
              else if ( feature.type() == "size_t" ) {
                process_size_t( option_t, feature, cart );
              }            
              else if ( feature.type() == "bool" ) {
                process_booleans( option_t, feature, cart );
              }           
              else { // treat the rest as strings
                process_strings( option_t, feature, cart );
              }
            }
          }
          else {
            // This may or may not be an error so callers must check conditions
            cart.add_residual( option );
          }
        }

        // Now check to see if all required keywords are present
        for ( const auto &key_r : specs_t.required() ) {
          if ( !psmrts_contains_string( key_r, required_list ) ) {
            std::string mess = "*** ProductCompare::process_cart(" + cart.name() + ")"
                              "- required feature key " + key_r  + 
                              " is not present in " + specs_t.name() + " specification";
            cart.add_error(  mess );
          }
        }

        return ( cart.error_count() > 0 );
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
                                ProductCart &cart ) const {

        if ( feature.type() == "directory") {
          cart.add_option( option );
          std::string expanded_d = this->translate_path( option.to_string() );
          if ( expanded_d != option.to_string() ) {
              cart.add_metadata( ProductOption( option.name() +"_expanded", expanded_d ) );
          }
        }
        else if ( feature.validate_file_suffix( option.to_string() ) ) {
          // Its a file.
          cart.add_option( option );
          std::string expanded_f = this->translate_path( option.to_string() );
          if ( option.to_string() != expanded_f ) {
            cart.add_metadata( ProductOption( option.name()+"_expanded", expanded_f) );
          }
        }
        else {
          // Its not compatible with this one
          std::string mess = "*** ProductCompare::process_cart() - "
                              "Invalid filename/extension in option (" 
                              + option.name() + " = " + option.to_string() + ")";
          cart.add_error(  mess );
          cart.add_residual( option );
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
                                   ProductCart &cart) const {
        std::vector<double> d_values;
        psmrts::optvis::DoublesVisitor visitor_d = OptionDoublesExtractor( option ).create_visitor( d_values, option );
        option.visit( visitor_d );
        bool is_all_valid = true;

        for ( size_t ndx = 0 ; ndx < d_values.size() ; ndx++ ) {
          if ( !visitor_d.isvalid( d_values[ndx] ) ) {
            std::string mess = "*** ProductCompare::process_order() - "
                              "Double value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            cart.add_error(  mess );
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
              std::string mess = "*** ProductCompare::process_order() - "
                                "Double value " + option.to_string( opt_nth )  + 
                                " at index = " + std::to_string( opt_nth ) +
                                " in option(" + option.name() + ") is not valid!";
              cart.add_error(  mess );                    
            }
          }
        }

        if ( is_all_valid == true ) {
          cart.add_option( option );
        }
        else {
          cart.add_residual( option );
        }
      
        return;
      }

      /**
       * @brief Process an integer feature specification with a config option
       * 
       * This method processes compares a product config option with a feature
       * specification. Option values are extracted and compared with feature
       * specs for validity. Errors that occur are recorded in the cart.
       * 
       * @param option  Configuration option that ultimately originates from
       *                  the user.
       * @param feature  Feature specification that it compares to the user
       *                  config.
       * @param cart    Product cart that accumulates validation of the
       *                   options with the feature specs.
       */
      inline void process_integers( const ProductOption &option, 
                                    const ProductFeature &feature,
                                    ProductCart &cart) const {     
        std::vector<int> i_values;
        psmrts::optvis::IntegersVisitor visitor_i = OptionIntegersExtractor( option ).create_visitor( i_values, option );
        option.visit( visitor_i );
        bool is_all_valid = true;
      
        for ( size_t ndx = 0 ; ndx < i_values.size() ; ndx++ ) {
          if ( !visitor_i.isvalid( i_values[ndx] ) ) {
            std::string mess = "*** ProductCompare::process_cart() - "
                              "Integer value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            cart.add_error(  mess );
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
              std::string mess = "*** ProductCompare::process_cart() - "
                                "Integer value " + option.to_string( opt_nth )  + 
                                " at index = " + std::to_string( opt_nth ) +
                                " in option(" + option.name() + ") is not valid!";
              cart.add_error(  mess );                    
            }
          }
        }

        if ( is_all_valid == true ) {
          cart.add_option( option );
        }
        else {
          cart.add_residual( option );
        }        
      }

      /**
       * @brief Process an size_t feature specification with a config option
       * 
       * This method processes compares a product config option with a feature
       * specification. Option values are extracted and compared with feature
       * specs for validity. Errors that occur are recorded in the cart.
       * 
       * @param option  Configuration option that ultimately originates from
       *                  the user.
       * @param feature  Feature specification that it compares to the user
       *                  config.
       * @param cart    Product cart that accumulates validation of the
       *                   options with the feature specs.
       */
      inline void process_size_t( const ProductOption &option, 
                                    const ProductFeature &feature,
                                    ProductCart &cart) const {     
        std::vector<size_t> st_values;
        psmrts::optvis::SizetsVisitor visitor_st = OptionSizetsExtractor( option ).create_visitor( st_values, option );
        option.visit( visitor_st );
        bool is_all_valid = true;
      
        for ( size_t ndx = 0 ; ndx < st_values.size() ; ndx++ ) {
          if ( !visitor_st.isvalid( st_values[ndx] ) ) {
            std::string mess = "*** ProductCompare::process_cart() - "
                              "Size_t value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            cart.add_error(  mess );
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
              std::string mess = "*** ProductCompare::process_cart() - "
                                "Size_t value " + option.to_string( opt_nth )  + 
                                " at index = " + std::to_string( opt_nth ) +
                                " in option(" + option.name() + ") is not valid!";
              cart.add_error(  mess );                    
            }
          }
        }

        if ( is_all_valid == true ) {
          cart.add_option( option );
        }
        else {
          cart.add_residual( option );
        }        
      }

      /**
      * @brief Process a boolean feature specification with a config option
       * 
       * This method processes compares a product config option with a feature
       * specification. Option values are extracted and compared with feature
       * specs for validity. Errors that occur are recorded in the cart.
       * 
       * @param option  Configuration option that ultimately originates from
       *                  the user.
       * @param feature  Feature specification that it compares to the user
       *                  config.
       * @param cart    Product cart that accumulates validation of the
       *                   options with the feature specs.
       */
      inline void process_booleans( const ProductOption &option, 
                                    const ProductFeature &feature,
                                    ProductCart &cart) const {   
        // Process these as strings for better error detection
        std::vector<std::string> b_values;
        psmrts::optvis::StringsVisitor visitor_b = OptionStringsExtractor( option ).create_visitor( b_values, option );
        option.visit( visitor_b );
        bool is_valid = true;

        for ( size_t ndx = 0 ; ndx < b_values.size() ; ndx++ ) {
          if ( !visitor_b.isvalid( b_values[ndx] ) ) {
            std::string mess = "*** ProductCompare::process_cart() - "
                              "Boolean value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            cart.add_error(  mess );
            is_valid = false;
          }
        } 
        
        if ( is_valid == true ) {
          cart.add_option( option );
        }
        else {
          cart.add_residual( option );
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
       * specs for validity. Errors that occur are recorded in the cart.
       * 
       * @param option  Configuration option that ultimately originates from
       *                  the user.
       * @param feature  Feature specification that it compares to the user
       *                  config.
       * @param cart    Product cart that accumulates validation of the
       *                   options with the feature specs.
       */
      inline void process_strings( const ProductOption &option, 
                                   const ProductFeature &feature,
                                   ProductCart &cart) const { 
        std::vector<std::string> s_values;
        psmrts::optvis::StringsVisitor visitor_s = OptionStringsExtractor( option ).create_visitor( s_values, option );
        option.visit( visitor_s );
        bool is_all_valid = true;

        for ( size_t ndx = 0 ; ndx < s_values.size() ; ndx++ ) {
          if ( !visitor_s.isvalid( s_values[ndx] ) ) {
            std::string mess = "*** ProductCompare::process_cart() - "
                              "String value " + option.to_string( ndx )  + 
                              " in option(" + option.name() + ") is invalid!";
            cart.add_error(  mess );
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
              std::string mess = "*** ProductCompare::process_cart() - "
                                "String value " + option.to_string( opt_nth )  + 
                                " at index = " + std::to_string( opt_nth ) +
                                " in option(" + option.name() + ") is not valid!";
              cart.add_error(  mess );                    
            }
          }
        }        

        if ( is_all_valid == true ) {
          cart.add_option( option );
        }
        else {
          cart.add_residual( option );
        }          
      }



      /** Determine if a product is valid */
      inline bool is_valid_order( const ProductOrder &product ) const {
        if ( product.name() == "none"  ) return ( false );
        if ( product.error_count() > 0 ) return ( false );
        if ( product.size() == 0       ) return ( false );
        return ( true );
      }

    private:
      SharedTranslations m_translator;
  };

} // namespace psmrts

#endif
