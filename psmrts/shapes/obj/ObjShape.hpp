#ifndef ObjShape_hpp
#define ObjShape_hpp

#include <string>

#include <psmrts/shapes/obj/private/PsmrtsOBJFormat.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/ProductSpecification.hpp>

namespace psmrts  {
  /**
   * @brief OBJ format mesh process class
   * 
   * 
   */
  class ObjShape : public PsmrtsProduct {
    public:
     ObjShape( ) : PsmrtsProduct( "none", "obj" ), 
                   m_model(), m_mesh() { }
     ObjShape( const psmrts::PsmrtsOBJFormat &obj_t ) :
               PsmrtsProduct( obj_t.obj_source(), "obj" ), 
               m_model( obj_t ), m_mesh( obj_t.get_mesh() ) { }
     ObjShape( const std::string &obj_file ) :
               PsmrtsProduct( obj_file, "obj" ), 
               m_model( obj_file ), m_mesh( m_model.get_mesh() ) { }
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
       * @brief Bullet Features Processor
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
        features.add_feature( this->product_specifications().json_specs() );
        return ( true );
      }
        

      static inline ProductSpecification product_specifications() {
        char text[] = R"(
        {
          "name": "obj",
          "product": "shape",
          "type": "mesh",
          "description": "Reads Wavefront OBJ mesh files and creates a PMRTS mesh object",
          "driver": {
            "name": "obj",
            "type": "system",
            "aliases": [ "OBJ" ]
          },
          "features": [
            {
              "name": "obj_file",
              "type": "file",
              "description": "Name of OBJ file to read",
              "status": "required",
              "aliases": ["file", "obj_mesh", "mesh_file"],
              "file_suffixes": [ "obj", "OBJ" ]
            },
            {
              "name": "obj_string",
              "type": "string",
              "description": "Format-compatible string containing contents of an OBJ file",
              "status": "optional",
              "aliases": ["obj_mesh_string"]
            },            
            {
              "name": "obj_data_type",
              "type": "string",
              "description": "Type of mesh vector data requested/read",
              "status": "optional",
              "aliases": ["mesh_data_type"],
              "valid": ["double","float"],
              "default": "double"
            },         
            {
              "name": "obj_mtl_search_path",
              "type": "directory",
              "description": "Directory path to OBJ materials files",
              "status": "optional",
              "aliases": ["obj_materials_dir"]
            }   
          ]       
        } )";

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( "obj", "mesh", "shape", json_utils::parse_json_string( text )));
      }      

      inline const PsmrtsMeshData &get_mesh() const {
        return m_mesh;
      }

      /** Report all remaining features not available */
      PSMRTS_PROCESS_CATCHALL( "ObjShape" )


    protected:
      psmrts::PsmrtsOBJFormat m_model;
      psmrts::PsmrtsMeshData m_mesh;
  };

} // namespace psmrts

#endif
