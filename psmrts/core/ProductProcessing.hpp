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
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/ProductFeature.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductOrder.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/ProductMaker.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/PsmrtsFactory.hpp>
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
  class ProductProcessing : public PsmrtsRequest {
    public:
      using ProductInfo      = PsmrtsContainer<ProductOption>;
      using ProductFeatures  = PsmrtsContainer<ProductFeature>;
      using UIDType          = PsmrtsProduct::UIDType;

      using ProductSet       = struct product_set { ProductConfiguration config; 
                                                    ProductOrder   tracer;
                                                    ProductOrder   shape;
                                                    UIDType        tracer_uid;
                                                    UIDType        shape_uid;
                                                    inline const std::string &name() const { 
                                                      return ( config.name() ); 
                                                    }
                                                  };

      ProductProcessing( ) : PsmrtsRequest( "processor" ), m_translator( ) { }
      ProductProcessing( const PsmrtsTranslations &trans ) : 
                         PsmrtsRequest( "processor" ),
                         m_translator( trans ) { }       
      virtual ~ProductProcessing() = default;


      inline const PsmrtsTranslations &translator() const {
        return ( m_translator );
      }

      inline void set_translator( const PsmrtsTranslations &trans ) {
         m_translator  = trans;
      }

      /**
       * @brief Template search of product inventories for an existing prouduct id
       * 
       * @tparam Inventory  Type of product inventory such as PsmrtsTracer or
       *                     PsmrtsShape. It could be a local or the factory
       *                     inventory. 
       * @tparam Product    An std::optional<T> where T is PsrmtsTracer or
       *                     PsmrtsShape
       * @param uid_t       Assumed to be the valid id of an existing product
       * @param order       The product order, asummed to be from a ProductOrder
       *                      that has been processed or correctness.
       * @param inventory   The appropriate product type inventory such as
       *                      tracers() or shapes()
       * @param product     The std::optional<T> that will return the product 
       * @return true       If the product was successfully found.
       * @return false      If it was not found.
       */
      template<typename Inventory, typename Product>
        inline bool search_inventory( const UIDType uid_t, 
                                      const ProductOrder &order,
                                      const Inventory &inventory, 
                                      Product &product ) const {
          
          // Don't test invalid uids
          if ( PsmrtsUID::is_valid_uid( uid_t ) && inventory.contains( uid_t ) ) {
            product.emplace( inventory.find( uid_t ) );
          }
          else {
            // search using configs
            // std::cout << "ConfigCacheCompare for " << order.name() << ", size = " << inventory.size() << std::endl;
            for ( const auto &[ uid, p ] : inventory.cache() ) {
              // std::cout << "\nConfigMatching:\nFirst: " << order.config().to_json().dump(1) << "\n\nSecond: " << p.config().to_json().dump(1) << std::endl;
              if ( order.config().matches( p.config() ) ) {
                product.emplace( p );
                break;
              }
            }
          }
          return ( product.has_value() );
        }

      /**
       * @brief Comprehensive shape maker complete with search of existing resources
       * 
       * This method will search the local inventory for a specific product id.
       * If unsuccessful, it will the iterate through all products comparing
       * configurations. If it finds a match it ensures the product is in both
       * the passed inventory and the factory inventory.
       * 
       * @param product_s  The product set that contains a valid shape product
       *                     order 
       * @param inventory  The local inventory to check for shape products
       * @param shape_p    The optional parameter to return the result
       * @return true      If a shape product is succesfully found or created.
       *                     Upon success the product set is updated accordingly. 
       * @return false     False if failure. Errors are reported in the shape
       *                     product order 
       */
      inline bool make_shape( ProductSet &product_s,
                              PsmrtsInventory &inventory,
                              std::optional<PsmrtsShape> &shape_p ) const {

        // Refuse to process an invalid product
        if ( !this->is_valid_order( product_s.shape ) ) return ( false );
        UIDType shape_uid = product_s.shape_uid;


        // Let first check to see if we have a shape in the current factory
        bool has_shape = search_inventory( shape_uid, product_s.shape, inventory.shapes(), shape_p );
        
        // If its not in the current inventory, check the factory
        if ( !has_shape ) {
          has_shape = search_inventory( shape_uid, product_s.shape, PsmrtsFactory().find().shapes(), shape_p );

          // Add to local inventory
          if ( has_shape) {
            inventory.shapes().add_product( shape_p.value() );
          }            
        }

        // Check to see if don't have a shape and search using configs
        // ok, we have to make one now
        if ( !shape_p.has_value() ) {
          ProductMaker<PsmrtsShape> maker_t( product_s.shape.name() );
          if ( maker_t.process_config( product_s.shape.config(), this->translator() ) ) {
            shape_p.emplace( maker_t.product() );
            inventory.shapes().add_product( shape_p.value() );
            PsmrtsFactory().add_product( shape_p.value() );
            product_s.shape_uid = shape_p.value().uid();
          }
          else {
            if (maker_t.error_count() > 0 ) {
              this->add_error( maker_t.errors_to_string() );
            }
          }          
        }
                  
        return ( shape_p.has_value() );
      }

      /**
       * @brief Comprehensive tracer maker complete with search of existing resources
       * 
       * This method will search the local inventory for a specific product id.
       * If unsuccessful, it will the iterate through all products comparing
       * configurations. If it finds a match it ensures the product is in both
       * the passed inventory and the factory inventory.
       * 
       * @param product_s  The product set that contains a valid tracer product
       *                     order 
       * @param inventory  The local inventory to check for tracer products
       * @param tracer_p   The optional parameter to return the result
       * @return true      If a tracer product is succesfully found or created.
       *                     Upon success the product set is updated accordingly. 
       * @return false     False if failure. Errors are reported in the tracer
       *                     product order 
       */      
      inline bool make_tracer( ProductSet &product_s,
                              PsmrtsInventory &inventory,
                              std::optional<PsmrtsTracer> &tracer_p ) const {

        // Refuse to process an invalid product
        if ( !this->is_valid_order( product_s.tracer ) ) return ( false );
        UIDType tracer_uid = product_s.tracer_uid;


        // Let first check to see if we have a shape in the current factory
        bool has_tracer = search_inventory( tracer_uid, product_s.tracer, inventory.tracers(), tracer_p );
        
        // If its not in the current inventory, check the factory
        if ( !has_tracer ) {
          has_tracer = search_inventory( tracer_uid, product_s.tracer, PsmrtsFactory().find().tracers(), tracer_p );

          // Add to local inventory
          if ( has_tracer ) {
            inventory.tracers().add_product( tracer_p.value() );
          }
        }

        // Check to see if don't have a shape and search using configs
        // ok, we have to make one now
        if ( !tracer_p.has_value() ) {
          ProductMaker<PsmrtsTracer> maker_t( product_s.tracer.name() );
          if ( maker_t.process_config( product_s.tracer.config(), this->translator() ) ) {
            tracer_p.emplace( maker_t.product() );
            inventory.tracers().add_product( tracer_p.value() );
            PsmrtsFactory().add_product( tracer_p.value() );
            product_s.tracer_uid = tracer_p.value().uid();
          }
          else {
            if (maker_t.error_count() > 0 ) {
              this->add_error( maker_t.errors_to_string() );
            }
          }
        }
                  
        return ( tracer_p.has_value() );
      }

      /**
       * @brief Convenience method to process both types of products
       * 
       * This method will process a product set that may contain a tracer and or
       * a shape. It will create local copies of the products made, but it
       * inserts each on in the inventory passed in as a paramter. It will also
       * update the product set to reflect the uids of the newly created or
       * existing products. Upon return, each product can be reference using the
       * product id in each product set from the inventory.
       * 
       * @param product   The product set containing the products
       * @param inventory The inventory to propulate with products
       * @return true     If the any type of the product set was successfully produced.
       * @return false    If both failed.
       */
      inline bool process_product_set( ProductSet &product, 
                                       PsmrtsInventory &inventory ) const {

        // Process each product type
        std::optional<PsmrtsShape>  shape_p( std::nullopt );
        bool status_p = make_shape( product, inventory, shape_p );

        std::optional<PsmrtsTracer> tracer_p( std::nullopt );
        bool status_t = make_tracer(product, inventory, tracer_p );

        return ( status_p || status_t );
      }


      /**
       * @brief Process a configuration returning a product order with status
       * 
       * This method function takes a compound configuration and returns
       * results that are intended to create a PsmrtsTracer or PsrmtsShape,
       * potentially both if properly configured.
       * 
       * @param config        The composite product configuration originating
       *                        from a user
       * @param translations  A environment/parameter file path translator
       * @return ProductSet   A product set that contains configurations for a
       *                        tracer and or a shape if specified.
       */
      inline ProductSet process_configuration( const ProductConfiguration &config ) { 
        ProductSet products_t = init_product_set( config );
        if ( config.size() == 0 ) {
          products_t.shape.add_error( "process_configuration() - Invalid configuration - has no options"  );
          return ( products_t );
        }

        // Process the till the first occurance of valid or no errors occurs
        auto tracer_specs_v = ProductMaker<PsmrtsTracer>().get_product_specs();
        for ( const auto &tracer_s : tracer_specs_v ) {
          products_t.tracer = this->process_cart( ProductCart( tracer_s, config) );
          // products_t.tracer.add_dependency( tracer_s.name() );

          // If this parse is successful, we are done and its a standalone tracer.
          if ( products_t.tracer.isvalid() ) {
            return ( products_t );
          }

          // Check for errors. If none break for shape processing
          if ( products_t.tracer.error_count() == 0 ) {
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
        if ( ( products_t.tracer.error_count() > 0 ) || ( products_t.tracer.size() == 0 )) {
          // Process as shape only, start over
          config_t = config;
          products_t.tracer.set_specification();        
        }
        else {
          // order_t content contains processed tracer, lets see if we have
          // shape to consume the remaining residual/dependencies
          config_t = products_t.tracer.residual_config();
        }

        auto shape_specs_v  = ProductMaker<PsmrtsShape>().get_product_specs();
        for ( const auto &shape_s : shape_specs_v ) {
          products_t.shape = this->process_cart( ProductCart( shape_s, config_t ) );
          if ( products_t.shape.isvalid() ) break;
        }

        // Check for errors 
        if ( ( products_t.shape.error_count() > 0 ) || ( products_t.shape.size() == 0 ) ) {
          products_t.shape.set_specification();
        }

        return ( products_t );
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
          std::string mess = "ProductProcessing::process_cart(" + cart.name() + ") does not contain a valid product configuration";
          order.add_error(  mess );
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
          if ( !psmrts_contains_string( key_r, required_list ) ) {
            std::string mess = "*** ProductProcessing::process_cart(" + cart.name() + ")"
                              "- required feature key " + key_r  + 
                              " is not present in " + specs_t.name() + " specification";
            order.add_error(  mess );
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
          order.add_option( option );
          std::string expanded_d = order.translate_path( option.to_string() );
          if ( expanded_d != option.to_string() ) {
              order.add_metadata( ProductOption( option.name() +"_expanded", expanded_d ) );
          }
        }
        else if ( feature.validate_file_suffix( option.to_string() ) ) {
          // Its a file.
          order.add_option( option );
          std::string expanded_f = order.translate_path( option.to_string() );
          if ( option.to_string() != expanded_f ) {
            order.add_metadata( ProductOption( option.name()+"_expanded", expanded_f) );
          }
        }
        else {
          // Its not compatible with this one
          // std::cout << "FileOption File Invalid: " << option.name() << std::endl;
          std::string mess = "*** ProductProcessing::process_order() - "
                              "Invalid filename/extension in option(" 
                              + option.name() + ") = " + option.to_string();
          order.add_error(  mess );
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
            order.add_error(  mess );
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
              order.add_error(  mess );                    
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
            order.add_error(  mess );
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
              order.add_error(  mess );                    
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
            order.add_error(  mess );
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
              order.add_error(  mess );                    
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
            order.add_error(  mess );
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
            order.add_error(  mess );
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
              order.add_error(  mess );                    
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

      inline ProductSet init_product_set( const ProductConfiguration &config = ProductConfiguration() ) const {
        ProductSet products;
        products.config = config;
        products.tracer_uid = PsmrtsUID::null_uid();
        products.shape_uid  = PsmrtsUID::null_uid();
        return ( products );
      }      


      /** Determine if a product is valid */
      inline bool is_valid_order( const ProductOrder &product ) const {
        if ( product.name() == "none"  ) return ( false );
        if ( product.error_count() > 0 ) return ( false );
        if ( product.size() == 0       ) return ( false );
        return ( true );
      }


      /** Determine if any of the products are valid */
      inline bool is_valid_product( const ProductSet &products ) const {
        if ( is_valid_order( products.tracer ) || 
             is_valid_order( products.shape  ) ) {
          return ( true );
        }
        return ( false );
      }

      /** Return a composite set of errors if the exist */
      inline std::string product_error_string( const ProductSet &products ) const { 
        std::string error_s;
        if ( products.tracer.error_count() > 0 ) {
          error_s += products.tracer.errors_to_string();
        } 

        if (  products.shape.error_count() > 0 ) {
          error_s += products.shape.errors_to_string();
        } 
  
        if ( !is_valid_product( products )  ) {
          auto residuals = products.shape.residual_config();
          if ( residuals.size() > 0 ) {
            std::string resid_s = residuals.to_json( residuals.options() ).dump(-1);
            std::string mess = "Product config (" + products.config.name() +
                             ") contains unrecognized key/value options: " +
                             resid_s;
            error_s += mess;
          }
        }

        return ( ( ( error_s.length() > 0 ) ? error_s : "None found!" ) );
      }

      
    private:
      PsmrtsTranslations m_translator;

  };

} // namespace psmrts

#endif
