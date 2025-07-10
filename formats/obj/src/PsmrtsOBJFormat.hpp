#ifndef PsmrtsOBJFormat_hpp
#define PsmrtsOBJFormat_hpp

#include <string>
#include <memory>
#include <exception>
#include <iostream>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsBuffer.hpp>
#include <PsmrtsParameters.hpp>
#include <PsmrtsVector3.hpp>
#include <PsmrtsMeshData.hpp>
#include <ProductSpecification.hpp>

// See PsmrtsOBJImplementation.hpp for defining the tinyobj implemantion in your main
#include "tiny_obj_loader.h"
namespace psmrts {
  /**
   * @brief PsmrtsOBJFormat contains tools for OBJ file format I/O
   *
   * @author Kris J. Becker, University of Arizona
   * @history 2024-05-07 Kris J. Becker  Original Version
   */
  class PsmrtsOBJFormat {
    public:
      typedef tinyobj::real_t                       tinyobj_real_type;
      typedef PsmrtsVector3<tinyobj::real_t>        OBJVectorArray;
      typedef PsmrtsVector3i                        OBJIndexArray;

      typedef  OBJVectorArray::vector_type          OBJVectorType;
      typedef  OBJIndexArray::vector_type           OBJIndexType;

      /** Default constructor */
      PsmrtsOBJFormat() : m_obj_source(),
                          m_obj_config(), 
                          m_obj_reader() { }

      /** Construct an array of values */
      PsmrtsOBJFormat( const std::string &objfile,
                       const std::string &mtlpath = ""  ) : 
                       m_obj_source(),
                       m_obj_config(), 
                       m_obj_reader() {

        m_obj_source = objfile;
        m_obj_config = this->obj_config( mtlpath );
        m_obj_reader.reset( this->load_obj_file( objfile, m_obj_config ) );
        make_config(objfile, m_obj_reader.get());
        // Call a make_config( m_obj_reader.get() );
        // m_config_j = make_config( objfile, m_obj_reader.get() );

        const bool ThrowOnError = true;
        check_obj_errors( "*** PsmrtsOBJFormat(objfile)", ThrowOnError );
      }

      /** Use this one derived from a string */
      PsmrtsOBJFormat( tinyobj::ObjReader *obj_reader,
                      const std::string &source ) {
        
        m_obj_source = source;
        m_obj_config = tinyobj::ObjReaderConfig();
        m_obj_reader.reset( obj_reader );
        make_config(source, m_obj_reader.get());
        // Call a make_config( m_obj_reader.get() );
        // m_config_j = make_config( objfile, m_obj_reader.get() );
                
        const bool ThrowOnError = true;
        check_obj_errors( "*** PsmrtsOBJFormat(ObjReader)", ThrowOnError );
      }      
      
      /** Destructor */
      virtual ~PsmrtsOBJFormat() { }

      virtual std::string format_model_source() const {
        return ( obj_source() );
      }

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
      inline bool check_obj_errors( const std::string &src_id_str = "*** PsmrtsOBJFormat::status",
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

      /** Returns vectors converted to double precision */
      inline PsmrtsVector3d get_double_vectors( ) const {
        tinyobj_real_type *v = const_cast<tinyobj_real_type *> (&m_obj_reader->GetAttrib().vertices[0] );
        OBJVectorArray bt_vector_map( v, this->nVertexes() );
        PsmrtsVector3d out_vectors( bt_vector_map.size() );

        for ( size_t i = 0 ; i < bt_vector_map.size() ; i++ ) {
          auto iVec = bt_vector_map( i );
          out_vectors( i ) =  Eigen::Vector3d( { 
            static_cast<double>(iVec[0]), 
            static_cast<double>(iVec[1]), 
            static_cast<double>(iVec[2]) } );
        }

        return ( out_vectors );
      }

      /** Returns vectors converted to float precision */
      inline PsmrtsVector3f get_float_vectors( ) const {

        tinyobj_real_type *v = const_cast<tinyobj_real_type *> (&m_obj_reader->GetAttrib().vertices[0] );
        OBJVectorArray bt_vector_map( v, this->nVertexes() );
        PsmrtsVector3f out_vectors( bt_vector_map.size() );

        for ( size_t i = 0 ; i < bt_vector_map.size() ; i++ ) {
          auto iVec = bt_vector_map( i );
          out_vectors( i ) = Eigen::Vector3f( {
            static_cast<float>(iVec[0]),
            static_cast<float>(iVec[1]),
            static_cast<float>(iVec[2]) });
        }

        return ( out_vectors );
      }

      /** Returns vector containing index data of object */
      inline PsmrtsVector3i get_indexes( ) const {

        PsmrtsVector3i out_indexes( this->count_facets() );

        size_t ondx = 0;
        for ( auto const &shape : m_obj_reader->GetShapes() ) {
          size_t index_offset = 0;

          for ( size_t f = 0 ; f < shape.mesh.num_face_vertices.size() ; f++ ) {
            size_t fv = size_t( shape.mesh.num_face_vertices[f] );
            size_t fv3 = std::min( size_t( 3 ), fv );
            size_t v_ndxs[3] = { 0, 0, 0 };
            for (size_t v = 0; v < fv3 ; v++) {  
              size_t v_index = ( size_t( shape.mesh.indices[index_offset + v].vertex_index ) ); // 3 * size_t
              v_ndxs[v] = v_index;
            }

            // Set the ondx facet index
            out_indexes( ondx++ ) = Eigen::Vector3i( { 
              static_cast<int>(v_ndxs[0]), 
              static_cast<int>(v_ndxs[1]), 
              static_cast<int>(v_ndxs[2]) } );
            index_offset += fv;
          }
        }
        return ( out_indexes );
      }

      /**
       * @brief Get the index shape map object.
       * 
       * This function accepts a PsmrtsVector3i of indexes, checks to ensure it contains the 
       * same number of data as facets of the associated OBJ file, then creates and outputs
       * a standard vector containing PsmrtsVector3i objects representative of the shapes
       * located in the file. A PsmrtsVector3i input that contains a different ratio of indexes
       * relative to the number of facets in the file will result in a runtime error.
       * 
       * @param indexes                       Requires a PsmrtsVector3i representative of the OBJ file's
       *                                      index data, which should align with the OBJ's number of facets. 
       *                                      The function will throw an error if their counts are different.
       * @return std::vector<PsmrtsVector3i>  Returns a vector of PsmrtsVector3i representing the shapes present
       *                                      in the OBJ, and their relative facets.
       */
      inline std::vector<PsmrtsVector3i> get_index_shape_map( const PsmrtsVector3i &indexes ) const {

        if ( indexes.size() != this->count_facets() ) {
          std::string mess = "Index count in PsmrtsVector3i(" + std::to_string( indexes.size() ) +
                            ") does not match OBJ facet count(" + std::to_string( this->count_facets() ) + ")";
          throw std::runtime_error( "PsmtsOBJFormat::get_index_shape_map() - " + mess );          
        }

        std::vector<PsmrtsVector3i> obj_shape_maps;

        size_t ondx = 0;
        for ( auto const &shape : m_obj_reader->GetShapes() ) {
          size_t n_facets = shape.mesh.num_face_vertices.size();
          obj_shape_maps.push_back( indexes.slice( ondx, n_facets ) );
          ondx += n_facets;
        }

        return ( obj_shape_maps );
      }

      /** Returns mesh of object's index/vector data */
      inline PsmrtsMeshData get_mesh( ) const {
        m_tracker++;  // Track the number of meshes this instance creates
        return ( PsmrtsMeshData( get_indexes(), get_double_vectors() ) );
      }

      inline double elapsed_life_time_s() const {
        return ( m_tracker.runtime_s() );
      }

      inline size_t track_count() const {
        return ( m_tracker.count() );
      }

      /**
       * @brief Return a standalone clone of the current tracker stats
       *  
       * Get a snapshot of the performance at this moment. I'd immediately get
       * an end_time = system_clock_time
       * 
       * @return PsmrtsThreadSafeCounter 
       */
      inline PsmrtsThreadSafeCounter performance_snapshot() const {
        return ( m_tracker.clone() );
      }

      /**
       * @brief returns a PsmrtsOBJFormat mesh with the provided product option
       * parameters
       * 
       * Throws an error if wrong OBJ product specification configuration or
       * data.
       * 
       * @param  params
       * @return Format-relevant Mesh
       */
      static inline PsmrtsMeshData create( const ProductSpecification &params ) {
        try {

          if ( params.has_parameter( "obj_file" ) ) {
            ProductParameter objfile = params.get_parameter("obj_file");
            std::string mtlopt("");
            if ( params.has_parameter( "obj_mtl_search_path" ) ) {
              mtlopt = params.get_parameter("obj_mtl_search_path").value( "obj_mtl_search_path", mtlopt );
            }
            return ( PsmrtsOBJFormat( objfile.value<std::string>( "obj_file" ), mtlopt ).get_mesh() );  
          }
        }
        catch ( const std::runtime_error &re ) {
          std::string msg = std::string( "PsmrtsOBJFormat::create() failed - ").append( re.what() );
        }
        
        // Could return invalid mesh here, or throw exception - defaulting to throw for now
        std::string msg = std::string( "PsmrtsOBJFormat::create() invalid product request configuration" );
        throw std::runtime_error( msg );      
      }
      
    /**
     * @brief sets the objects internal product options based on Objreader results
     * and provided obj file string representation
     * 
     * @param  objfile path/file name of associated objfile
     * @param  reader  reader of above file
     * @return * void 
     */
    inline void make_config(std::string objfile, tinyobj::ObjReader *reader) {
      ordered_json options;
      options["obj_file"] = objfile;
      if (sizeof(tinyobj_real_type) == sizeof(float)) {
        options["obj_data_type"] = "float";
      } else {
        options["obj_data_type"] = "double";
      }
      options["obj_mtl_search_path"] = m_obj_config.mtl_search_path;
      options["required"] = { "obj_file" };
      options["optional"] = { "obj_data_type", "obj_mtl_search_path" };
      m_config_j = ProductSpecification("obj", "mesh", options);
    }
#if 0
    /**
     * @brief returns true if the input ProductSpecification contains the same
     * values as the object
     * 
     * @param params OBJ ProductSpecification config
     * @return true  if params has same values as object
     * @return false if params is empty or has different values
     */
    inline bool compare(const ProductSpecification &params) {
      if (params.name() != m_config_j.name() || params.type() != m_config_j.type()) {
        return false;
      }

      ordered_json options = m_config_j.specs().parameters();
      ordered_json p_options = params.specs().parameters();
      for (auto aspect : options.items()) {
        if (!p_options.contains(aspect.key())) {
          return false;
        }
        if(p_options.at(aspect.key()) != aspect.value()) {
          return false;
        }
      }
      return true;
    }
#endif
    /**
     * @brief Get the Product Config of an OBJ object
     * 
     * (Temporary getter for object-specific production config speccs)
     * 
     * @return OBJ Object-specific ProductSpecification
     */
    inline ProductSpecification getProductConfig() {
      return m_config_j;
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
      ProductSpecification                 m_config_j;
      PsmrtsThreadSafeCounter              m_tracker;     // Tracks times and copy counts

  };
}  // namespace psmrts

#endif // PsmrtsOBJFormat_hpp
