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
     * @author Kyle A. Becker, University of Arizona
     * @history 2024-06-21
     */
    class PsmrtsPLYFormat {
        public:

        /** Default Constructor */
        PsmrtsPLYFormat() : m_ply_source(), m_mesh(), m_tracker() {}

        /** Construct a ply_file object */
        PsmrtsPLYFormat( const std::string &plyfile, const bool prefer_double = false )  {
            (void) load_ply_file( plyfile );
        }

        /** Destructor */
        virtual ~PsmrtsPLYFormat() {}

        virtual std::string format_model_source() {
            return ( ply_source() );
        }

        inline bool isValid() const {
            if ( !m_mesh.isValid() ) return (false);
            return ( true );
        }

        /** The PLY data source */
        inline const std::string &ply_source() const {
            return ( m_ply_source );
        }
    
        inline size_t nVertexes() const {
            return ( m_mesh.nvectors() );
        }

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
         * This function only obtains the 
         * 
         * @param plyfile       Name of PLY file to load
         * @param prefer_double If true, prefer double precision vertex data.
         *                        This may be possible for ascii, but makes no
         *                        sense for binary PLY data
         * @return true         If the load was successful
         * @return false        If the load failed
         */
        inline bool load_ply_file( const std::string &plyfile ) {

            // Init section
            m_ply_source = plyfile;
            m_mesh       = PsmrtsMeshData();  // Resets the state
            m_tracker    = PsmrtsThreadSafeCounter();

            // Open the file
            miniply::PLYReader reader( plyfile.c_str() );
            if ( !reader.valid()  ) {
                throw std::runtime_error("Failed to open file: " + plyfile );
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
            bool gotVerts = false, gotFaces = false;

            PsmrtsVector3d p_vectors;
            PsmrtsVector3i p_indexes;
            // miniply::PLYPropertyType vtype =  miniply::PLYPropertyType::Double;
            size_t vbytes = sizeof( double );

            while ( reader.has_element() && (!p_vectors.isValid()|| !p_indexes.isValid() ))  {

                if (reader.element_is(miniply::kPLYVertexElement) && reader.load_element() && reader.find_pos(indexes)) {
                  size_t  nverts  = reader.num_rows();
                  p_vectors = PsmrtsVector3d( nverts );
                  reader.extract_properties(indexes, 3, miniply::PLYPropertyType::Double,  p_vectors(0).data() );
                }
                else if ( !p_indexes.isValid() && reader.element_is(miniply::kPLYFaceElement) && reader.load_element()) {
                  size_t nindexes = reader.num_rows();
                  p_indexes = PsmrtsVector3i( nindexes );
                  reader.extract_properties(faceIdxs, 3, miniply::PLYPropertyType::Int,  p_indexes(0).data() );
                }

                reader.next_element();
            }
            // Check to see if we have the mesh -- throw error if vectors/indexes are invalid
            if ( !(p_vectors.isValid()  && p_indexes.isValid()) ) {
                throw std::runtime_error( "Vectors or Indexes are not valid in: " + plyfile);
            }

            // Allocate a mesh now    
            m_mesh = PsmrtsMeshData( p_indexes, p_vectors);

            return ( m_mesh.isValid() );
        }

        /**
         * @brief Conversion of ply file header data to json 
         * 
         * Used in load_ply_file() to create a json version of the header data,
         * providing information on elements, their properties, types, and sizes.
         * 
         * @param reader miniply file reader 
         */
        inline void parse_config(miniply::PLYReader& reader) {
            nlohmann::json j_result;
            for (uint32_t i=0; i < reader.num_elements(); i++) {
                const miniply::PLYElement* elem = reader.get_element(i);

                nlohmann::json j_element;
                j_element["element"] = elem->name;
                j_element["size"]    = elem->count;

                nlohmann::json j_properties_list = nlohmann::json::array();
                for(const miniply::PLYProperty& prop : elem->properties) {
                    nlohmann::json j_property;
                    if (prop.countType != miniply::PLYPropertyType::None) {
                        j_property["property"] = prop.name;
                        j_property["type"] = property_type_string(prop.type);
                        j_property["list_count_type"] = property_type_string(prop.countType);
                        j_properties_list.push_back(j_property);
                    }
                    else {
                        j_property["property"] = prop.name;
                        j_property["type"] = property_type_string(prop.type);
                        j_properties_list.push_back(j_property);
                    }
                }
                j_element["properties"] = j_properties_list;

                j_result["elements"].push_back(j_element);
            }
            m_json = j_result;
            return;
        }

        /**
         * @brief String output helper for miniply property types
         * 
         * This method is used to convert the appropriate ply type to a string version
         * when building the json header output for the reader in parse_config().
         * 
         * @param prop ply property type, as defined by miniply
         * @return std::string 
         */
        inline std::string property_type_string(miniply::PLYPropertyType prop) {
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

        /** Returns the header json information */
        inline  json config() {
            return m_json;
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
            std::string                         m_ply_source;
            PsmrtsMeshData                      m_mesh;
            PsmrtsThreadSafeCounter             m_tracker;
            std::string                         m_print;
            json                                m_json;

    };
} // namespace psmrts

#endif // PsmrtsPLYFormat_hpp
