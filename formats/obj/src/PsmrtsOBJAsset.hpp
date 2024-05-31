#ifndef PsmrtsOBJAsset_hpp
#define PsmrtsOBJAsset_hpp

#include <string>
#include <memory>
#include <exception>
#include <iostream>

#include <Eigen/Geometry>

#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>
#include <RayTrace.hpp>

// See PsmrtsOBJImplementation.hpp for defining the tinyobj implemantion in your main
#include "tiny_obj_loader.h"
namespace psmrts {
  /**
   * @brief PsmrtsOBJAsset contains tools for OBJ file format I/O
   *
   * @author Kris J. Becker, University of Arizona
   * @history 2024-05-07 Kris J. Becker  Original Version
   */
  class PsmrtsOBJAsset {
    public:
      typedef PsmrtsDataModel<tinyobj::real_t>     ObjVectorData;
      typedef PsmrtsDataModel<int>                 ObjIndexData;

      typedef  ObjVectorData::vector_type          ObjVectorType;
      typedef  ObjIndexData::vector_type           ObjIndexType;

      /** Default constructor */
      PsmrtsOBJAsset() : m_obj_source(), m_obj_config(), m_obj_reader() { }

      /** Construct an array of values */
      PsmrtsOBJAsset( const std::string &objfile,
                      const std::string &mtlpath = ""  ) {

        m_obj_source = objfile;
        m_obj_config = this->obj_config( mtlpath );
        m_obj_reader.reset( this->load_obj_file( objfile, m_obj_config ) );

        const bool ThrowOnError = true;
        check_obj_errors( "*** PsmrtsOBJAsset(objfile)", ThrowOnError );
      }

      /** Use this one derived from a string */
      PsmrtsOBJAsset( tinyobj::ObjReader *obj_reader,
                      const std::string &source ) {
        
        m_obj_source = source;
        m_obj_config = tinyobj::ObjReaderConfig();
        m_obj_reader.reset( obj_reader );

        const bool ThrowOnError = true;
        check_obj_errors( "*** PsmrtsOBJAsset(ObjReader)", ThrowOnError );
      }      
      
      /** Destructor */
      virtual ~PsmrtsOBJAsset() { }


      /** Valid if an OBJ reader is allocated and its contents are valid */
      inline bool isValid() const {
        if ( !m_obj_reader )          return ( false );
        if ( !m_obj_reader->Valid() ) return ( false );
        return ( true );
      }

      /** The OBJ data source */
      inline const std::string &obj_source() const {
        return ( m_obj_source );
      }

      /** OBJs can have more than one shape in a file */
      inline size_t nShapes() const {
        if ( !this->isValid() ) return ( 0 );
        return ( m_obj_reader->GetShapes().size() );
      }

      /** Indexes are local to each shape into a largerer array */
      inline size_t nIndexes( const size_t nthShape = 0 ) const {
        if ( nthShape >= nShapes() ) return ( 0 );
        return ( m_obj_reader->GetShapes()[nthShape].mesh.num_face_vertices.size() );
      }

      /* Return count of all facets in all shapes */
      inline size_t shape_facet_count(  ) const {
        if ( !this->isValid() ) return ( 0 );
        return ( count_facets() );
      }

      /** A single buffer of verticies that are 0-based */
      inline size_t nVertexes( ) const {
        if ( !this->isValid() ) return ( 0 );
        return ( m_obj_reader->GetAttrib().vertices.size() / 3 );
      }

      /** Any materials that were loaded */
      inline size_t nMaterials() const {
        if ( !this->isValid() ) return ( 0 );
        return ( m_obj_reader->GetMaterials().size() );
      }      

      /** Return pointer to OBJ reader object - could be nullptr */
      inline const tinyobj::ObjReader *shape() const {
        // Use to access the data within the reader...
        return ( m_obj_reader.get() );
      }
      
      /** Return reference to OBJ config object */
      inline const tinyobj::ObjReaderConfig &config() const {
        return ( m_obj_config );
      }      

      /**
       * @brief Provide a default ObjReaderConfig structure for this class
       * 
       * The object oriented version of tinyObjReader provides a config
       * structure to specify load actions. This method provides defaults
       * for this class.
       * 
       * This method will set the path to materials if provided. It also
       * opts to perform triangulations on >3 facet indexes and will not
       * load vertex colors.
       * 
       * Callers can get this copy and alter as desired before calling
       * load_obj_file() or load_obj_string().
       * 
       * @param mtl_path  Optional path to materials files
       * 
       * @return tinyobj::ObjReaderConfig OBJ config options
       */
      static inline tinyobj::ObjReaderConfig obj_config( const std::string &mtl_path = "" ) {

        tinyobj::ObjReaderConfig      t_config;

        t_config.triangulate          = true;
        t_config.triangulation_method = "simple";
        t_config.vertex_color         = false;
        t_config.mtl_search_path      = mtl_path;

        return ( t_config );
      }


      /**
       * @brief Load the contents of a Wavefront OBJ file
       * 
       * This method loads the contents of a file to that contains data
       * compatible with the OBJ file format. Please see
       * http://www.martinreddy.net/gfx/3d/OBJ.spec.
       * 
       * A pointer to a tinyobj::ObjReader object that contains the
       * contents of "objfle" parameter. No error checking is done
       * after the read attempt, so callers should check the
       * status of the read using check_obj_errors().
       * 
       * @param objfile  Name of file to read/parse
       * @param objconf  OBJ reader configuration object
       * @return tinyobj::ObjReader* Returns a pointer to the contents of the
       *                               file. A nullptr is returned if the object
       *                               cannot be allocated. 
       */
      static inline tinyobj::ObjReader *load_obj_file( const std::string &objfile, 
                                                       const tinyobj::ObjReaderConfig &objconf = tinyobj::ObjReaderConfig() ) {
        
        std::unique_ptr<tinyobj::ObjReader> t_reader( new tinyobj::ObjReader );
        if ( !t_reader ) return ( nullptr );

        // Read and return. Reader must assess condition. See check_for_errors().
        t_reader->ParseFromFile( objfile, objconf );

        return ( t_reader.release() );
      }
      

      /**
       * @brief Parse the contents of strings containing and OBJ specification
       * 
       * This method accepts a string containing an OBJ format specification.
       * This implies that the whole of a file containing OBJ data can be
       * passed in the "obj_text" string that will result in a valid triangle
       * mesh.
       * 
       * @param obj_text Text string containing an OBJ specification
       * @param mtl_text Text string containing the materials data
       * @param objconf  An OBJ configuration to use during parsing
       * @return tinyobj::ObjReader* Returns a pointer to the contents of the
       *                               string are parsed. A nullptr is
       *                               returned if the object cannot be
       *                               allocated.
       */
      static inline tinyobj::ObjReader *load_obj_string( const std::string &obj_text, 
                                                         const std::string &mtl_text = "", 
                                                         const tinyobj::ObjReaderConfig &objconf = tinyobj::ObjReaderConfig() ) {
        
        std::unique_ptr<tinyobj::ObjReader> t_reader( new tinyobj::ObjReader );
        if ( !t_reader ) return ( nullptr );

        // Read and return. Reader must assess condition. See check_obj_errors().
        t_reader->ParseFromString( obj_text, mtl_text, objconf );

        return ( t_reader.release() );
      }      


      /**
       * @brief Check validity of an ObjReader after the load operation
       * 
       * This method will check the status of the ObjReader. This is typically
       * needed after the OBJ data is loaded/parsed. 
       * 
       * The error conditions are:
       *   1) There is no ObjReader object allocated within the object
       *   2) There is an error string in the ObjReader error variable
       *   
       * Warnings are also check and reported if a string is detected in
       * the ObjReader warning varible.
       * 
       * The caller can decide to throw errors or return true/false status.
       * 
       * @param src_id_str    Message to prepend to report on errors
       * @param throwOnError  True will throw exceptions when errors are
       *                        detected, otherwise returns status
       * @return true         For a valid state of the ObjReader object
       * @return false        Either an unallocated ObjReader or error
       *                        has occured
       */
      inline bool check_obj_errors( const std::string &src_id_str = "*** PsmrtsOBJAsset::status",
                                    const bool throwOnError = true ) const {

        if ( !m_obj_reader ) {
          std::string msg = src_id_str + " " + m_obj_source;
          if ( throwOnError == true ) throw std::runtime_error( msg );
          return ( false );
        }

        if ( !m_obj_reader->Error().empty() ) {
          std::string msg = src_id_str + " - " + m_obj_reader->Error() + " for file " + m_obj_source;
          if ( throwOnError == true ) throw std::runtime_error( msg );
          return ( false );
        }

        if ( !m_obj_reader->Warning().empty() ) {
          std::cout << src_id_str << "->TinyObjReader::Warning " 
                    << m_obj_reader->Warning() << " for file " << m_obj_source
                    << std::endl;
        }

        return ( true );
      } 


      template <typename T> 
        inline PsmrtsDataModel<T> get_vectors(  ) const {

          typedef typename PsmrtsDataModel<T>::vector_type vector_type;

          ObjVectorData bt_vector_map( &this->shape()->GetAttrib().vertices[0], this->nVertexes() );
          PsmrtsDataModel<T> out_vectors( bt_vector_map.size() );

          for ( size_t i = 0 ; i < bt_vector_map.size() ; i++ ) {
            auto iVec = bt_vector_map( i );
            out_vectors( i ) = vector_type( { iVec[0], iVec[1], iVec[2] } );
          }

          return ( out_vectors );
        }

      template <typename T> 
        inline PsmrtsDataModel<T> get_indexes( ) const {
          typedef typename PsmrtsDataModel<T>::vector_type vector_type;

          PsmrtsDataModel<T> out_indexes( this->count_facets() );

          size_t ondx = 0;
          for ( auto const &shape : m_obj_reader->GetShapes() ) {
            size_t index_offset = 0;

            for ( size_t f = 0 ; f < shape.mesh.num_face_vertices.size() ; f++ ) {
              size_t fv = size_t( shape.mesh.num_face_vertices[f] );
              size_t fv3 = std::min( size_t( 3 ), fv );
              size_t v_ndxs[3] = { 0, 0, 0 };
              for (size_t v = 0; v < fv3 ; v++) {  
                size_t v_index = ( 3 * size_t( shape.mesh.indices[index_offset + v].vertex_index ) );
                v_ndxs[v] = v_index;
              }

              // Set the ondx facet index
              out_indexes( ondx++ ) = vector_type( { v_ndxs[0], v_ndxs[1], v_ndxs[2] } );
              index_offset += fv;
            }
          }
          return ( out_indexes );
        }


    protected:

      inline size_t count_facets( ) const {
        size_t n_facets = 0;
        if ( this->isValid() ) {
          for ( auto const &shape : m_obj_reader->GetShapes() ) {
            n_facets += shape.mesh.num_face_vertices.size();
          }
        }
        return ( n_facets );
      }


      inline size_t count_facet_indexes( ) const {
        size_t n_indexes = 0;
        if ( this->isValid() ) {
          for ( auto const &shape : m_obj_reader->GetShapes() ) {
            n_indexes += shape.mesh.indices.size();
          }
        }
        return ( n_indexes );
      }

    private:
      std::string                          m_obj_source;
      tinyobj::ObjReaderConfig             m_obj_config;
      std::shared_ptr<tinyobj::ObjReader>  m_obj_reader;
  };
}  // namespace psmrts

#endif // PsmrtsOBJAsset_hpp
