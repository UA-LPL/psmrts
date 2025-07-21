#ifndef PsmrtsDSKFormat_hpp
#define PsmrtsDSKFormat_hpp

#include <string>
#include <memory>
#include <exception>
#include <iostream>

#include <Eigen/Geometry>
#include <nlohmann/json.hpp>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsVector3.hpp>
#include <PsmrtsMeshData.hpp>
#include <DskKernelModel.hpp>


namespace psmrts {

    /**
     * @brief PsmrtsDSKFormat contains tools for DSK (bds) file format I/O
     * 
     * @author Kris J. Becker and Kyle A. Becker, University of Arizona
     * @history 2025-2-12 Kyle A. Becker   Original Version
     */

    class PsmrtsDSKFormat {
        public:

        /** Default Constructor */
        PsmrtsDSKFormat() {}

        /** Construct a DSK object */
        PsmrtsDSKFormat( const std::string &dskfile ) {
            (void) load_dsk_file( dskfile ); 
        }

        /** Destructor */
        virtual ~PsmrtsDSKFormat() {}

        /** Returns the data source of the format model - DSK filepath */
        virtual std::string format_model_source() const {
            return ( dsk_source() );
        }

        /** Returns validity state of the object's mesh */
        inline bool isValid() const {
            if ( !m_mesh.isValid() ) return ( false );
            return ( true );
        }

        /** The DSK data source */
        inline const std::string &dsk_source() const {
            return ( m_dsk_source );
        }

        /** Returns the number of vertexes */
        inline size_t nVertexes() const {
            return ( m_mesh.nvectors() ); 
        }

        /** Returns the number of indexes */
        inline size_t nIndexes() const {
            return ( m_mesh.nfacets() ); 
        }

        /** 
         * @brief Loads a DSK file mesh data
         * 
         * This is the main method that will load a mesh based on a DSK
         * file. It retrieves and allocates appropriate index and double
         * vector data. It is advisable to input the correct pathing
         * prefaced by the PsmrtsUtilities' psmrts_formats_path function.
         * 
         * @param dskfile      Absolute path string of the target DSK file
         * @return true        If the load was successful
         * @return false       If the load failed
         */
        inline bool load_dsk_file( const std::string &dskfile ) {
            m_dsk_source = dskfile;
            m_mesh = PsmrtsMeshData();

            naif::DskKernelModel d_model( dskfile );

            PsmrtsVector3i dsk_indexes = d_model.load_facet_indexes();
            PsmrtsVector3d dsk_vectors = d_model.load_facet_vectors();

            m_mesh = PsmrtsMeshData( dsk_indexes, dsk_vectors );

            parse_config( d_model ); 

            return (m_mesh.isValid() );
        }
        
        /** Returns Psmrts Mesh of DSK Index / Vector data */
        inline PsmrtsMeshData get_mesh( const bool make_it_a_double = false ) const {
            if ( true == make_it_a_double ) {
                if ( !m_mesh.isVectorDouble() ) {
                    return ( PsmrtsMeshData( this->get_indexes(), this->get_double_vectors() ) );
                }
            }
            return (m_mesh);
        }

        /** Get a double precision vector array */
        inline PsmrtsVector3d get_double_vectors() const {
            PsmrtsVector3d v_vectors = m_mesh.vectors().double_vectors();
            if (m_mesh.vectors().isFloat()) {
                v_vectors = vector_to_type<PsmrtsVector3d::value_type>(m_mesh.vectors().float_vectors() );
            }

            return ( v_vectors );
        }

        /** Get a float precision vector array */
        inline PsmrtsVector3f get_float_vectors() const {
            PsmrtsVector3f v_vectors = m_mesh.vectors().float_vectors();
            if ( m_mesh.vectors().isDouble() ) {
                v_vectors = vector_to_type<PsmrtsVector3f::value_type>( m_mesh.vectors().double_vectors() );
            }
             
            return ( v_vectors );
        }

        /** Get index buffer from DSK file mesh */
        inline PsmrtsVector3i get_indexes() const {
            return ( m_mesh.indexes() );
        }

        inline void parse_config( naif::DskKernelModel &model ) {
            nlohmann::ordered_json j_result = nlohmann::ordered_json::object();
            j_result["header"]["file"] = m_dsk_source;
            int seg_num = model.n_dsk_segments();
            j_result["header"]["nSegments"] = seg_num;

            nlohmann::ordered_json j_segments = nlohmann::ordered_json::array();
            // JSON: [Segment(s)]. (Get list of Segments - DskSegmentList &segments)
            for (int i=0; i < seg_num; i++ ) {
                const naif::DskSegment seg = model.segment(i);

                nlohmann::ordered_json j_segment;
                j_segment["number"]   = seg.segment_number();
                j_segment["id"]       = seg.id();
                j_segment["vertices"] = seg.n_vertices();
                j_segment["plates"]   = seg.n_plates();
                j_segment["bodyId"]   = seg.bodyid();
                j_segment["frameId"]  = seg.frameid();
                j_segment["dtype"]    = seg.dtype();
                j_segment["dclass"]   = seg.dclass();

                j_segments.push_back(j_segment);
            } 
            j_result["segments"] = j_segments;

            m_config = j_result;
            return;
        }

        inline const ordered_json &config() const {
            return m_config;
        }

        private:
            std::string           m_dsk_source;
            PsmrtsMeshData        m_mesh;
            ordered_json          m_config;
    };
} // namespace psmrts

#endif // PsmrtsDSKFormat_hpp
