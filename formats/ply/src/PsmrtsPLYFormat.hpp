#ifndef PsmrtsPLYFormat_hpp
#define PsmrtsPLYFormat_hpp

#include <string>
#include <memory>
#include <exception>
#include <iostream>
#include <fstream>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsBuffer.hpp>
#include <PsmrtsVector3.hpp>
#include <PsmrtsMeshData.hpp>

#include "miniply.h"


namespace psmrts {

    class PsmrtsPLYFormat {
        public:
            PsmrtsPLYFormat( const std::string &plyfile ) : m_ply_reader(plyfile.c_str()) {
                if (!m_ply_reader.valid()) {
                    throw std::runtime_error("Failed to open file..");
                }
                m_ply_source = plyfile;
            }

            virtual ~PsmrtsPLYFormat() {}
    

    inline const std::string &ply_source() {
        return (m_ply_source);
    }

    inline bool isValid() {
        return (m_ply_reader.valid());
    }


    inline std::string extract_info() {
        m_mesh = PsmrtsMeshData();

        uint32_t vertexLoc = m_ply_reader.find_element( "vertex" );
        miniply::PLYElement *vectors = m_ply_reader.get_element(vertexLoc);
        std::vector<uint8_t> x_prop_data;
        std::vector<uint8_t> y_prop_data;
        std::vector<uint8_t> z_prop_data;

        std::vector<miniply::PLYProperty> props = vectors->properties;
        auto prop_size = props.size();

        return std::to_string(prop_size);



        /*
        std::string print_vex = std::to_string(vectors->count);
        size_t nvectors = vectors->count;
        size_t nbytes = vectors->rowStride;

        PsmrtsBufferData b_raw( vectors->properties[0].listData.data(), nbytes );
        PsmrtsStridingBuffer b_data( b_raw, nbytes / 3);

        return print_vex;
        */
    }
     

    inline std::string print_info() {
        /** 
        inline static const char* kFileTypes[] = {
                "ascii",
                "binary_little_endian",
                "binary_big_endian",
                };
        inline static const char* kPropertyTypes[] = {
                "char",
                "uchar",
                "short",
                "ushort",
                "int",
                "uint",
                "float",
                "double",
                };
        */
        std::string data_info = "";

        data_info += "ply\n";
        for (uint32_t i = 0; i < m_ply_reader.num_elements(); i++ ) {
            const miniply::PLYElement* elem = m_ply_reader.get_element(i);
            data_info += elem->name;
            data_info +=  ": " + std::to_string(elem->count) + "\n";
            for(const miniply::PLYProperty& prop : elem->properties) {
                data_info += "Property List ";
                data_info += prop.name;
                switch(uint32_t(prop.countType)) {}
    
                data_info += " Count type: " + std::to_string(uint32_t(prop.countType));
                data_info += ", Type: " + std::to_string(uint32_t(prop.type)) + "\n";
            }
        }

        return data_info;
    }





    private:
        std::string        m_ply_source;
        miniply::PLYReader m_ply_reader;
        PsmrtsMeshData     m_mesh;
    };
};
#endif // PsmrtsPLYFormat_hpp
/*
namespace psmrts {
    
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
     
    class PsmrtsPLYFormat {
        public:

         Default Constructor 
        PsmrtsPLYFormat() : m_ply_source(), m_ply_file(), m_mesh(), m_tracker() {}

         Construct a ply_file object 
        PsmrtsPLYFormat( const std::string &plyfile) { //, const bool prefer_double = false )  {
            (void) load_ply_file( plyfile, prefer_double );
        }

         Destructor 
        virtual ~PsmrtsPLYFormat() {}

        virtual std::string format_model_source() {
            return ( ply_source() );
        }

        inline bool isValid() const {
            if ( !m_ply_file ) return (false);
            return ( true );
        }

        The PLY data source 
        inline const std::string &ply_source() const {
            return ( m_ply_source );
        }
    
        inline size_t n_elements() const {
            if (!m_ply_file) { return 0; }
            return ( m_ply_file->get_elements().size() );
        }

        inline size_t nVertexes() const {
            return ( m_mesh.nvectors() );
        }

        inline size_t nIndexes() const {
            return ( m_mesh.nfacets() );
        }

        
         * @brief Load header contents of a PLY file - no data!
         * 
         * This method loads the contents ofa PLY file header only.
         * Please see:
         * https://github.com/ddiakopoulos/tinyply/tree/master/source
         * 
         * It returns a shared pointer to a tinyply::PlyFile object that
         * contains the header contents of the "filename" parameter. To
         * retrieve the elements will require you to re-open the file
         * and issue a PlyFile->read( file_stream ) after requesting
         * the file elements.
         * 
         * The input PLY file is closed upon return from this function.
         * 
         * See also load_ply_file()
         * 
         * @param filename 
         * @return std::shared_ptr<tinyply::PlyFile> 
         
        static inline std::shared_ptr<tinyply::PlyFile> read_ply_file(const std::string& filename) {
            std::shared_ptr<tinyply::PlyFile> ply_file;
            try {
                std::ifstream file_stream(filename, std::ios::binary | std::ios::in);
                if (!file_stream.is_open()) {
                    throw std::runtime_error("Falied to open file: " + filename );
                }

                // Read header only
                ply_file = std::make_shared<tinyply::PlyFile>();
                ply_file->parse_header( file_stream );
            }
            catch (const std::exception& e) {
                std::string mess = "PsmrtsPLYFormat::read_plt_file() - failed to read PLY file " + filename;
                mess += "\nError: " + std::string(e.what());
                throw std::runtime_error(mess);
            }
            return ( ply_file );
        };

       
        
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
         
        inline bool load_ply_file( const std::string &plyfile, const bool prefer_double = false ) { // set to true, if txt file

            // Init section
            m_ply_source = plyfile;
            m_ply_file   = std::make_shared<tinyply::PlyFile>( );
            m_mesh       = PsmrtsMeshData();  // Resets the state
            m_tracker    = PsmrtsThreadSafeCounter();

            // Open the file for ereading
            std::ifstream file_stream(plyfile, std::ios::binary);
            if (!file_stream) {
              throw std::runtime_error("Falied to open file: " + plyfile );
            }

            m_ply_file->parse_header( file_stream );            
            // std::cout << print_file() << std::endl;

            // *Request* the facets and vectors. Note the buffers are not valid
            // until the file is read (below) after the requests are made!
            auto facets   = m_ply_file->request_properties_from_element( "face", {"vertex_indices"}, 0 );            
            auto vertices = m_ply_file->request_properties_from_element( "vertex", { "x", "y", "z"} );

            // Lets see if we can force a double precision read of asciii vertex data
            
            
            if ( !m_ply_file->is_binary_file() ) {
                if ( true == prefer_double ) {
                  vertices->t = tinyply::Type::FLOAT64;
                }
            }
            
            // Reading...
            m_ply_file->read( file_stream );

            // Extract the facets
            PsmrtsVector3i v_facets= extract_vectors<PsmrtsVector3i::value_type> ( *facets );


            // Extract the vectors with whatever type you would like.
            if ( vertices->t == tinyply::Type::FLOAT32 ) {
                PsmrtsVector3f v_vertices_f = extract_vectors<PsmrtsVector3f::value_type> ( *vertices );
                // std::cout << "Extracted FLOAT vertices: " << v_vertices_f.size() << std::endl;
                m_mesh = PsmrtsMeshData( v_facets, v_vertices_f );
            }
            else if ( vertices->t == tinyply::Type::FLOAT64 ) {
                PsmrtsVector3d v_vertices_d = extract_vectors<PsmrtsVector3d::value_type> ( *vertices );
                // std::cout << "Extracted DOUBLE vertices: " << v_vertices_d.size() << std::endl;
                m_mesh = PsmrtsMeshData( v_facets, v_vertices_d );           
            }

            return ( m_mesh.isValid() );
        }
        

        
         * @brief Conversion/extraction of PLY vector data types
         * 
         * @tparam TO_T   Template parameter of type to get data from
         * @param pdata    Input buffer data to extract/convert
         * @return PsmrtsVector3<TO_T>  Output vector array of converted data
         
        template <typename TO_T> 
          PsmrtsVector3<TO_T> extract_vectors( tinyply::PlyData &pdata ) const {
             
            PsmrtsVector3<TO_T> v_to_t;  // Output data buffer
            size_t nvectors = pdata.count;
            size_t nbytes   = pdata.buffer.size_bytes();
            tinyply::PropertyInfo p_property = tinyply::PropertyTable[pdata.t];

            // Gonna sanity check this buffer
            if ( ( pdata.buffer.get() == nullptr ) || ( nbytes == 0 ) ) {
                std::string mess = "PsmrtsPLYFormat::extract_vectors() - No data in PlyData buffer - perhaps was not read properly from " + m_ply_source;
                throw std::runtime_error( mess );
            }

            // Create mapping buffers - no data is copied here
            PsmrtsBufferData b_raw( pdata.buffer.get(), nbytes );        // Map reference to PLY pdata.Buffer
            PsmrtsStridingBuffer b_data( b_raw, p_property.stride * 3 ); // Config for 3-element vector with property type size

            // Convert the PLY buffer to the type TO_T
            switch ( pdata.t ) {
                case tinyply::Type::INT8:  {
                    v_to_t = vector_to_type<TO_T> ( PsmrtsVector3<int8_t>( b_data ) );
                    break;
                }   
                case tinyply::Type::UINT8:   {
                    v_to_t = vector_to_type<TO_T> ( PsmrtsVector3<uint8_t>( b_data ) );
                    break;
                }   
                case tinyply::Type::INT16:   {
                    v_to_t = vector_to_type<TO_T> ( PsmrtsVector3<int16_t>( b_data ) );
                    break;
                }
                case tinyply::Type::UINT16:   {
                    v_to_t = vector_to_type<TO_T> ( PsmrtsVector3<uint16_t>( b_data ) );
                    break;
                }
                case tinyply::Type::INT32:  {
                    v_to_t = vector_to_type<TO_T> ( PsmrtsVector3<int32_t>( b_data ) );
                    break;
                }
                case tinyply::Type::UINT32:   {
                    v_to_t = vector_to_type<TO_T> ( PsmrtsVector3<uint32_t>( b_data ) );
                    break;
                }
                case tinyply::Type::FLOAT32: {
                    v_to_t = vector_to_type<TO_T> ( PsmrtsVector3<float>( b_data ) );
                    break;
                }
                case tinyply::Type::FLOAT64:  {
                    v_to_t = vector_to_type<TO_T> (PsmrtsVector3<double>( b_data ) );
                    break;
                }
                case tinyply::Type::INVALID:
                default:    
                  throw std::invalid_argument("invalid ply property");
                  break;
            }

            return ( v_to_t );
          }


         Perhaps return a std::vector<std::string> array of these properties or convert them to JSON? Yes! 
        inline std::string print_file() {
            if (!m_ply_file) { return "No File Allocated - Bad Print"; }
            std::string result = "";
            // elements
            for (const auto& element : m_ply_file->get_elements()) {
                result += "element: " + element.name + " (" + std::to_string(element.size) + " instances)\n";
                for (const auto& property : element.properties) {
                    result += "     property: " + property.name + " (type: ";
                    result += tinyply::PropertyTable[property.propertyType].str;
                    if (property.isList) {
                        result += ", list count type: " + tinyply::PropertyTable[property.listType].str;
                    }
                    result += ")\n";
                }
            }
            return result;
        }

         Convert ply header data to JSON 
        inline void ply_to_json( json &j ) {
            if (!m_ply_file) { j = json(); }

            nlohmann::json result;

            for (const auto& element : m_ply_file->get_elements()) {
                nlohmann::json j_element;
                j_element["element"] = element.name;
                j_element["size"]    = element.size;

                nlohmann::json j_properties_list = nlohmann::json::array();
                for (const auto& property : element.properties) {
                    nlohmann::json j_property;
                    j_property["property"] = property.name;
                    j_property["type"] = tinyply::PropertyTable[property.propertyType].str;
                    if (property.isList) {
                        j_property["list_count_type"] = tinyply::PropertyTable[property.listType].str;
                    }
                    j_properties_list.push_back(j_property);
                }
                j_element["properties"] = j_properties_list;

                result["elements"].push_back(j_element);
            }
            j = result;

            return;
        }
    
        Returns the mesh as read from the file unless true is provided which returns doubles 
        inline PsmrtsMeshData get_mesh( const bool make_it_a_double = false ) const  {
            if ( true == make_it_a_double ) {
                if ( !m_mesh.isVectorDouble() ) {
                    return ( PsmrtsMeshData( this->get_indexes(), this->get_double_vectors() ) );
                }
            }

            return ( m_mesh );

        }


         Get a double precision vector array  
        inline PsmrtsVector3d get_double_vectors() const {
            PsmrtsVector3d v_vectors = m_mesh.vectors().double_vectors();
            if ( m_mesh.vectors().isFloat() ) {
                v_vectors = vector_to_type<PsmrtsVector3d::value_type>( m_mesh.vectors().float_vectors() );
            }

            return ( v_vectors );
        }
        
         Get a float precision vector array  
        inline PsmrtsVector3f get_float_vectors() const {
            PsmrtsVector3f v_vectors = m_mesh.vectors().float_vectors();
            if ( m_mesh.vectors().isDouble() ) {
                v_vectors = vector_to_type<PsmrtsVector3f::value_type>( m_mesh.vectors().double_vectors() );
            }

            return ( v_vectors );
        }


         Get index buffer extracted from PLY file 
        inline PsmrtsVector3i get_indexes() const {
            return ( m_mesh.indexes() );
        }

        inline double elapsed_life_time_s() const {
            return (m_tracker.runtime_s() );
        }

        inline size_t track_count() const {
            return (m_tracker.count() );

        }

        
         * @brief Return a standalone clone of the currrent trackerr stats
         * 
         * Get a snapshot of the performance at this moment. I'd immediately get
         * an end_time = system_clock_time
         * 
         * @return PsmrtsThreadSafeCounter
         
        inline PsmrtsThreadSafeCounter performance_snapshot() {
            return ( m_tracker.clone() );
        }

        inline const std::shared_ptr<tinyply::PlyFile> &fetch_ply_file() const {
            return ( m_ply_file );
        }
        
        private:
            std::string                         m_ply_source;
            std::shared_ptr<tinyply::PlyFile>   m_ply_file;
            PsmrtsMeshData                      m_mesh;
            PsmrtsThreadSafeCounter             m_tracker;

            // miniply
            miniply::PLYReader m_ply_reader;

    };
} // namespace psmrts

#endif // PsmrtsPLYFormat_hpp
*/