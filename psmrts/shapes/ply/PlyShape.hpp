/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PlyShape_hpp
#define PlyShape_hpp

#include <string>

#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>

namespace psmrts {
    /**
     * @brief PLY format mesh process class
     * 
     */
  class PlyShape : public PsmrtsProduct {
    public:
      using ProductInfo     = ProductSpecification::ProductInfo;
      using ProductFeatures = ProductSpecification::ProductFeatures;
                
      PlyShape() : PsmrtsProduct( "none", "shape", "ply"), 
                  m_mesh(),
                  m_config( "ply" ) { }
      PlyShape( const std::string &ply_file );
      PlyShape( const ProductCart &processed_cart );
      virtual ~PlyShape() = default;
      
      /**
      * @brief PRQFeatures holding Format-relevant specification data
      *  - Possibly needs removal
      * @param features 
      * @return true 
      * @return false 
      */
      inline bool process( PRQFeatures &features ) const {
        features.add_feature( this->product_specifications().to_json() );
        return ( true );
      }
      

      static inline ProductSpecification product_specifications() {
        ProductInfo  info( "ply", { 
                      ProductOption( "name", "ply"),
                      ProductOption( "product", "shape"),
                      ProductOption( "description", "Reads ASCII and Binary PLY mesh files and creates a PSMRTS mesh object") } );
        ProductFeature product( "shape", {
                                ProductOption( "name", "shape" ),
                                ProductOption( "type", "string" ),
                                ProductOption( "description", "Describe the product type" ),
                                ProductOption( "status", "optional" ),
                                ProductOption( "default", "ply" ),
                                ProductOption( "valid", "ply" ) } );                          
        ProductFeature pfile( "ply_file", {
                                ProductOption( "name", "ply_file"),
                                ProductOption( "type", "file"),
                                ProductOption( "description", "Name of PLY file to read"),
                                ProductOption( "status", "required"),
                                ProductOption( "aliases", { "file", "filename", "ply_mesh", "mesh_file", "shapefile" } ),
                                ProductOption( "file_suffixes", { "ply", "PLY" } ) } );
        ProductFeature dtype( "ply_data_type", {
                                ProductOption( "name", "ply_data_type"),
                                ProductOption( "type", "string"),
                                ProductOption( "description", "Type of mesh vector data requested/read" ),
                                ProductOption( "status", "optional"),
                                ProductOption( "aliases", { "data_type", "mesh_data_type" } ), 
                                ProductOption( "valid", { "double", "float" } ),
                                ProductOption( "default", "double" ) });

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( info, { product, pfile, dtype } ) );                                             
      }

        inline const PsmrtsMeshData &get_mesh() const {
          return m_mesh;
        }

        inline const ProductConfiguration &config() const {
          return m_config;
        }

      inline bool matches( const ProductConfiguration &conf ) const {
        return ( this->config().matches( conf ) );
      }

        PSMRTS_PROCESS_CATCHALL( "PlyShape" )

      private:
        PsmrtsMeshData m_mesh;
        ProductConfiguration m_config;

      void create( const ProductCart &cart );        
    };

} // namespace psmrts

#endif
