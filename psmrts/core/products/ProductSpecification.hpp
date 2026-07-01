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
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductFeature.hpp>


namespace psmrts { 

  namespace json_p = psmrts::json_utils;


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
      using ResidualList     = PsmrtsContainer<ProductOption>;
      using Creator = std::function<void(const ProductConfiguration &config)>;


      ProductSpecification( ) : m_name( "none" ), m_product( "none" ),
                                m_info( "info" ), m_features( "features" ),
                                m_creator( std::nullopt ) { }
      ProductSpecification( const std::string &name,
                            const std::string &product ) :
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

      /** Returns the name of the product specification */
      inline const std::string &name() const {
        return ( m_name );
      }
      
      /** Returns the name of the product */
      inline const std::string &product() const {
        return ( m_product );
      }

      /** Returns the number of feature options in this spec */
      inline size_t size() const {
        return ( m_features.size() );
      }

      /** Returns the product information section */
      inline const ProductInfo &info() const {
        return ( m_info );
      }

      /** Returns the list if product features in this spec */
      inline const ProductFeatures &features() const {
        return ( m_features );
      }

      /** Returns the infomation key names in this spec */
      inline std::vector<std::string> info_keys() const {
        return ( m_info.keys() );
      }

      /** Returns a list of all feature names in this spec */
      inline std::vector<std::string> feature_names() const {
        return ( m_features.keys() );
      }

      /** Adds a option to the info section of the spec */
      inline void add_info( const ProductOption &option ) {
        m_info.replace( option );
      }

      /** Adds a product feature option */
      inline void add_feature( const ProductFeature &feature ) {
        m_features.replace( feature );
      }

      /** Checks for the existance of a feature by name */
      inline bool contains( const std::string &name ) const {
        return ( m_features.contains( name ) );
      }

      /** Finds/returns an existing feature option by name */
      inline const ProductFeature &find( const std::string &name ) const {
        return ( m_features.find( name ) );
      }      

      /** Returns the set of required feature names in this spec */
      inline std::vector<std::string> required() const {
        std::vector<std::string> keys;
        for ( const auto &f : this->features() ) {
          if ( f.is_required() ) keys.push_back( f.name() );
        }
        return ( keys );
      }

      /** Returns the set of dependency options in this spec */
      inline std::vector<std::string> dependency() const {
        std::vector<std::string> keys;
        for ( const auto &f : this->features() ) {
          if ( f.is_dependency() ) keys.push_back( f.name() );
        }
        return ( keys );
      }

      /** Returns the set of optional feature names in this spec */
      inline std::vector<std::string> optional() const {
        std::vector<std::string> keys;
        for ( const auto &f : this->features() ) {
          if ( f.is_optional() ) keys.push_back( f.name() );
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
       * @param default_a    Default to return if aliases do not exist
       * @return std::string If an alias name exits, the real name of the
       *                       feature is returned. If no alias exists, an empty
       *                       string is returned.
       */
      inline std::string get_alias_feature_name( const std::string &name,
                                                 const std::string &default_a = "" ) const {
        for ( const auto &f : this->features() ) {
          if ( f.isa_alias( name ) ) return ( f.name() );
        }
        return ( default_a );        
      }

      /**
       * @brief Find an aliased option for the given feature
       * 
       * This method will search through a product configuration for the first
       * alias names in the feature given. If the config contains an option with
       * a feature name, the name of the option is returned.
       * 
       * @param feature   Feature containing aliases to search for
       * @param config    Product configuration that may contain a feature alias
       *                    product option
       * @return std::string Returns the name of the first feature alias name
       *                      found in the config or a empty string if one is
       *                      not found
       */
      inline std::string find_feature_option_name( const ProductFeature &feature,
                                                   const ProductConfiguration &config ) 
                                                   const {

        // Check for feature name before checking aliases
        if ( config.contains ( feature.name() ) ) return ( feature.name() );

        // Search for feature aliases in config
        for ( const auto &alias : feature.aliases() ) {
          if ( config.contains( alias ) ) return ( alias );
        }

        // Not feature option not in config
        return ( "" );        
      }


      /** Return the JSON specification */
      inline ordered_json to_json() const {

        ordered_json j_info = {};
        for ( const auto &j_data : info().data()  ) {
          j_info.update( j_data.to_json() );
        }

        ordered_json j_features = ordered_json::array();
        for ( const auto &j_feature : features().data()  ) {
          j_features.push_back( j_feature.to_json() );
        } 
        
        ordered_json j;
        j.update( json_p::insert_object( "info",  j_info ) );
        j.update( json_p::insert_object( "features", j_features ) );
        return ( j );
      }

      /**
       * @brief Check the feature for a valid option
       * 
       * Searches the "valid" specification data and compares it with the given
       * option. 
       * 
       * @param option    Option value to check against feature valid list
       * @param feature   Feature to check valid list in option value
       * @param validator Error logger to report invalid conditions
       * @return true     True if the option value is valid or the feature does
       *                   not contain a valid key in its spec 
       * @return false    If the option value is not valid in the feature
       */
      inline bool valid_option_with_feature( const ProductOption &option,
                                             const ProductFeature &feature,
                                             PsmrtsRequest &validator ) const {

        bool is_good = true;
        if ( feature.contains( "valid" ) ) {
          bool is_valid = psmrts_contains_string( option.to_string(), feature.valid_list() );
          if ( !is_valid ) {
            std::string mess = "ProductSpecification::validate option (" +
                               option.name() + ") value is not valid."
                               " Value: " + option.to_string() + ", Expected: " +
                               feature.find("valid").to_string();
            validator.add_error( std::runtime_error( mess ) );
            is_good = false;
          }
        }

        return ( is_good );
      }

      /**
       * @brief Determines if the option contains a valid option
       * 
       * Search through all product specifications feature names that may match
       * the option name or an alias and validate its value.
       * 
       * @param option    Option to validate
       * @param validator Error logger
       * @return true     True if this spec contains a feature option that is
       *                    validated against the option value. 
       * @return false    False if the spec does not contain the option name or
       *                    alias or the values in the option are invalid. 
       */
      inline bool validate_option_default( const ProductOption &option,
                                           PsmrtsRequest &validator ) const {
        bool is_good = true;
        const std::string name_t = option.name();
        const std::string alias_name = this->get_alias_feature_name( name_t, name_t );

        ProductOption option_a( alias_name, option );
        if ( this->contains( alias_name ) ) {
          const ProductFeature &feature_t = this->find( alias_name );
          if ( feature_t.contains( "default" ) ) {
            const ProductOption &default_t = feature_t.find( "default" );
            if ( default_t.size() != option_a.size() ) {
              validator.add_error( name_t + " (" + alias_name + ") feature default not same size in specs." );
              is_good = false;
            }

            // Compare the options
            if ( ProductOption::StringsComparator::compare( option_a,  default_t) == false ) {
              validator.add_error( name_t + " (" + alias_name + ") does not compare with feature default." );
              is_good = false;
            }

          }
          else {
            validator.add_error( name_t + " (" + alias_name + ") feature default does not exist in specs." );
            is_good = false;
          }
        }
        else { 
          validator.add_error( name_t + " (" + alias_name + ") feature does not exist in specs." );
          is_good = false;
        }
              
        return ( is_good );
      }
      
      /**
       * @brief Compares/extracts/validates a configuration with this spec
       * 
       * Given a product configuration, the contents of it are compared with
       * this specification for valid content and values with expanding if
       * applicable (e.g., file names).
       * 
       * Residuals returned indicating the option is not part of the spec and is
       * likely invalid within the provided config. 
       * 
       * @param config    Product configuration to compare with this spec
       * @param residuals A list if residual options that are not in this spec
       * @param validator Error logger for this operation
       * @return ProductConfiguration Valid/translated product config extracted
       *          from the parameter config and content of this spec 
       */
      inline ProductConfiguration extract( const ProductConfiguration &config,
                                           ResidualList &residuals, 
                                           PsmrtsRequest &validator ) const {
        ProductConfiguration config_t( this->name() );
        std::vector<std::string> required_list;   

        for ( const auto &option : config.options() ) {
          std::string option_name_t = option.name();
          std::string f_name = this->get_alias_feature_name( option_name_t ); 

          if ( this->contains( option_name_t ) || this->contains( f_name ) ) {
            if ( f_name.length() == 0 ) f_name = option_name_t;

            const ProductFeature &feature = this->find( f_name );
            if ( feature.is_required() ) required_list.push_back( f_name );

            if ( feature.is_dependency() ) {
              residuals.add( option );
            }
            else {
              ProductOption option_t( f_name, option );  
              config_t.add( option_t );
              this->valid_option_with_feature( option_t, feature, validator );          
              if ( config.metadata().contains( option_name_t+"_expanded" ) ) {
                config_t.add( ProductOption( f_name, config.metadata().find( option_name_t+"_expanded" ) ) );
              }
            }
          }
          else {
            residuals.add( option );
          }
        }

        // Now check to see if all required keywords are present
        for ( const auto &key_r : this->required() ) {
          if ( std::find(required_list.begin(), required_list.end(), key_r ) == required_list.end() ) {
            std::string mess = "*** ProductSpecifications::extract(" + config.name() + ")"
                              "- required feature key " + key_r  + 
                              " is not present in " + this->name() + " specification";
            validator.add_error( std::runtime_error( mess ) );
          }
        }

        return ( config_t );
      }

      /** Adds a generic constructor that may be used to create the product */
      inline void add_creator( const Creator &creator ) {
        m_creator = creator;
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
