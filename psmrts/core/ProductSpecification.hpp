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

#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/ProductFeature.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
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
          if ( !f.isa_alias( name ) ) return ( f.name() );
        }
        return ( "" );        
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

    private:
      std::string     m_name;
      std::string     m_product;
      ProductInfo     m_info;
      ProductFeatures m_features;
      std::optional<Creator> m_creator;
  };

} // namespace psmrts

#endif
