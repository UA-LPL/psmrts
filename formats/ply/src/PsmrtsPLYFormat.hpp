#ifndef PsmrtsPLYFormat_hpp
#define PsmrtsPLYFormat_hpp

#include <string>
#include <memory>
#include <exception>
#include <iostream>
#include <fstream>

#include <cstdio>
#include <cstring>
#include <string>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsBuffer.hpp>
#include <PsmrtsVector3.hpp>
#include <PsmrtsMeshData.hpp>
#include <PsmrtsParameters.hpp>
#include <ProductSpecification.hpp>

#include <miniply.h>


namespace psmrts {
    /**
     * @brief PsmrtsPLYFormat contains tools for PLY file format I/O
     * 
     * Read - only grabs header information
     * load - obtains the data in the file, and places it into a mesh
     *  - only gets vertex (x, y, z) values, and associated indexes
     *  - grabbing other element data is not supported YET
     * 
     * For binary ply files, load prefer_double is default to false,
     * Must set prefer_double settting to true for .txt ply
     * 
     * @author Kris J. Becker, and Kyle A. Becker, University of Arizona
     * @history 2024-06-21
     */
    class PsmrtsPLYFormat {
        public:

        /** Default Constructor */
        PsmrtsPLYFormat() : m_ply_source(), m_file_type("unknown"), 
                            m_mesh(), m_tracker(), m_config() {}

        /** Construct a ply_file object */
        PsmrtsPLYFormat( const std::string &plyfile )  {
            (void) load_ply_file( plyfile );
        }

        /** Destructor */
        virtual ~PsmrtsPLYFormat() {}

        /** Returns the data source of the format model - PLY filepath */
        virtual std::string format_model_source() {
            return ( ply_source() );
        }

        /** Returns validity state of the object's mesh */
        inline bool isValid() const {
            if ( !m_mesh.isValid() ) return (false);
            return ( true );
        }

        /** The PLY data source */
        inline const std::string &ply_source() const {
            return ( m_ply_source );
        }

        /** Returns the type of the referred PLY file - ie. binary or text */
        inline std::string file_type() const {
          return ( m_file_type );
        }
        
        /** Returns number of vertexes */
        inline size_t nVertexes() const {
            return ( m_mesh.nvectors() );
        }

        /** Returns number of indexes */
        inline size_t nIndexes() const {
            return ( m_mesh.nfacets() );
        }

        /**
         * @brief Load header contents of a PLY file - no data!
         * 
         * This method loads the contents ofa PLY file header only.
         * 
         * The input PLY file is closed upon return from this function.
         * 
         * See also load_ply_file()
         * 
         * @param filename 
         * @return miniply::PLYReader *
         */
        inline static miniply::PLYReader *open( const std::string& filename ) {
            std::unique_ptr<miniply::PLYReader> plyreader;
            try {
                plyreader.reset( new miniply::PLYReader( filename.c_str() ) );
                if ( !plyreader->valid()  ) {
                    throw std::runtime_error("Failed to open file: " + filename );
                }
            }
            catch (const std::exception& e) {
                std::string mess = "PsmrtsPLYFormat::open() - failed to open PLY file " + filename;
                mess += "\nError: " + std::string(e.what());
                throw std::runtime_error(mess);
            }
            return ( plyreader.release() );
        };

       
        /**
         * @brief Load a PL file mesh data
         * 
         * This is the main method that will load the triangular mesh from a
         * PLY file. The only elements read here are facets and vertex data.
         * Other elements can be read from this file. See also read_ply_file().
         * 
         * @param plyfile       Name of PLY file to load
         * @return true         If the load was successful
         * @return false        If the load failed
         */
        inline bool load_ply_file( const std::string &plyfile ) {

            // Init section
            m_ply_source = plyfile;
            m_file_type  = "unknown";
            m_mesh       = PsmrtsMeshData();  // Resets the state
            m_tracker    = PsmrtsThreadSafeCounter();

            // Open the file
            miniply::PLYReader reader( plyfile.c_str() );
            if ( !reader.valid()  ) {
                throw std::runtime_error("Failed to open file: " + plyfile );
            }
            
            // Determine the file type
            if ( reader.file_type() == miniply::PLYFileType::ASCII ) {
                m_file_type = "ascii";
            }
            else {
                m_file_type = "binary";
            }

            // Config header data to json
            parse_config(reader);

            // Lets assume only triangles in the ply, which is more efficient to read here. See
            // https://github.com/vilya/miniply#loading-from-a-ply-file-known-to-only-contain-triangles
            uint32_t faceIdxs[3];
            miniply::PLYElement* faceElem = reader.get_element( reader.find_element(miniply::kPLYFaceElement ) );
            if (faceElem == nullptr) {
                throw std::runtime_error( "Unable to find PLY Face element in " + plyfile );
            }

            // Convert list to a fixed size list...
            faceElem->convert_list_to_fixed_size(faceElem->find_property("vertex_indices"), 3, faceIdxs);

            uint32_t indexes[3];

            PsmrtsVector3d p_vectors;
            PsmrtsVector3i p_indexes;
            miniply::PLYPropertyType v_type =  miniply::PLYPropertyType::Double;

            while ( reader.has_element() && (!p_vectors.isValid()|| !p_indexes.isValid() ))  {

                if (reader.element_is(miniply::kPLYVertexElement) && reader.load_element() && reader.find_pos(indexes)) {
                  size_t  nverts  = reader.num_rows();
                  p_vectors = PsmrtsVector3d( nverts );
                  reader.extract_properties(indexes, 3, v_type,  p_vectors(0).data() );
                }
                else if ( !p_indexes.isValid() && reader.element_is(miniply::kPLYFaceElement) && reader.load_element()) {
                  size_t nindexes = reader.num_rows();
                  p_indexes = PsmrtsVector3i( nindexes );
                  reader.extract_properties(faceIdxs, 3, miniply::PLYPropertyType::Int,  p_indexes(0).data() );
                }

                reader.next_element();
            }
            // Check to see if we have the mesh -- throw error if vectors/indexes are invalid
            if ( !( p_vectors.isValid() && p_indexes.isValid() ) ) {
                throw std::runtime_error( "Vectors or Indexes are not valid/present in: " + plyfile);
            }

            // Allocate a mesh now    
            m_mesh = PsmrtsMeshData( p_indexes, p_vectors);

            return ( m_mesh.isValid() );
        }

        /**
         * @brief Returns an ordered JSON containing relative product options
         * and possible values 
         * 
         * @return ordered_json of product options
         */
        static inline ProductSpecification product_options() {
            char text[] = R"(
            {
              "ply_file": "<filename>",
              "ply_file_type": ["binary", "ascii"],
              "ply_data_type": ["char", "uchar", "short", "ushort", "int", "uint", "float", "double"],
              "required": ["ply_file"],
              "optional": ["ply_file_type", "ply_data_type"]
            }
            )";
            return (ProductSpecification("ply", "mesh", json_utils::parse_json_string( text )));
        }

        /**
         * @brief Conversion of ply file header data to json
         * 
         * Used in load_ply_file() to create a json version of the product related
         * data options
         * 
         * @param reader miniply file reader 
         */
        inline void parse_config( miniply::PLYReader& reader )  {
            ordered_json options;
            options["ply_file"] = m_ply_source;
            options["ply_file_type"] = m_file_type;
            
            int largest_type = 0;
            std::string largest_text = "";
            for (uint32_t i=0; i < reader.num_elements(); i++) {
                const miniply::PLYElement* elem = reader.get_element(i);
                for(const miniply::PLYProperty& prop : elem->properties) {
                    if (property_type_size( prop.type ) > largest_type) {
                        largest_type = property_type_size( prop.type );
                        largest_text = property_type_string( prop.type );
                    }
                }
            }
            options["ply_data_type"] = largest_text;
            m_config = ProductSpecification("ply", "mesh", options);
            return;
        }

        /**
         * @brief String output helper for miniply property types
         * 
         * This method is used to convert the appropriate ply type to a string version
         * when building the json header output for the reader in parse_config().
         * 
         * 
         * @param prop ply property type, as defined by miniply
         * @return std::string 
         */
        inline std::string property_type_string( const miniply::PLYPropertyType prop) const {
            switch(prop) {
                case miniply::PLYPropertyType::Char:
                    return "char";
                case miniply::PLYPropertyType::UChar:
                    return "uchar";
                case miniply::PLYPropertyType::Short:
                    return "short";
                case miniply::PLYPropertyType::UShort:
                    return "ushort";
                case miniply::PLYPropertyType::Int:
                    return "int";
                case miniply::PLYPropertyType::UInt:
                    return "uint";
                case miniply::PLYPropertyType::Float:
                    return "float";
                case miniply::PLYPropertyType::Double:
                    return "double";
                default:
                    return "None";
            }
        }

        /**
         * @brief size determinant helper function for PLY types
         * 
         * @param  prop PLY property
         * @return int based size of PLY data type for property type
         */
        inline int property_type_size( const miniply::PLYPropertyType prop) const {
            switch(prop) {
                case miniply::PLYPropertyType::Char:
                    return sizeof(char);
                case miniply::PLYPropertyType::UChar:
                    return sizeof(unsigned char);
                case miniply::PLYPropertyType::Short:
                    return sizeof(short);
                case miniply::PLYPropertyType::UShort:
                    return sizeof(unsigned short);
                case miniply::PLYPropertyType::Int:
                    return sizeof(int);
                case miniply::PLYPropertyType::UInt:
                    return sizeof(unsigned int);
                case miniply::PLYPropertyType::Float:
                    return sizeof(float);
                case miniply::PLYPropertyType::Double:
                    return sizeof(double);
                default:
                    return 0;
            }
        }
        
        /**
         * @brief returns a PsmrtsPLYFormat object with the provided product option
         * parameters
         * 
         * @param  params 
         * @return PsmrtsPLYFormat 
         */
        static inline PsmrtsMeshData create( const ProductSpecification &params ) {

            try {
                if ( params.has_parameter( "ply_file" ) ) {
                    ProductParameter plyfile = params.get_parameter("ply_file");
                    return ( PsmrtsPLYFormat(  plyfile.value<std::string>( "ply_file" ) ).get_mesh() );
                }
            }
            catch ( const std::runtime_error &re) {
                std::string msg = std::string( "PsmrtsPLYFormat::create() failed - ").append( re.what() );
            }

            // Could return invalid model
            // ... return ( PsmrtsPLYFormat( ) );
            // or throw exception
            std::string msg = std::string( "PsmrtsPLYFormat::create() invalid product request configuration" );
            throw std::runtime_error( msg );
        }

#if 0
        /**
         * @brief returns true if the input product json contains the same
         * values as the object
         * 
         * @param params ProductSpecification PLY config
         * @return true  if params has same values as object
         * @return false if params is empty or has different values
         */
        inline bool compare(const ProductSpecification &params) {
            if (params.name() != m_config.name() || params.type() != m_config.type()) {
                return false;
              }
        
              ordered_json options = m_config.specs().parameters();
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

        /** Returns the header json information
        inline const json &config() const {
            return m_config;
        }*/
        
        inline const ProductSpecification &config() const {
            return m_config;
        }


        /** Returns the mesh as read from the file unless true is provided which returns doubles */
        inline PsmrtsMeshData get_mesh( const bool make_it_a_double = false ) const  {
            if ( true == make_it_a_double ) {
                if ( !m_mesh.isVectorDouble() ) {
                    return ( PsmrtsMeshData( this->get_indexes(), this->get_double_vectors() ) );
                }
            }

            return ( m_mesh );

        }

        /** Get a double precision vector array  */
        inline PsmrtsVector3d get_double_vectors() const {
            PsmrtsVector3d v_vectors = m_mesh.vectors().double_vectors();
            if ( m_mesh.vectors().isFloat() ) {
                v_vectors = vector_to_type<PsmrtsVector3d::value_type>( m_mesh.vectors().float_vectors() );
            }

            return ( v_vectors );
        }
        
        /** Get a float precision vector array  */
        inline PsmrtsVector3f get_float_vectors() const {
            PsmrtsVector3f v_vectors = m_mesh.vectors().float_vectors();
            if ( m_mesh.vectors().isDouble() ) {
                v_vectors = vector_to_type<PsmrtsVector3f::value_type>( m_mesh.vectors().double_vectors() );
            }

            return ( v_vectors );
        }


        /** Get index buffer extracted from PLY file */
        inline PsmrtsVector3i get_indexes() const {
            return ( m_mesh.indexes() );
        }

        inline double elapsed_life_time_s() const {
            return (m_tracker.runtime_s() );
        }

        inline size_t track_count() const {
            return (m_tracker.count() );
        }

        /**
         * @brief Return a standalone clone of the currrent trackerr stats
         * 
         * Get a snapshot of the performance at this moment. I'd immediately get
         * an end_time = system_clock_time
         * 
         * @return PsmrtsThreadSafeCounter
         */
        inline PsmrtsThreadSafeCounter performance_snapshot() {
            return ( m_tracker.clone() );
        }

        private:
            std::string                m_ply_source;
            std::string                m_file_type;
            PsmrtsMeshData             m_mesh;
            PsmrtsThreadSafeCounter    m_tracker;
            ProductSpecification       m_config;

    };
} // namespace psmrts

#endif // PsmrtsPLYFormat_hpp
