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

#include "tinyply.h"


namespace psmrts {
    /**
     * @brief PsmrtsPLYFormat contains tools for PLY file format I/O
     * 
     * @author Kyle A. Becker, University of Arizona
     * @history 2024-06-21
     */
    class PsmrtsPLYFormat {
        public:
          typedef tinyply::Type                   tinyply_type;
          typedef PsmrtsVector3<tinyply::Type>    PLYVectorArray;
          typedef PsmrtsVector3i                  PLYIndexArray;

          typedef PLYVectorArray::vector_type     PLYVectorType;
          typedef PLYIndexArray::vector_type      PLYIndexType;

        /** Default Constructor */
        PsmrtsPLYFormat() : m_ply_source() {}

        /** Construct a manipulatable ply_file object */
        PsmrtsPLYFormat( const std::string &plyfile ) :
                        m_ply_source(),
                        m_ply_file()  {

            m_ply_source = plyfile;
            m_ply_file = read_ply_file(plyfile);
                        }

        /** Destructor */
        virtual ~PsmrtsPLYFormat() {}


        virtual std::string format_model_source() {
            return ( ply_source() );
        }

        /**
         * @brief Load the contents of a PLY file
         * 
         * This method loads the contents of a file that contains data
         * compatible with the PLY file format. Please see:
         * https://github.com/ddiakopoulos/tinyply/tree/master/source
         * 
         * It returns a shared pointer to a tinyply::PlyFile object that
         * contains the contents of the "filename" parameter. A small error
         * check is included, and will output a corresponding message with
         * the parameter filename if unsuccessful.
         * 
         * @param filename 
         * @return std::shared_ptr<tinyply::PlyFile> 
         */
        static inline std::shared_ptr<tinyply::PlyFile> read_ply_file(const std::string& filename) {
            std::shared_ptr<tinyply::PlyFile> ply_file = std::make_shared<tinyply::PlyFile>();
            try {
                std::ifstream file_stream(filename, std::ios::binary);
                if (!file_stream) {
                    throw std::runtime_error("Falied to open file: " + filename );
                }

                ply_file->parse_header(file_stream);
            }
            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
            return ply_file;
        };

       
        inline bool isValid() const {
            if (!m_ply_file) return (false);
            return ( true );
        }

        /** The PLY data source */
        inline const std::string &ply_source() const {
            return ( m_ply_source );
        }
    
        inline size_t nElements() const {
            return m_ply_file->get_elements().size();
        }

        inline size_t nVertexes() const {
            std::shared_ptr<tinyply::PlyData> vertices;
            vertices = m_ply_file->request_properties_from_element( "vertex", { "x", "y", "z"} );
            return vertices->count;
        }

        inline size_t nIndexes() const {
            std::shared_ptr<tinyply::PlyData> indexes;
            indexes = m_ply_file->request_properties_from_element( "face", {"vertex_indices"} );
            return indexes->count;
        }

        
        inline std::string print_file() {
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
    
        inline PsmrtsVector3d get_double_vectors() const {
            std::shared_ptr<tinyply::PlyData> vertices;
            vertices = m_ply_file->request_properties_from_element( "vertex", { "x", "y", "z"} );
            const double* vertex_buffer = reinterpret_cast<double*>(vertices->buffer.get());
            PsmrtsVector3d out_vectors( vertices->count );

            for ( size_t i = 0; i < vertices->count; i++ ) {
                out_vectors( i ) = Eigen::Vector3d( { vertex_buffer[i * 3 + 0], vertex_buffer[i * 3 + 1], vertex_buffer[i * 3 + 2]});
            }

            return ( out_vectors );
        }
        
        inline PsmrtsVector3d get_float_vectors() const {
            std::shared_ptr<tinyply::PlyData> vertices;
            vertices = m_ply_file->request_properties_from_element( "vertex", { "x", "y", "z"} );
            const float* vertex_buffer = reinterpret_cast<float*>(vertices->buffer.get());
            PsmrtsVector3d out_vectors( vertices->count );

            for ( size_t i = 0; i < vertices->count; i++ ) {
                out_vectors( i ) = Eigen::Vector3d( { vertex_buffer[i * 3 + 0], vertex_buffer[i * 3 + 1], vertex_buffer[i * 3 + 2]});
            }

            return ( out_vectors );
        }


        // WIP...
        inline PsmrtsVector3i get_indexes() const {
            std::shared_ptr<tinyply::PlyData> indices;
            indices = m_ply_file->request_properties_from_element( "face", { "vertex_indices" }, 3 );
            const int* index_buffer = reinterpret_cast<int*>(indices->buffer.get());
            PsmrtsVector3i out_indexes( indices->count );

            size_t index = 0;
            while (index < indices->count) {
                // Nubmer of vertices in current face
                const int* vertex_count_ptr = reinterpret_cast<const int*>(index_buffer + index );
                int num_vertices = *vertex_count_ptr;
                if ( num_vertices == 3 ) {
                    int v0 = *( index_buffer + index + 1 );
                    int v1 = *( index_buffer + index + 2 );
                    int v2 = *( index_buffer + index + 3 );
                    out_indexes( index ) = Eigen::Vector3i( {v0, v1, v2} );
                }
                // in case more?
                index += 1 + num_vertices;
            }
            return ( out_indexes );
        }
        /**
         * @brief Accessing m_ply_file Data:
         * 
         * elements -
         * for (const auto& element : ply_file->get_elements()) {
         *      std::cout << "element: " << element.name << " (" << element.size << " instances" << std::endl;
         * }
         * 
         * properties -
         * for (const auto& property : element.properties) {
         *      std::cout << "property: " << property.name << " (type: ";
         *      std::cout << tinyply::PropertyTable[property.propertyType].str;
         *      if (property.isList) {
         *          std::cout << ", list count type: " << tinyply::PropertyTable[property.listType].str;
         *      }
         *      std::cout << ")" <<  std::endl;
         * }
         * 
         */

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
            std::shared_ptr<tinyply::PlyFile>   m_ply_file;
            std::shared_ptr<tinyply::PlyData>   vertices;
            PsmrtsThreadSafeCounter             m_tracker;
    };
} // namespace psmrts

#endif // PsmrtsPLYFormat_hpp
