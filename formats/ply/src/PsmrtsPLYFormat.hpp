#ifndef PsmrtsPLYFormat_hpp
#define PsmrtsPLYFormat_hpp

#include <string>
#include <memory>
#include <exception>
#include <iostream>

#include <Eigen/Geometry>

#include <PsmrtsDataModel.hpp>
#include <PsmrtsMeshData.hpp>
#include <PsmrtsRayTrace.hpp>

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
            typedef PsmrtsDataModel<>   PlyVectorData;
            typedef PsmrtsDataModel<int>    PlyIndexData;

            typedef PlyVectorData::vector_type  PlyVectorType;
            typedef PlyIndexData::vector_type   PlyIndexType;

            /** Default Constructor - placeholder */
            PsmrtsPLYFormat() : m_ply_source(), m_ply_reader() { }

            /** Read and Construct from file */
            PsmrtsPLYFormat( const std::string &plyfile ) {
                std::unique_ptr<std::istream> file_stream;
                PlyFile file;
                m_ply_source = plyfile;

                file_stream.reset(new std::ifsteam(m_ply_source, std::ios:binary ) );

                file.parse_header(*file_stream);

                for (int i = 0; i < file.size(); i++ ) {
                    PlyVectorData(i) = file.getElements()[i];
                }
                
            }


        protected:

            private:
                std::string     m_ply_source;
                std::shared_ptr<tinyply::PlyReader> m_ply_reader;
    };
} // namespace psmrts

#endif // PsmrtsPLYFormat_hpp