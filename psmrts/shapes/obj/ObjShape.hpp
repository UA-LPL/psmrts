/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ObjShape_hpp
#define ObjShape_hpp

#include <string>

#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductSpecification.hpp>


namespace psmrts  {
  /**
   * @brief OBJ format mesh process class
   * 
   * 
   */
  class ObjShape : public PsmrtsProduct {
    public:
      using ProductInfo     = ProductSpecification::ProductInfo;
      using ProductFeatures = ProductSpecification::ProductFeatures;

      ObjShape( ) : PsmrtsProduct( "none", "obj" ), 
                    m_model(), m_mesh(), m_config("obj" ) { }
      ObjShape( const psmrts::PsmrtsOBJFormat &obj_t ) :
                PsmrtsProduct( obj_t.obj_source(), "obj" ), 
                m_model( obj_t ), m_mesh( obj_t.get_mesh() ),
                m_config( obj_t.get_config()) { 
        m_config.add_metadata( ProductOption( "minimum_radius", m_mesh.minimum_radius() ) );
        m_config.add_metadata( ProductOption( "maximum_radius", m_mesh.maximum_radius() ) );
      }
      ObjShape( const std::string &obj_file ) :
                PsmrtsProduct( obj_file, "obj" ), 
                m_model( obj_file ), m_mesh( m_model.get_mesh() ),
                m_config( m_model.get_config() ) { 
        m_config.add_metadata( ProductOption( "minimum_radius", m_mesh.minimum_radius() ) );
        m_config.add_metadata( ProductOption( "maximum_radius", m_mesh.maximum_radius() ) );
      }
      ObjShape( const ProductConfiguration &config,
                const PsmrtsTranslations &trans = PsmrtsTranslations::create() ) {
        this->create( config, trans );
      }      
      virtual ~ObjShape() { }


      /**
       * @brief OBJ Facet Processor
       * 
       * This method accepts a PRQFacet, usually instantiated with a ray
       * trace, and processes it - storing the facet associated with the 
       * trace's intercept back into the PRQFacet. The resulting facet
       * can be accessed using the PRQFacet's facet() function.
       * 
       * It returns true if the process results in a valid facet.
       * 
       * @param facet   PRQFacet provides desired ray trace, and stores
       *                  resulting facet data
       * @return true   If process results in valid facet intercept
       * @return false  If process fails to find facet/intercept
       
      inline bool process( PRQFacet &facet ) const {
         return ( m_model.get_facet( facet.trace(), facet.facet() ) );
      }
        */


      /**
       * @brief OBJ Features Processor
       * 
       * This method accepts a PRQFeatures, and stores into it all the 
       * relevant Bullet information using JSON.
       * 
       * @param features PRQFeatures that holds tracer-relevant information
       *                  in a JSON format
       * @return true    If features were added successfully
       * @return false   If any issues during processing
       */
      inline bool process( PRQFeatures &features ) const {
        features.add_feature( this->product_specifications().to_json() );
        return ( true );
      }
        

      static inline ProductSpecification product_specifications() {
        ProductInfo  info( "obj", { 
                                 ProductOption( "name", "obj"),
                                 ProductOption( "product", "shape"),
                                 ProductOption( "description", "Reads Wavefront OBJ mesh files and creates a PMRTS mesh object") } );
        ProductFeature product( "shape", {
                                ProductOption( "name", "shape" ),
                                ProductOption( "type", "string" ),
                                ProductOption( "description", "Describe the product type" ),
                                ProductOption( "status", "optional" ),
                                ProductOption( "default", "obj" ),
                                ProductOption( "valid", "obj" ) } );                                  
        ProductFeature ofile( "obj_file", {
                                 ProductOption( "name", "obj_file"),
                                 ProductOption( "type", "file"),
                                 ProductOption( "description", "Name of OBJ file/string to read"),
                                 ProductOption( "status", "required"),
                                 ProductOption( "aliases", {"file", "filename", "obj_mesh", "mesh_file", "shapefile"} ),
                                 ProductOption( "file_suffixes", { "obj", "OBJ" } ) } );
        ProductFeature ostr( "obj_string", {
                                 ProductOption( "name", "obj_string"),
                                 ProductOption( "type", "string"),
                                 ProductOption( "description", "Format-compatible string containing contents of an OBJ file" ),
                                 ProductOption( "status", "optional"),
                                 ProductOption( "aliases", "obj_mesh_string" ) } );
        ProductFeature dtype( "obj_data_type", {
                                 ProductOption( "name", "obj_data_type"),
                                 ProductOption( "type", "string"),
                                 ProductOption( "description", "Type of mesh vector data requested/read"),
                                 ProductOption( "status", "optional"),
                                 ProductOption( "aliases", { "data_type", "mesh_data_type" } ), 
                                 ProductOption( "valid", { "double", "float"} ),
                                 ProductOption( "default", "double" ) } );
        ProductFeature mtld( "obj_mtl_search_path", {
                                 ProductOption( "name", "obj_mtl_search_path"),
                                 ProductOption( "type", "directory"),
                                 ProductOption( "description", "Directory path to OBJ materials files"),
                                 ProductOption( "status", "optional"),
                                 ProductOption( "aliases", { "obj_materials_dir", "obj_materials_directory" } ) } );


        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( info, { product, ofile, ostr, dtype, mtld } ) );        
      }      

      inline const PsmrtsMeshData &get_mesh() const {
        return m_mesh;
      }

      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      inline bool matches( const ProductConfiguration &conf ) const {
        return ( this->config().matches( conf ) );
      }

      /** Report all remaining features not available */
      PSMRTS_PROCESS_CATCHALL( "ObjShape" )


    protected:
      psmrts::PsmrtsOBJFormat m_model; // Move to .cpp, WIP
      psmrts::PsmrtsMeshData  m_mesh;
      ProductConfiguration    m_config;

      inline void create( const ProductConfiguration &config,
                          const PsmrtsTranslations &trans ) {

        // Check for valid shape type
        ProductOrder order = this->product_specifications().process_order( config, trans );
        if (order.error_count() > 0 ) {
          std::string mess = "ObjShape::create(" + config.name() + ") has errors: " +
                              order.errors_to_string();
          throw std::runtime_error( mess );          
        }

        m_config = order.config();
        if ( m_config.contains( "shape" ) ) {
          if ( m_config.find( "shape" ).to_string() != "obj" ) {
            std::string mess = "ObjShape::create() - shape type must be \"obj\""
                               " but found " + m_config.find("shape").to_string();
            throw std::runtime_error( mess );
          }
        }

        // Check for conflicts
        if ( m_config.contains( "obj_file" ) && m_config.contains( "obj_string" ) ) {
          std::string mess = "ObjShape::create() - only one of obj_file or obj_string is allowed!";
          throw std::runtime_error( mess );
        }

        // Collect and create construction variables
        std::string text_q;
        std::string objfile;
        std::string materials_path; 
        if ( m_config.contains( "obj_mtl_search_path" ) ) {
          materials_path  = m_config.find( "obj_mtl_search_path" ).to_string();
          if ( m_config.metadata().contains( "obj_mtl_search_path_expanded" ) ) {
            materials_path = m_config.metadata().find( "obj_mtl_search_path_expanded" ).to_string();
          }
        }   
        
        
        // Check for obj_file
        if ( m_config.contains( "obj_file" ) ) {
          objfile  = m_config.find( "obj_file" ).to_string();
          if ( m_config.metadata().contains( "obj_file_expanded" ) ) {
            objfile =  m_config.metadata().find( "obj_file_expanded" ).to_string();
          }
          m_model = PsmrtsOBJFormat( objfile, materials_path );
        }
        else if ( m_config.contains( "obj_string" ) ) {
          text_q = m_config.find( "obj_string" ).to_string();
          m_model = PsmrtsOBJFormat( PsmrtsOBJFormat::load_obj_string( text_q, materials_path,
                                                      PsmrtsOBJFormat::obj_config( materials_path ) ),
                                                      "obj_string" );
        }

        if ( m_config.contains( "obj_data_type") && 
             ( m_config.find( "obj_data_type" ).to_string() == "float" ) ) {
          m_mesh = PsmrtsMeshData( m_model.get_indexes(), m_model.get_float_vectors() );
        }
        else {
          m_mesh = PsmrtsMeshData( m_model.get_indexes(), m_model.get_double_vectors() );
        }

        m_config = m_model.get_config();
        m_config.add_metadata( ProductOption( "minimum_radius", m_mesh.minimum_radius() ) );
        m_config.add_metadata( ProductOption( "maximum_radius", m_mesh.maximum_radius() ) );           

        return;
      }
  };

} // namespace psmrts

#endif
